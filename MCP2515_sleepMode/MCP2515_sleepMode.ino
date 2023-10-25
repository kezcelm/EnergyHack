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

const int SPI_CS_PIN = 10;
#define CAN_INT 2                                            // Set INT to pin 2

#define RS_TO_MCP2515 true                                   // Set this to false if Rs is connected to your Arduino
#define RS_OUTPUT MCP_RX0BF                                  // RX0BF is a pin of the MCP2515. You can also define an Arduino pin here

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin
                             
#define KEEP_AWAKE_TIME 200                                  // time the controller will stay awake after the last activity on the bus (in ms)
unsigned long lastBusActivity = millis();

unsigned char flagRecv = 0;
unsigned char len = 0;
unsigned char buf[8];

void setup()
{
    Serial.begin(9600);

    while (CAN_OK != CAN.begin(CAN_500KBPS, MCP_8MHz))       // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(100);
    }
    Serial.println("CAN BUS Shield init ok!");

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
}

void MCP2515_ISR()
{
    flagRecv = 1;
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

            switch (CAN.getCanId()){
              case 0x40000020:
                Serial.println("Recive 0x40000020");
                break;
              case 0x40000120:
                Serial.println("Recive 0x40000120");
                break;
              case 0x4000042C:
                Serial.println("Recive 0x4000042C");
                break;
              case 0x4000022C || 0x4000026C:
                Serial.println("Recive 0x4000022C or 0x4000026C");
                break;
              case 0x4000052C:
                Serial.println("Recive 0x4000052C");
                break;
              case 0x641:
                Serial.println("Recive 0x641");
                break;
              default:
                break;
            }
        }
    } else if(millis() > lastBusActivity + KEEP_AWAKE_TIME) 
    {
      // Put MCP2515 into sleep mode
      Serial.println(F("CAN sleep"));
      CAN.sleep();
      
      // Put the transceiver into standby (by pulling Rs high):
      if(RS_TO_MCP2515) 
        CAN.mcpDigitalWrite(RS_OUTPUT, HIGH);
      else 
        digitalWrite(RS_OUTPUT, HIGH);
      
      // Put the MCU to sleep
      Serial.println(F("MCU sleep"));

      // Clear serial buffers before sleeping
      Serial.flush();

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

      Serial.println(F("Woke up"));
    }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/