#include <mcp_can.h>
#include <mcp_can_dfs.h>

#include <Battery.h>

#include <PinChangeInterrupt.h>
#include <PinChangeInterruptBoards.h>
#include <PinChangeInterruptPins.h>
#include <PinChangeInterruptSettings.h>

#include <SPI.h>
#include "mcp_can.h"
#include <avr/sleep.h>
#include "CanFrame.h"
#include <PinChangeInterrupt.h>
#include <Battery.h>
#include <EEPROM.h>

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// Adjusted parameters
#define VCC 5060                             // Vcc, measure on arduino
// #define VOLTAGE_DIVIDER 8.6642335766         // Voltage divider ratio (R1 + R2) => (R2 17,7+105)/13,7
#define VOLTAGE_DIVIDER 8.7062986965
#define AMPERE_SENSOR_OFFSET 512             // 512 by defoult
#define AMP_DIRECRION -1;                     // current direction, depends on battery in+/out+ connection; 1/-1
#define COULOMB_CAPACITY 47500



//#define P 1.112500                      //  1.1125 minimalnie za duzo
//#define P 1.112000                      //  1.112 minimalnie za duzo
//#define P 1.111500                      //  1.1115 minimalnie za malo
//#define P 1.111750                      //  1.11175 minimalnie za malo
//#define P 1.111875                     //  1.11185 minimalnie za malo
//#define P 1.111925                        //  1.111925 minimalnie za malo
//#define P 1.111999                        //  111999  minimalnie za malo
// #define P 1.112025                        // to adjust voltage drop gap on load battery
#define INITIAL_DELAY 3000                // time for charging capasitors

//--------------------------------------------------------------------------
// Initial data
const int LED1 = 3;
const int LED2 = 4;
const int LED3 = 5;
const int LED4 = 6;
const int LED5 = 7;
const int Switch = 8;

const int SPI_CS_PIN = 10;

//--------------------------------------------------------------------------
// Data for interupt
#define CAN_INT 2                     // Set INT to pin 2

#define RS_TO_MCP2515 true            // Set this to false if Rs is connected to your Arduino
#define RS_OUTPUT MCP_RX0BF           // RX0BF is a pin of the MCP2515. You can also define an Arduino pin here

MCP_CAN CAN(SPI_CS_PIN);              // Set CS pin
                             
#define KEEP_AWAKE_TIME 2000          // time the controller will stay awake after the last activity on the bus (in ms)
unsigned long lastBusActivity = millis();

unsigned char flagRecv = 0;
unsigned char len = 0;
unsigned char buf[8];

//--------------------------------------------------------------------------
// Data for duplicate message check below
int lastLen = -1;
unsigned char lastBuf[8];                                    
unsigned long lastMsgTime = 0;
#define DUPLICATE_TIMEOUT 20

//--------------------------------------------------------------------------
// Data for check battery level
#define BAT_MIN 31000
#define BAT_MAX 41000
#define BAT_ARR_SIZE 256                 // battery level array size, must be power of 2

Battery battery(BAT_MIN, BAT_MAX, A2);
unsigned char batLevel = 0;            // actual percentage battery level
unsigned int batArray[BAT_ARR_SIZE];
unsigned int batteryVoltage;

//--------------------------------------------------------------------------
// Data for current mearsurement
#define AMPERE_ARR_SIZE 4               // current array size, must be power of 2

int chargingIter = 0;                   // for LED blinking while charging
int ampereSensorPin = A0;

double avgAmpereValue = 0;                 // initial value
double dropGap = 0;
int miliAmperValue = 0;

double ampereArray[AMPERE_ARR_SIZE];


//--------------------------------------------------------------------------
// Data for coulomb counter
#define COULOMB_AMPERE_ARR_SIZE 4               // current coulomb array size, must be power of 2
double coulumb = 0;
unsigned int coulumbRound = 0;
unsigned char  coulumbPercentage = 0;
double avgCoulAmpereValue = 0;
double coulAmpereArray[COULOMB_AMPERE_ARR_SIZE];

