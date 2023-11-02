
#include "CanFrame.h"
#include <Arduino.h>
// #include "mcp_can.h"


CanFrame::CanFrame(unsigned long canId, byte canExt, byte canDlc, const byte *data) {
  this->can_id = canId;
  this->can_ext = canExt;
  this->can_dlc = canDlc;
  this->data = data;
}

void CanFrame::sendCAN(MCP_CAN CAN){
  CAN.sendMsgBuf(this->can_id,  this->can_ext,  this->can_dlc,  this->data);
}


//Hardcoded frames
const byte *data580[] = {0x00, 
                  0xC0, 
                  0x00};
const byte *data581[] = {0x00, // Battery level - data[0] = 0x64 - 100%
                  0x95, 
                  0x37, 
                  0xE3, 
                  0x3D, 
                  0x04, 
                  0x42}; 
const byte *data582[] = {0x00, 
                  0x00, 
                  0x78, 
                  0x9E};
const byte *data583[] = {0xF4, 
                  0x01, 
                  0x48, 
                  0xA3, 
                  0x01};
const byte *data584[] = {0x00, 
                  0x00, 
                  0x00, 
                  0x00};
const byte *data590[] = {0x0B, 
                  0x14};
const byte *data591[] = {0x14, 
                  0x0A, 
                  0x13, 
                  0xB0, 
                  0x0C, 
                  0x00, 
                  0x00, 
                  0x00};
const byte *data592[] = {0x1D, 
                  0x10, 
                  0x1A, 
                  0x10, 
                  0x94, 
                  0x93};
const byte *data593[] = {0x20, 
                  0x00, 
                  0x15, 
                  0x00, 
                  0x31, 
                  0x00, 
                  0x02, 
                  0x5E};
const byte *data594[] = {0x28, 
                  0x27, 
                  0x08, 
                  0xF4};
const byte *data595[] = {0x61, 
                  0x10, 
                  0xB0, 
                  0x0A, 
                  0xAC, 
                  0x79};
const byte *data59A[] = {0x9A, 
                  0x00, 
                  0x13, 
                  0x00, 
                  0x02, 
                  0x00};
const byte *data59B[] = {0x02, 
                  0x03};
const byte *data59F[] = {0x01}; // data[0] = 0x00 or 0x01
const byte *data780[] = {0x00, 
                  0xC0, 
                  0x00};
const byte *data781[] = {0x00,  // Battery level - data[0] = 0x64 - 100%
                  0x95, 
                  0x37, 
                  0xE3, 
                  0x3D, 
                  0x04, 
                  0x42};
const byte *data782[] = {0x21, 
                  0x00, 
                  0x7A, 
                  0x9E};
const byte *data783[] = {0xA0, 
                  0x0F, 
                  0x48, 
                  0xA3, 
                  0x01};
const byte *data784[] = {0x00, 
                  0x0C, 
                  0x00};