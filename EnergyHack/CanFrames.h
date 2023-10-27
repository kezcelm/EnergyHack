#ifndef CAN_FRAMES_H
#define CAN_FRAMES_H

#include <Arduino.h>

struct can_frame {
    unsigned long can_id;
    byte can_ext;
    byte can_dlc;
    const byte *data; 
    bool wait_sent=true;
};

struct can_frame frame580;
unsigned char data580[] = {0x00, 
                  0xC0, 
                  0x00};

struct can_frame frame581;
unsigned char data581[] = {0x00, // Battery level - data[0] = 0x64 - 100%
                  0x95, 
                  0x37, 
                  0xE3, 
                  0x3D, 
                  0x04, 
                  0x42}; 

struct can_frame frame582;
unsigned char data582[] = {0x00, 
                  0x00, 
                  0x78, 
                  0x9E};

struct can_frame frame583;
unsigned char data583[] = {0xF4, 
                  0x01, 
                  0x48, 
                  0xA3, 
                  0x01};

struct can_frame frame584;
unsigned char data584[] = {0x00, 
                  0x00, 
                  0x00, 
                  0x00};

struct can_frame frame590;
unsigned char data590[] = {0x0B, 
                  0x14};

struct can_frame frame591;
unsigned char data591[] = {0x14, 
                  0x0A, 
                  0x13, 
                  0xB0, 
                  0x0C, 
                  0x00, 
                  0x00, 
                  0x00};

struct can_frame frame592;
unsigned char data592[] = {0x1D, 
                  0x10, 
                  0x1A, 
                  0x10, 
                  0x94, 
                  0x93};

struct can_frame frame593;
unsigned char data593[] = {0x20, 
                  0x00, 
                  0x15, 
                  0x00, 
                  0x31, 
                  0x00, 
                  0x02, 
                  0x5E};

struct can_frame frame594;
unsigned char data594[] = {0x28, 
                  0x27, 
                  0x08, 
                  0xF4};

struct can_frame frame595;
unsigned char data595[] = {0x61, 
                  0x10, 
                  0xB0, 
                  0x0A, 
                  0xAC, 
                  0x79};

struct can_frame frame59A;
unsigned char data59A[] = {0x9A, 
                  0x00, 
                  0x13, 
                  0x00, 
                  0x02, 
                  0x00};

struct can_frame frame59B;
unsigned char data59B[] = {0x02, 
                  0x03};

struct can_frame frame59F;
unsigned char data59F[] = {0x01}; // data[0] = 0x00 or 0x01

struct can_frame frame780;
unsigned char data780[] = {0x00, 
                  0xC0, 
                  0x00};

struct can_frame frame781;
unsigned char data781[] = {0x00,  // Battery level - data[0] = 0x64 - 100%
                  0x95, 
                  0x37, 
                  0xE3, 
                  0x3D, 
                  0x04, 
                  0x42};

struct can_frame frame782;
unsigned char data782[] = {0x21, 
                  0x00, 
                  0x7A, 
                  0x9E};

struct can_frame frame783;
unsigned char data783[] = {0xA0, 
                  0x0F, 
                  0x48, 
                  0xA3, 
                  0x01};

struct can_frame frame784;
unsigned char data784[] = {0x00, 
                  0x0C, 
                  0x00};

#endif