//--------------------------------------------------------------------------
// Data for drop voltage equation
// #define AX2 -3.3567     // base values for ax2 + bx + c equation
// #define BX 193.5095
// #define C -1.3374

// double ax2 = AX2 * P;
// double bx = BX * P;
// double c = C * P;
// #define P 2 // za dużo ok 5-6%
// #define P 1.7   // za dużo
// #define P 1.5      // za malo
// #define P 1.6   // za dużo ok 1-2%
#define P 1.57

#define AX 177.4866
#define B 4

double ax = AX * P;
double b = B * P;

//--------------------------------------------------------------------------
// Data for time checking
unsigned long actTime = 0;
unsigned long prevTime = 0;

//--------------------------------------------------------------------------
// Intiaize CAN frames to send
CanFrame frame580(0x580, 0, 3, data580);
CanFrame frame581(0x581, 0, 7, data581);
CanFrame frame582(0x582, 0, 4, data582);
CanFrame frame583(0x583, 0, 5, data583);
CanFrame frame584(0x584, 0, 4, data584);
CanFrame frame590(0x590, 0, 2, data590);
CanFrame frame591(0x591, 0, 8, data591);
CanFrame frame592(0x592, 0, 6, data592);
CanFrame frame593(0x593, 0, 8, data593);
CanFrame frame594(0x594, 0, 4, data594);
CanFrame frame595(0x595, 0, 6, data595);
CanFrame frame59A(0x59A, 0, 6, data59A);
CanFrame frame59B(0x59B, 0, 2, data59B);
CanFrame frame59F(0x59F, 0, 1, data59F);
CanFrame frame780(0x780, 0, 3, data780);
CanFrame frame781(0x781, 0, 7, data781);
CanFrame frame782(0x782, 0, 4, data782);
CanFrame frame783(0x783, 0, 5, data783);
CanFrame frame784(0x784, 0, 3, data784);

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void setup()
{
    Serial.begin(9600);
    CAN.begin(CAN_500KBPS, MCP_8MHz);
    battery.begin(VCC, VOLTAGE_DIVIDER);
    CAN.setMode(MODE_NORMAL);
    delay(INITIAL_DELAY);                       // wait for charge capacitor
    for (int i=0;i<BAT_ARR_SIZE;i++)            // initaial values of battery level
    {
      batArray[i] = battery.level();
    }

    batLevel = calculateAverage(batArray, BAT_ARR_SIZE);     // calculate average
    if (batLevel > 100)
    {
      batLevel = 100;
    }
    coulumb = (100-batLevel) * COULOMB_CAPACITY*0.01;

    //LEDs
    pinMode(Switch, INPUT_PULLUP);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(LED4, OUTPUT);
    pinMode(LED5, OUTPUT);
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, HIGH);
    digitalWrite(LED4, HIGH);
    digitalWrite(LED5, HIGH);
}

void MCP2515_ISR()
{
    flagRecv  = 1;
}

