#include <SPI.h>
#include "mcp_can.h"
#include <avr/sleep.h>
#include "CanFrame.h"
#include <PinChangeInterrupt.h>
#include <Battery.h>

const int LED1 = 3;
const int LED2 = 4;
const int LED3 = 5;
const int LED4 = 6;
const int LED5 = 7;
const int Switch = 8;

const int SPI_CS_PIN = 10;

//used for interupt
#define CAN_INT 2                     // Set INT to pin 2

#define RS_TO_MCP2515 true            // Set this to false if Rs is connected to your Arduino
#define RS_OUTPUT MCP_RX0BF           // RX0BF is a pin of the MCP2515. You can also define an Arduino pin here

MCP_CAN CAN(SPI_CS_PIN);              // Set CS pin
                             
#define KEEP_AWAKE_TIME 2000          // time the controller will stay awake after the last activity on the bus (in ms)
unsigned long lastBusActivity = millis();

unsigned char flagRecv = 0;
unsigned char len = 0;
unsigned char buf[8];

// used for duplicate message check below
int lastLen = -1;
unsigned char lastBuf[8];                                    
unsigned long lastMsgTime = 0;
#define DUPLICATE_TIMEOUT 20

// used for check battery level
#define BAT_MIN 32000
#define BAT_MAX 40500
Battery battery(BAT_MIN, BAT_MAX, A2);
unsigned char batLevel = 100;

#define BAT_ARR_SIZE 512              // battery level array size, must be power of 2
unsigned int batArray[BAT_ARR_SIZE];

int ampereSensorPin = A0;               // current measurement
double ampereValue = 0;                 // initial value
#define AMPERE_SENSOR_OFFSET 512
double dropGap = 0;
#define AMPERE_ARR_SIZE 16               // current array size, must be power of 2
double ampereArray[AMPERE_ARR_SIZE];
int chargingIter = 0;

//--------------------------------------------------------------------------
// double ax2 = -4.22;  // initaial values
// double bx = 264.48;
double c = 98.66;

// double ax2 = -4.69475;    // 1.1125 minimalnie za duzo
// double bx = 294.234;
// double c = 109.75925;

double ax2 = -4.69264;    // 1.112
double bx = 294.10176;
//--------------------------------------------------------------------------

unsigned long actTime = 0;
unsigned long prevTime = 0;

// int CAN frames to send
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

bool firstTime = true;

void setup()
{
    Serial.begin(9600);
    CAN.begin(CAN_500KBPS, MCP_8MHz);
    battery.begin(5160, 8.6559);  //9.41 -> 9.4098451042
    CAN.setMode(MODE_NORMAL);
    delay(3000);
    for (int i=0;i<BAT_ARR_SIZE;i++)
    {
      batArray[i] = battery.level();
    }

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

    // TODO: uncoment when interupts will works. For now works only on arduino UNO.
    /*
    // attach interrupt
    
    pinMode(CAN_INT, INPUT_PULLUP);
    attachPCINT(digitalPinToPCINT(CAN_INT), MCP2515_ISR, FALLING);

    CAN.setSleepWakeup(1);                                   // this tells the MCP2515 to wake up on incoming messages

    Pull the Rs pin of the MCP2551 transceiver low to enable it:


    if(RS_TO_MCP2515) 
    {
      CAN.mcpPinMode(MCP_RX0BF, MCP_PIN_OUT);
      CAN.mcpDigitalWrite(RS_OUTPUT, LOW);
    } else {
      pinMode(RS_OUTPUT, OUTPUT);
      digitalWrite(RS_OUTPUT, LOW);
    }
    */
}

void MCP2515_ISR()
{
    flagRecv  = 1;
}

