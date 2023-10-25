// demo: CAN Sleep Example - receive
// by Kai, based on the receive_interrupt example by loovee and the additions from Zak Kemble (https://github.com/coryjfowler/MCP_CAN_lib/pull/10/files)
// 
// By setting the MCU, the CAN controller (MCP2515) and the transceiver (MCP2551) into sleep mode, you can reduce 
// the power consumption of the whole setup from around 50mA down to 240uA (Arduino directly connected to 5V, regulator and 
// power LED removed). The node will wake up when a new message arrives, process the message and go back to sleep
// afterwards.
//
// Known issues:
// - Because it takes some time for the controller to wake up, the first message is usually lost. Look at the 
//   send_sleep example on how to avoid this by sending a special wakeup message before the normal message.
// - If you only have 2 devices on the CAN bus (the device running this sketch and some other device sending 
//   messages), you may find that duplicate messages are received when waking up. This is because when 
//   the MCP2515 wakes up it enters LISTENONLY mode where it does not send ACKs to messages, so the transmitter 
//   will retransmit the same message a few times. See below for a simple solution to filter duplicate messages out. 

#include <SPI.h>
#include "mcp_can.h"
#include <avr/sleep.h>
#include "CanFrames.h"

const int SPI_CS_PIN = 10;
#define CAN_INT 2                                            // Set INT to pin 2

#define RS_TO_MCP2515 true                                   // Set this to false if Rs is connected to your Arduino
#define RS_OUTPUT MCP_RX0BF                                  // RX0BF is a pin of the MCP2515. You can also define an Arduino pin here

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin
                             
#define KEEP_AWAKE_TIME 2000                                // time the controller will stay awake after the last activity on the bus (in ms)
unsigned long lastBusActivity = millis();

unsigned char flagRecv = 0;
unsigned char len = 0;
unsigned char buf[8];


int batCheckPin = A0;                                   // To check battery level
int batCheckValue = 0;
unsigned char batLevel = 0;
#define BAT_MIN 802
#define BAT_MAX 1023 // full charged, but 1013 when battery is not conneted to bike.

void setup()
{
    Serial.begin(9600);
    CAN.begin(CAN_500KBPS, MCP_8MHz);

    // attach interrupt
    pinMode(CAN_INT, INPUT);
    attachInterrupt(digitalPinToInterrupt(CAN_INT), MCP2515_ISR, FALLING);

    CAN.setSleepWakeup(1);                                   // this tells the MCP2515 to wake up on incoming messages

    // Pull the Rs pin of the MCP2551 transceiver low to enable it:
    if(RS_TO_MCP2515) 
    {
      CAN.mcpPinMode(MCP_RX0BF, MCP_PIN_OUT);
      CAN.mcpDigitalWrite(RS_OUTPUT, LOW);
    } else {
      pinMode(RS_OUTPUT, OUTPUT);
      digitalWrite(RS_OUTPUT, LOW);
    }

  // Initiate CAN data
  setFrameData(&frame580, 0x580, 0, 3, data580);
  setFrameData(&frame581, 0x581, 0, 7, data581);
  setFrameData(&frame582, 0x582, 0, 4, data582);
  setFrameData(&frame583, 0x583, 0, 5, data583);
  setFrameData(&frame584, 0x584, 0, 4, data584);
  setFrameData(&frame590, 0x590, 0, 2, data590);
  setFrameData(&frame591, 0x591, 0, 8, data591);
  setFrameData(&frame592, 0x592, 0, 6, data592);
  setFrameData(&frame593, 0x593, 0, 8, data593);
  setFrameData(&frame594, 0x594, 0, 4, data594);
  setFrameData(&frame595, 0x595, 0, 6, data595);
  setFrameData(&frame59F, 0x59F, 0, 1, data59F);
  setFrameData(&frame780, 0x780, 0, 3, data780);
  setFrameData(&frame781, 0x781, 0, 7, data781);
  setFrameData(&frame782, 0x782, 0, 4, data782);
  setFrameData(&frame783, 0x783, 0, 5, data783);
  setFrameData(&frame783, 0x784, 0, 3, data784);

}

void MCP2515_ISR()
{
      = 1;
}

void loop()
{
    if(flagRecv) 
    {                                   // check if get data

        flagRecv = 0;                   // clear flag
        lastBusActivity = millis();

        while (CAN_MSGAVAIL == CAN.checkReceive()) 
        {
            // read data,  len: data length, buf: data buf
            CAN.readMsgBuf(&len, buf);
            getBatteryLevel();
            switch (CAN.getCanId()){
              case 0x020: //  0x40000020
                data59F[0] = 0x01;
                sendCAN(&frame59F);
                break;
              case 0x120:
                data59F[0] = 0x00;
                sendCAN(&frame59F);
                break;
              case 0x42C:
                sendCAN(&frame590);
                sendCAN(&frame591);
                sendCAN(&frame592);
                break;
              case 0x22C || 0x26C:
                sendCAN(&frame580);
                sendCAN(&frame581);
                sendCAN(&frame583);
                break;
              case 0x52C:
                sendCAN(&frame593);
                sendCAN(&frame594);
                sendCAN(&frame595);
                break;
              case 0x641:
                sendCAN(&frame780);
                sendCAN(&frame781);
                sendCAN(&frame784);
                break;
              default:
                break;
            }
        }
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
}

void sendCAN(can_frame *frame){
  CAN.sendMsgBuf(frame->can_id, frame->can_ext, frame->can_dlc, frame->data);
}

void setFrameData(can_frame *canFrame, unsigned long canId, byte canExt, byte canDlc, const byte *data){
  canFrame->can_id = canId;
  canFrame->can_ext = canExt;
  canFrame->can_dlc = canDlc;
  canFrame->data = data;
}

void getBatteryLevel(){
  batCheckValue = analogRead(batCheckPin);
  if (batCheckValue < BAT_MIN){
    batLevel = 0;
  }
  else{
     batLevel = ceil((((batCheckValue - BAT_MIN) * 100)/(BAT_MAX - BAT_MIN)));
     data581[0] = batLevel;
     data781[0] = batLevel;
  }
}


/*********************************************************************************************************
  END FILE
*********************************************************************************************************/