void loop()
{
  flagRecv = 0;                   // clear flag
  lastBusActivity = millis();
  actTime = millis();
  if (actTime - prevTime >= 1000UL)    // read battery level in every 1s

  {
    prevTime = actTime;

    // Current measurement and calculate voltage drop
    r_left_double(ampereArray, AMPERE_ARR_SIZE);
    ampereArray[AMPERE_ARR_SIZE-1] = (((analogRead(ampereSensorPin) - AMPERE_SENSOR_OFFSET)/25.6)+0.7)*AMP_DIRECRION;
    avgAmpereValue = calculateAverage_double(ampereArray, AMPERE_ARR_SIZE);
    // dropGap = ax2*avgAmpereValue*avgAmpereValue + 
    //           bx*avgAmpereValue +
    //           c;
    dropGap = ax*avgAmpereValue + b;   // linear f(x) = Ax + B

    // Voltage measurement and calculate battery level
    batteryVoltage = battery.voltage();
    r_left(batArray, BAT_ARR_SIZE);                                           // shift battery measurement array
    batArray[BAT_ARR_SIZE-1] = battery.level(batteryVoltage + dropGap);       // add current measurement to last element, plus battery drop on load
    batLevel = calculateAverage(batArray, BAT_ARR_SIZE);                      // calculate average

    // Coulomb counter 0 <-> COULOMB_CAPACITY
    r_left_double(coulAmpereArray, COULOMB_AMPERE_ARR_SIZE);
    coulAmpereArray[COULOMB_AMPERE_ARR_SIZE-1] = ampereArray[AMPERE_ARR_SIZE-1];
    avgCoulAmpereValue = calculateAverage_double(coulAmpereArray, COULOMB_AMPERE_ARR_SIZE);
    coulumb+=avgCoulAmpereValue;
    if (coulumb <= 0)
      coulumbRound = 0;
    else if (coulumb >= COULOMB_CAPACITY)
      coulumbRound = COULOMB_CAPACITY;
    else
      coulumbRound = round(coulumb);

    // Clculate percentage from coulomb counter, not use for now
    coulumbPercentage = 100 - floor(coulumb*(100.00 / COULOMB_CAPACITY));

    //  data781[0] = battery.level(batteryVoltage + dropGap);
    data781[0] = batLevel;
          
    data782[3] = highByte(batteryVoltage);
    data782[2] = lowByte(batteryVoltage);
    Serial.println(batteryVoltage);
    miliAmperValue = abs(avgAmpereValue)*10000;
    data782[1] = highByte(miliAmperValue);
    data782[0] = lowByte(miliAmperValue);

    // if (batLevel>=100){ //stop chargingwhen D1/D2 = D3/D4???
    //    data781[1] = 0xE3;
    //    data781[2] = 0x3D;
    // }

    // TMP just to check battery capacity
    // store Coulomb counter value in CAN 591 frame data
    data591[0] = 0x00;
    data591[1] = 0x00;
    data591[2] = 0x00;
    data591[4] = highByte(batteryVoltage);
    data591[3] = lowByte(batteryVoltage); //shov actual voltage

    if(ampereArray[AMPERE_ARR_SIZE-1] <= -0.5)  //   charging
    {
      charging(chargingIter);
      if (chargingIter<5)
        chargingIter++;
      else
        chargingIter=0;
    }
    else
    {
      chargingIter=0;
      charging(chargingIter);
    }
  copy_frames();
  }

  while (CAN_MSGAVAIL == CAN.checkReceive()) 
  {
    // read data,  len: data length, buf: data buf
    CAN.readMsgBuf(&len, buf);
    lastLen = len;
    memcpy(lastBuf, buf, sizeof(buf));
    lastMsgTime = millis();

    switch (CAN.getCanId())
    {
      case 0x020: //  0x40000020
        data59F[0] = 0x01;
        frame59F.sendCAN(CAN);
        break;
      case 0x120:
        data59F[0] = 0x00;
        frame59F.sendCAN(CAN);
        break;
      case 0x42C:
        frame590.sendCAN(CAN);
        frame591.sendCAN(CAN);
        frame592.sendCAN(CAN);
        break;
      case 0x22C:
        frame580.sendCAN(CAN);
        frame581.sendCAN(CAN);
        frame582.sendCAN(CAN);
        frame583.sendCAN(CAN);
        break;
      case 0x26C:
        frame580.sendCAN(CAN);
        frame581.sendCAN(CAN);
        frame583.sendCAN(CAN);
        break;
      case 0x52C:
        frame593.sendCAN(CAN);
        frame594.sendCAN(CAN);
        frame595.sendCAN(CAN);
        break;
      case 0x72C:
        frame59A.sendCAN(CAN);
        frame59B.sendCAN(CAN);
        break;
      case 0x641:
        frame780.sendCAN(CAN);
        frame781.sendCAN(CAN);
        frame782.sendCAN(CAN);
        frame784.sendCAN(CAN);
        break;
//charging
      // case 0x7C0:
      //   frame780.sendCAN(CAN);
      //   break;
      // case 0x7C1:
      //   frame781.sendCAN(CAN);
      //   break;
      // case 0x7C2:
      //   frame782.sendCAN(CAN);
      //   frame783.sendCAN(CAN);
      //   frame784.sendCAN(CAN);
      //   break;
      // case 0x2EC:
      //   frame580.sendCAN(CAN);
      //   frame581.sendCAN(CAN);
      //   frame582.sendCAN(CAN);
      //   frame583.sendCAN(CAN);
      //   break;
      default:
        break;
    }
  }
}

