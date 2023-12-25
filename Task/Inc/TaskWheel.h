#pragma once
#ifndef TASKWHEEL_H
#define TASKWHEEL_H
#include "cstdint"
namespace Wheel{
#define RPM_CONST 9167.3247220931713402877047702568f
#define RPM_CONST_REV 0.00010908307824964559855773067303054f

    struct __attribute__((packed, aligned(1))) M2006_t{
        uint16_t mechanical_degree;
        int16_t rpm;
        int16_t torque;
    };

}

#endif