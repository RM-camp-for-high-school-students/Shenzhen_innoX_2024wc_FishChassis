#pragma once
#ifndef FISHMESSAGE_H
#define FISHMESSAGE_H
#include <cstdint>
#include "om.h"

extern uint32_t fishPrintf(uint8_t *buf, const char *str, ...);

struct Msg_Ins_t {
    float quaternion[4];
    /*Y-P-R*/
    float Euler[3];
    uint64_t timestamp;
};



#endif