//shift left int array
void r_left(int *a,int n) 
{
  memmove(a,a+1,sizeof(int)*(n-1));
}

//calculate average from int array
int calculateAverage(int *ar, int size) 
{
  long long level = 0;
  int iter = size;
  int count = 0;
  for (int i=0;i<size;i++)
  {
    level+=ar[i];
  }
  while (iter!=1)
  {
      iter=iter>>1;
      count++;
  }
  return level >> count;
}

//shift left double array
void r_left_double(double *a,int n) 
{
  memmove(a,a+1,sizeof(double)*(n-1));
}

//calculate average from double array
double calculateAverage_double(double *ar, int size) 
{
  double value = 0;
  int iter = size;
  int count = 0;
  for (int i=0;i<size;i++)
  {
    value+=ar[i];
  }
  return value/size;
}

void checkBattery(int batLevel)
{
  if (batLevel >= 11) digitalWrite(LED1, LOW);
  if (batLevel >= 21) digitalWrite(LED2, LOW);
  if (batLevel >= 41) digitalWrite(LED3, LOW);
  if (batLevel >= 61) digitalWrite(LED4, LOW);
  if (batLevel >= 80) digitalWrite(LED5, LOW);
  if (batLevel <= 10) 
  {
    digitalWrite(LED1, LOW);delay(500);digitalWrite(LED1, HIGH);delay(500);
    digitalWrite(LED1, LOW);delay(500);digitalWrite(LED1, HIGH);delay(500);
    digitalWrite(LED1, LOW);delay(500);digitalWrite(LED1, HIGH);delay(500);
  }
  delay(1000);
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);
  digitalWrite(LED5, HIGH);
}

void charging(int chargingIter)
{
  switch (chargingIter)
  {
    case 1: digitalWrite(LED1, LOW); break;
    case 2: digitalWrite(LED2, LOW); break;
    case 3: digitalWrite(LED3, LOW); break;
    case 4: digitalWrite(LED4, LOW); break;
    case 5: digitalWrite(LED5, LOW); break;
    default: 
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);
      digitalWrite(LED4, HIGH);
      digitalWrite(LED5, HIGH);
      break;
  }
}

void copy_frames(){
  *(data580+0) = *(data780+0);
  *(data580+1) = *(data780+1);
  *(data580+2) = *(data780+2);

  *(data581+0) = *(data781+0);
  *(data581+1) = *(data781+1);
  *(data581+2) = *(data781+2);
  *(data581+3) = *(data781+3);
  *(data581+4) = *(data781+4);
  *(data581+5) = *(data781+5);
  *(data581+6) = *(data781+6);

  *(data582+0) = *(data782+0);
  *(data582+1) = *(data782+1);
  *(data582+2) = *(data782+2);
  *(data582+3) = *(data782+3);

  *(data583+0) = *(data783+0);
  *(data583+1) = *(data783+1);
  *(data583+2) = *(data783+2);
  *(data583+3) = *(data783+3);
  *(data583+4) = *(data783+4);

  *(data584+0) = *(data784+0);
  *(data584+1) = *(data784+1);
  *(data584+2) = *(data784+2);
  *(data584+3) = *(data784+3);
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/