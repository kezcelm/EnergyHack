#ifndef CAN_FRAME_H
#define CAN_FRAME_H

#include <Arduino.h>
#include "mcp_can.h"

struct can_frame {
    unsigned long can_id;
    byte can_ext;
    byte can_dlc;
    const byte *data; 
    bool wait_sent=true;
};

class CanFrame {
  
	public:
    CanFrame(unsigned long can_id, byte can_ext, byte can_dlc, const byte *data);
    void sendCAN(MCP_CAN CAN);

  private:
    unsigned long can_id;
    byte can_ext;
    byte can_dlc;
    const byte *data; 
    bool wait_sent=true;
};

extern const byte *data580[];
extern const byte *data581[];
extern const byte *data582[];
extern const byte *data583[];
extern const byte *data584[];
extern const byte *data590[];
extern const byte *data591[];
extern const byte *data592[];
extern const byte *data593[];
extern const byte *data594[];
extern const byte *data595[];
extern const byte *data59A[];
extern const byte *data59B[];
extern const byte *data59F[];
extern const byte *data780[];
extern const byte *data781[];
extern const byte *data782[];
extern const byte *data783[];
extern const byte *data784[];

#endif