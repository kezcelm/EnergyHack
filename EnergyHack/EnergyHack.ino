#include <SPI.h>
#include <mcp2515.h>
#include "CanFrames.h"
#include "mcp_can.h"
#include <avr/sleep.h>

#define BAT_MIN 802
#define BAT_MAX 1023 // full charged, but 1013 when battery is not conneted to bike.

struct can_frame canMsg;
MCP2515 mcp2515(10);
int batCheckPin = A0;
int batCheckValue = 0;
int batLevel = 0;

void setFrameData(can_frame *canFrame, canid_t canId, __u8 canDlc, __u8 *data){
  canFrame->can_id = canId;
  canFrame->can_dlc = canDlc;
  for(int i = 0; i < canDlc; ++i) {
    canFrame->data[i] = data[i];
  }
}

void getBatteryLevel(){
  batCheckValue = analogRead(batCheckPin);
  if (batCheckValue < BAT_MIN){
    batLevel = 0;
  }
  else{
     batLevel = ceil((((batCheckValue - BAT_MIN) * 100)/(BAT_MAX - BAT_MIN)));
     frame581.data[0] = batLevel;
     frame781.data[0] = batLevel;
  }
}

void setup() {
  Serial.begin(9600);
  SPI.begin();

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
  //mcp2515.setSleepMode();
  
  setFrameData(&frame580, 0x580, 3, data580);
  setFrameData(&frame581, 0x581, 7, data581);
  setFrameData(&frame582, 0x582, 4, data582);
  setFrameData(&frame583, 0x583, 5, data583);
  setFrameData(&frame584, 0x584, 4, data584);
  setFrameData(&frame590, 0x590, 2, data590);
  setFrameData(&frame591, 0x591, 8, data591);
  setFrameData(&frame592, 0x592, 6, data592);
  setFrameData(&frame593, 0x593, 8, data593);
  setFrameData(&frame594, 0x594, 4, data594);
  setFrameData(&frame595, 0x595, 6, data595);
  setFrameData(&frame59F, 0x59F, 1, data59F);
  setFrameData(&frame780, 0x780, 3, data780);
  setFrameData(&frame781, 0x781, 7, data781);
  setFrameData(&frame782, 0x782, 4, data782);
  setFrameData(&frame783, 0x783, 5, data783);
  setFrameData(&frame783, 0x784, 3, data784);

}

void loop() {
  getBatteryLevel();
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    switch (canMsg.can_id){
      case 0x40000020:
        frame59F.data[0] = 0x01;
        mcp2515.sendMessage(&frame59F);
        break;

      case 0x40000120:
        frame59F.data[0] = 0x00;
        mcp2515.sendMessage(&frame59F);
        break;

      case 0x4000042C:
        mcp2515.sendMessage(&frame590);
        mcp2515.sendMessage(&frame591);
        mcp2515.sendMessage(&frame592);
        break;

      case 0x4000022C || 0x4000026C:
        mcp2515.sendMessage(&frame580);
        mcp2515.sendMessage(&frame581);
        mcp2515.sendMessage(&frame583);
        break;

      case 0x4000052C:
        mcp2515.sendMessage(&frame593);
        mcp2515.sendMessage(&frame594);
        mcp2515.sendMessage(&frame595);
        break;

      case 0x641:
        mcp2515.sendMessage(&frame780);
        mcp2515.sendMessage(&frame781);
        mcp2515.sendMessage(&frame784);
        break;

      default:
        break;
    }
  }
}