void loop()
{
/* TODO: uncoment when interupts will works. For now works only on arduino UNO.

    if(flagRecv) 
    {             // check if get data
*/

        flagRecv = 0;                   // clear flag
        lastBusActivity = millis();
        actTime = millis();
        if (actTime - prevTime >= 1000UL)                                            // read battery level in every 1s

        {
          prevTime = actTime;
          r_left_double(ampereArray, AMPERE_ARR_SIZE);
          ampereArray[AMPERE_ARR_SIZE-1] = (analogRead(ampereSensorPin) - AMPERE_SENSOR_OFFSET)/-25.6;    //  -25.6 becouse of wrong wire connection
          ampereValue = calculateAverage_double(ampereArray, AMPERE_ARR_SIZE);
          dropGap = ax2*ampereValue*ampereValue + 
                    bx*ampereValue;// +
                    //c;

          r_left(batArray, BAT_ARR_SIZE);                                           // shift battery measurement array
          batArray[BAT_ARR_SIZE-1] = battery.level(battery.voltage() + dropGap);    // add current measurement to last element, plus battery drop on load                                            
          
          batLevel = calculateAverage(batArray, BAT_ARR_SIZE);                      // calculate average
          *data581 = batLevel;
          *data781 = batLevel;

          

          if(ampereValue <= -2.00)  //   charging
          {
            switch (chargingIter)
            {
              case 1: digitalWrite(LED1, LOW); break;
              case 2: digitalWrite(LED2, LOW); break;
              case 3: digitalWrite(LED3, LOW); break;
              case 4: digitalWrite(LED4, LOW); break;
              case 5: digitalWrite(LED5, LOW); break;
              default: 
                chargingIter=0; 
                digitalWrite(LED1, HIGH);
                digitalWrite(LED2, HIGH);
                digitalWrite(LED3, HIGH);
                digitalWrite(LED4, HIGH);
                digitalWrite(LED5, HIGH);
                break;
            }
            chargingIter++;
          }
        }

        while (CAN_MSGAVAIL == CAN.checkReceive()) 
        {
            // read data,  len: data length, buf: data buf
            CAN.readMsgBuf(&len, buf);
            // check if this is a duplicate message (including a timeout, so that the same message is accepted again after a while)
            // if((len != lastLen) || (millis() > lastMsgTime + DUPLICATE_TIMEOUT) || (memcmp((const void *)lastBuf, (const void *)buf, sizeof(buf)) != 0))
            // {
              lastLen = len;
              memcpy(lastBuf, buf, sizeof(buf));
              lastMsgTime = millis();

              switch (CAN.getCanId())
              {
                case 0x020: //  0x40000020
                  *data59F = 0x01;
                  frame59F.sendCAN(CAN);
                  break;
                case 0x120:
                  *data59F = 0x00;
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
                  frame784.sendCAN(CAN); 
                  break;
                default:
                  break;
              }
            // }
        }
/* TODO: uncoment when interupts will works. For now works only on arduino UNO.
    } else if(millis() > lastBusActivity + KEEP_AWAKE_TIME) 
    { 
      // Put MCP2515 into sleep mode
      CAN.sleep();
      
      // Put the transceiver into standby (by pulling Rs high):
      if(RS_TO_MCP2515) 
        CAN.mcpDigitalWrite(RS_OUTPUT, HIGH);
      else 
        digitalWrite(RS_OUTPUT, HIGH);

      cli(); // Disable interrupts
      if(!flagRecv) // Make sure we havn't missed an interrupt between the check above and now. If an interrupt happens between now and sei()/sleep_cpu() then sleep_cpu() will immediately wake up again
      {
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_enable();
        sleep_bod_disable();
        sei();
        sleep_cpu();
        // Now the Arduino sleeps until the next message arrives...
        sleep_disable();
      }
      sei();

      CAN.wake(); // When the MCP2515 wakes up it will be in LISTENONLY mode, here we put it into the mode it was before sleeping

      // Wake up the transceiver:
      if(RS_TO_MCP2515) 
        CAN.mcpDigitalWrite(RS_OUTPUT, LOW);
      else 
        digitalWrite(RS_OUTPUT, LOW);
    }
*/
}

//shift left array
void r_left(int *a,int n) 
{
  memmove(a,a+1,sizeof(int)*(n-1));
}

//calculate average from array
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

//shift left array
void r_left_double(double *a,int n) 
{
  memmove(a,a+1,sizeof(double)*(n-1));
}

//calculate average from array
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
  else
  {
    delay(3000);
  }
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);
  digitalWrite(LED5, HIGH);
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/