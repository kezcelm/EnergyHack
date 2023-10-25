#ifndef CAN_FRAMES_H
#define CAN_FRAMES_H

#include <Arduino.h>

struct can_frame frame580;
__u8 data580[] = {0x00, 
                  0xC0, 
                  0x00};

struct can_frame frame581;
__u8 data581[] = {0x00, // Battery level - data[0] = 0x64 - 100%
                  0x95, 
                  0x37, 
                  0xE3, 
                  0x3D, 
                  0x04, 
                  0x42}; 

struct can_frame frame582;
__u8 data582[] = {0x00, 
                  0x00, 
                  0x78, 
                  0x9E};

struct can_frame frame583;
__u8 data583[] = {0xF4, 
                  0x01, 
                  0x48, 
                  0xA3, 
                  0x01};

struct can_frame frame584;
__u8 data584[] = {0x00, 
                  0x00, 
                  0x00, 
                  0x00};

struct can_frame frame590;
__u8 data590[] = {0x0B, 
                  0x14};

struct can_frame frame591;
__u8 data591[] = {0x14, 
                  0x0A, 
                  0x13, 
                  0xB0, 
                  0x0C, 
                  0x00, 
                  0x00, 
                  0x00};

struct can_frame frame592;
__u8 data592[] = {0x1D, 
                  0x10, 
                  0x1A, 
                  0x10, 
                  0x94, 
                  0x93};

struct can_frame frame593;
__u8 data593[] = {0x20, 
                  0x00, 
                  0x15, 
                  0x00, 
                  0x31, 
                  0x00, 
                  0x02, 
                  0x5E};

struct can_frame frame594;
__u8 data594[] = {0x28, 
                  0x27, 
                  0x08, 
                  0xF4};

struct can_frame frame595;
__u8 data595[] = {0x61, 
                  0x10, 
                  0xB0, 
                  0x0A, 
                  0xAC, 
                  0x79};

struct can_frame frame59F;
__u8 data59F[] = {0x01}; // data[0] = 0x00 or 0x01

struct can_frame frame780;
__u8 data780[] = {0x00, 
                  0xC0, 
                  0x00};

struct can_frame frame781;
__u8 data781[] = {0x00,  // Battery level - data[0] = 0x64 - 100%
                  0x95, 
                  0x37, 
                  0xE3, 
                  0x3D, 
                  0x04, 
                  0x42};

struct can_frame frame782;
__u8 data782[] = {0x21, 
                  0x00, 
                  0x7A, 
                  0x9E};

struct can_frame frame783;
__u8 data783[] = {0xA0, 
                  0x0F, 
                  0x48, 
                  0xA3, 
                  0x01};

struct can_frame frame784;
__u8 data784[] = {0x00, 
                  0x0C, 
                  0x00};

#endif