#include <SPI.h>
#include "mcp_can.h"
#include <avr/sleep.h>
#include "CanFrame.h"
#include <PinChangeInterrupt.h>
#include <Battery.h>


const int SPI_CS_PIN = 10;

//used for interupt
#define CAN_INT 2                                            // Set INT to pin 2

#define RS_TO_MCP2515 true                                   // Set this to false if Rs is connected to your Arduino
#define RS_OUTPUT MCP_RX0BF                                  // RX0BF is a pin of the MCP2515. You can also define an Arduino pin here

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin
                             
#define KEEP_AWAKE_TIME 2000                                // time the controller will stay awake after the last activity on the bus (in ms)
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
#define BAT_MAX 41000
Battery battery(BAT_MIN, BAT_MAX, A0);
unsigned char batLevel = 0;

#define BAT_ARR_SIZE 64   // must be power of 2
unsigned int batArray[BAT_ARR_SIZE];

unsigned long actTime = 0;
unsigned long prevTime = 0;

// used for check motor power
unsigned int actualSupportLevel = 0;
unsigned int crankRPM = 0;
unsigned int torque = 0;
unsigned int actRiderPower = 0;
unsigned int avrRiderPower = 0;
#define RID_ARR_SIZE 8   // must be power of 2
unsigned int riderPowerArr[RID_ARR_SIZE];
unsigned int motorPower = 0;

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

void setup()
{
    Serial.begin(9600);
    CAN.begin(CAN_500KBPS, MCP_8MHz);
	  battery.begin(5170, 9.5);  //9.41 -> 9.4098451042
    
    CAN.setMode(MODE_NORMAL);

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
        if (actTime - prevTime >= 250UL)                         // read battery level in every 250ms
        {
          prevTime = actTime;
          r_left(batArray, BAT_ARR_SIZE);                       // shift battery measurement array
          batArray[BAT_ARR_SIZE-1] = battery.level();           // add current measurement to last element 

          // if (motorPower!=0) // if motor is loaded, allow for voltage drop
          // {
          //   batArray[BAT_ARR_SIZE-1] = (battery.level()+motorPower/10 > batLevel ? batLevel:battery.level()+motorPower/10);
          // }
          // else
          // {
          //   batArray[BAT_ARR_SIZE-1] = battery.level();
          // }
          
          batLevel = calculateAverage(batArray, BAT_ARR_SIZE);  // calculate average
          *data581 = batLevel;
          *data781 = batLevel;
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
                case 0x642: 
                  actualSupportLevel = getPercentLevel(buf[0]);                   // support level is in CAN Data [0]
                  break;
                case 0x6C2:    // Make a lot of calculation to get motor power from CANBUS data
                  crankRPM = round(buf[5]*25.6) + floor((buf[4]+3)*0.1);          // calculate RPM from CAN data [4] and [5]
                  torque = buf[2] + (0xFF*buf[3]);                                // calculate torque grom CAN data [2] and [3]
                  actRiderPower = (torque * crankRPM) / 100;                      // calculate rider power
                  r_left(riderPowerArr, RID_ARR_SIZE);                   // shift rider power array
                  riderPowerArr[RID_ARR_SIZE-1] = actRiderPower;         // add rider power to last element of array
                  avrRiderPower = calculateAverage(riderPowerArr, RID_ARR_SIZE);
                  motorPower = 
                    (actualSupportLevel*avrRiderPower/100 <= 250 ? 
                    actualSupportLevel*avrRiderPower/100 : 250);                   //calculate actual motor power, can not be more than 250W
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
  long level = 0;
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

int getPercentLevel(char data)
{
  switch(data)
  {
    case 0x1B: return 50; break;
    case 0x1A: return 75; break;
    case 0x19: return 100; break;
    case 0x18: return 125; break;
    case 0x17: return 150; break;
    case 0x16: return 175; break;
    case 0x15: return 200; break;
    case 0x14: return 250; break;
    case 0x13: return 300; break;
    case 0x12: return 350; break;
    case 0x11: return 360; break;
    default: return 0; break;
  }
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/