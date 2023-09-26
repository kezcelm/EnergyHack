#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsg;
struct can_frame frame580; 
struct can_frame frame581; //581 7 5A BE 37 E3 3D 4 42 
struct can_frame frame582; 
struct can_frame frame583; 
struct can_frame frame584;
struct can_frame frame590;
struct can_frame frame591;
struct can_frame frame592;
struct can_frame frame593;
struct can_frame frame594;
struct can_frame frame595;
struct can_frame frame59F;
struct can_frame frame780; //
struct can_frame frame781; //781 7 5A BE 37 E3 3D 4 42 
struct can_frame frame782;
struct can_frame frame783;
struct can_frame frame784;
MCP2515 mcp2515(10);

uint32_t
    timeNow,
    timeMsg1;


void setup() {
  Serial.begin(9600);
  SPI.begin();
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();

  timeNow = micros();
  timeMsg1 = timeNow;

  frame580.can_id = 0x580;
  frame580.can_dlc = 3;
  frame580.data[0] = 0x00;
  frame580.data[1] = 0xC0;
  frame580.data[2] = 0x00;

  frame581.can_id = 0x581;
  frame581.can_dlc = 7;
  frame581.data[0] = 0x5A;   //0x62 - 98%
  frame581.data[1] = 0x95;
  frame581.data[2] = 0x37;
  frame581.data[3] = 0xE3;
  frame581.data[4] = 0x3D;
  frame581.data[5] = 0x04;
  frame581.data[6] = 0x42;

  frame582.can_id = 0x582; //00 00 78 9E
  frame582.can_dlc = 4;
  frame582.data[0] = 0x00;
  frame582.data[1] = 0x00;
  frame582.data[2] = 0x78;
  frame582.data[3] = 0x9E;

  frame583.can_id = 0x583; // 583 5 F4 1 48 A3 1 
  frame583.can_dlc = 5;
  frame583.data[0] = 0xF4;
  frame583.data[1] = 0x01;
  frame583.data[2] = 0x48;
  frame583.data[3] = 0xA3;
  frame583.data[4] = 0x01;

  frame584.can_id = 0x584;
  frame584.can_dlc = 0;
  frame584.data[0] = 0x00;
  frame584.data[1] = 0x00;
  frame584.data[2] = 0x00;
  frame584.data[3] = 0x00;
  
  frame590.can_id = 0x590;
  frame590.can_dlc = 4;
  frame590.data[0] = 0x0B;
  frame590.data[1] = 0x14;

  frame591.can_id = 0x591;
  frame591.can_dlc = 8;
  frame591.data[0] = 0x14;
  frame591.data[1] = 0x0A;
  frame591.data[2] = 0x13;
  frame591.data[3] = 0xB0;
  frame591.data[4] = 0x0C;
  frame591.data[5] = 0x00;
  frame591.data[6] = 0x00;
  frame591.data[7] = 0x00;

  frame592.can_id = 0x592;
  frame592.can_dlc = 6;
  frame592.data[0] = 0x1D;
  frame592.data[1] = 0x10;
  frame592.data[2] = 0x1A;
  frame592.data[3] = 0x10;
  frame592.data[4] = 0x94;
  frame592.data[5] = 0x93;

  frame593.can_id = 0x593;
  frame593.can_dlc = 8;
  frame593.data[0] = 0x20;
  frame593.data[1] = 0x00;
  frame593.data[2] = 0x15;
  frame593.data[3] = 0x00;
  frame593.data[4] = 0x31;
  frame593.data[5] = 0x00;
  frame593.data[6] = 0x02;
  frame593.data[7] = 0x5E;

  frame594.can_id = 0x594;
  frame594.can_dlc = 4;
  frame594.data[0] = 0x28;
  frame594.data[1] = 0x27;
  frame594.data[2] = 0x08;
  frame594.data[3] = 0xF4;

  frame595.can_id = 0x595; 
  frame595.can_dlc = 6;
  frame595.data[0] = 0x61;
  frame595.data[1] = 0x10;
  frame595.data[2] = 0xB0;
  frame595.data[3] = 0x0A;
  frame595.data[4] = 0xAC;
  frame595.data[5] = 0x79;

  frame59F.can_id = 0x59F;
  frame59F.can_dlc = 1;
  frame59F.data[0] = 0x01;
  
  frame780.can_id = 0x780;
  frame780.can_dlc = 3;
  frame780.data[0] = 0x00;
  frame780.data[1] = 0x0C;
  frame780.data[2] = 0x00;
  
  frame781.can_id = 0x781;
  frame781.can_dlc = 7;
  frame781.data[0] = 0x5A;   //0x62 - 98%
  frame781.data[1] = 0x95;
  frame781.data[2] = 0x37;
  frame781.data[3] = 0xE3;
  frame781.data[4] = 0x3D;
  frame781.data[5] = 0x04;
  frame781.data[6] = 0x42;
  
  frame782.can_id = 0x782;
  frame782.can_dlc = 4;
  frame782.data[0] = 0x21;
  frame782.data[1] = 0x00;
  frame782.data[2] = 0x7A;
  frame782.data[3] = 0x9E;
  
  frame783.can_id = 0x783; 
  frame783.can_dlc = 5;
  frame783.data[0] = 0xA0;
  frame783.data[1] = 0x0F;
  frame783.data[2] = 0x48;
  frame783.data[3] = 0xA3;
  frame783.data[4] = 0x01;
  
  
  frame783.can_id = 0x784;
  frame783.can_dlc = 3;
  frame783.data[0] = 0x00;
  frame783.data[1] = 0x0C;
  frame783.data[2] = 0x00;

}

void loop() {

  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {

      if (canMsg.can_id == 0x40000020)
      {   
        frame59F.data[0] = 0x01;     
        mcp2515.sendMessage(&frame59F);
      } 
      if (canMsg.can_id == 0x4000120)
      {        
        frame59F.data[0] = 0x00; 
        mcp2515.sendMessage(&frame59F);
      } 

      if (canMsg.can_id == 0x4000042C)
      {        
        mcp2515.sendMessage(&frame590);
        mcp2515.sendMessage(&frame591);
        mcp2515.sendMessage(&frame592);
      } 
      if (canMsg.can_id == 0x4000022C || canMsg.can_id == 0x4000026C)
      {
        mcp2515.sendMessage(&frame580);
        mcp2515.sendMessage(&frame581);
        mcp2515.sendMessage(&frame583);
      } 
      if (canMsg.can_id == 0x4000052C)
      {
        mcp2515.sendMessage(&frame593);
        mcp2515.sendMessage(&frame594);
        mcp2515.sendMessage(&frame595);
      } 
      if (canMsg.can_id == 0x641)
      {        
        mcp2515.sendMessage(&frame780);
        mcp2515.sendMessage(&frame781);
        mcp2515.sendMessage(&frame784);
      } 
  }
}