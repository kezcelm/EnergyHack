#include <SPI.h>
#include <mcp2515.h>
#include "CanFrames.h"
// #include "CanFrames.cpp"

struct can_frame canMsg;
MCP2515 mcp2515(10);

void setFrameData(can_frame canFrame, canid_t canId, __u8 canDlc, __u8 *data){
  canFrame.can_id = canId;
  canFrame.can_dlc = canDlc;
  for(int i = 0; i < canDlc; ++i) {
    canFrame.data[i] = data[i];
  }
}

void setup() {
  Serial.begin(9600);
  SPI.begin();

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();

setFrameData(frame580, 0x580, 3, data580);
// frame580.can_id = 0x580;
// frame580.can_dlc = 3;
// frame580.data[0] = 0x00;
// frame580.data[1] = 0xC0;
// frame580.data[2] = 0x00;

setFrameData(frame581, 0x581, 7, data581);
// frame581.can_id = 0x581;
// frame581.can_dlc = 7;
// frame581.data[0] = 0x5A;  // Battery level 0x64 = 100%
// frame581.data[1] = 0x95;
// frame581.data[2] = 0x37;
// frame581.data[3] = 0xE3;
// frame581.data[4] = 0x3D;
// frame581.data[5] = 0x04;
// frame581.data[6] = 0x42;

setFrameData(frame582, 0x582, 4, data582);
// frame582.can_id = 0x582;
// frame582.can_dlc = 4;
// frame582.data[0] = 0x00;
// frame582.data[1] = 0x00;
// frame582.data[2] = 0x78;
// frame582.data[3] = 0x9E;

setFrameData(frame583, 0x583, 5, data583);
// frame583.can_id = 0x583;
// frame583.can_dlc = 5;
// frame583.data[0] = 0xF4;
// frame583.data[1] = 0x01;
// frame583.data[2] = 0x48;
// frame583.data[3] = 0xA3;
// frame583.data[4] = 0x01;

setFrameData(frame584, 0x584, 4, data584);
frame584.can_id = 0x584;
// frame584.can_dlc = 4;
// frame584.data[0] = 0x00;
// frame584.data[1] = 0x00;
// frame584.data[2] = 0x00;
// frame584.data[3] = 0x00;

setFrameData(frame590, 0x590, 2, data590);
// frame590.can_id = 0x590;
// frame590.can_dlc = 2;
// frame590.data[0] = 0x0B;
// frame590.data[1] = 0x14;

setFrameData(frame591, 0x591, 8, data591);
// frame591.can_id = 0x591;
// frame591.can_dlc = 8;
// frame591.data[0] = 0x14;
// frame591.data[1] = 0x0A;
// frame591.data[2] = 0x13;
// frame591.data[3] = 0xB0;
// frame591.data[4] = 0x0C;
// frame591.data[5] = 0x00;
// frame591.data[6] = 0x00;
// frame591.data[7] = 0x00;

setFrameData(frame592, 0x592, 6, data592);
// frame592.can_id = 0x592;
// frame592.can_dlc = 6;
// frame592.data[0] = 0x1D;
// frame592.data[1] = 0x10;
// frame592.data[2] = 0x1A;
// frame592.data[3] = 0x10;
// frame592.data[4] = 0x94;
// frame592.data[5] = 0x93;

setFrameData(frame593, 0x593, 8, data593);
// frame593.can_id = 0x593;
// frame593.can_dlc = 8;
// frame593.data[0] = 0x20;
// frame593.data[1] = 0x00;
// frame593.data[2] = 0x15;
// frame593.data[3] = 0x00;
// frame593.data[4] = 0x31;
// frame593.data[5] = 0x00;
// frame593.data[6] = 0x02;
// frame593.data[7] = 0x5E;

setFrameData(frame594, 0x594, 4, data594);
// frame594.can_id = 0x594;
// frame594.can_dlc = 4;
// frame594.data[0] = 0x28;
// frame594.data[1] = 0x27;
// frame594.data[2] = 0x08;
// frame594.data[3] = 0xF4;

setFrameData(frame595, 0x595, 6, data595);
// frame595.can_id = 0x595;
// frame595.can_dlc = 6;
// frame595.data[0] = 0x61;
// frame595.data[1] = 0x10;
// frame595.data[2] = 0xB0;
// frame595.data[3] = 0x0A;
// frame595.data[4] = 0xAC;
// frame595.data[5] = 0x79;

setFrameData(frame59F, 0x59F, 1, data59F);
// frame59F.can_id = 0x59F;
// frame59F.can_dlc = 1;
// frame59F.data[0] = 0x01;

setFrameData(frame780, 0x780, 3, data780);
frame780.can_id = 0x780;
// frame780.can_dlc = 3;
// frame780.data[0] = 0x00;
// frame780.data[1] = 0x0C;
// frame780.data[2] = 0x00;

setFrameData(frame781, 0x781, 7, data781);
// frame781.can_id = 0x781;
// frame781.can_dlc = 7;
// frame781.data[0] = 0x5A;  // Battery level 0x64 = 100%
// frame781.data[1] = 0x95;
// frame781.data[2] = 0x37;
// frame781.data[3] = 0xE3;
// frame781.data[4] = 0x3D;
// frame781.data[5] = 0x04;
// frame781.data[6] = 0x42;

setFrameData(frame782, 0x782, 4, data782);
// frame782.can_id = 0x782;
// frame782.can_dlc = 4;
// frame782.data[0] = 0x21;
// frame782.data[1] = 0x00;
// frame782.data[2] = 0x7A;
// frame782.data[3] = 0x9E;

setFrameData(frame783, 0x783, 5, data783);
// frame783.can_id = 0x783;
// frame783.can_dlc = 5;
// frame783.data[0] = 0xA0;
// frame783.data[1] = 0x0F;
// frame783.data[2] = 0x48;
// frame783.data[3] = 0xA3;
// frame783.data[4] = 0x01;

setFrameData(frame783, 0x784, 3, data784);
// frame784.can_id = 0x784;
// frame784.can_dlc = 3;
// frame784.data[0] = 0x00;
// frame784.data[1] = 0x0C;
// frame784.data[2] = 0x00;

}

void loop() {

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