#pragma once
#ifndef FISHMESSAGE_H
#define FISHMESSAGE_H
#include <cstdint>
#include "mavlink.h"
#include "om.h"

extern uint32_t fishPrintf(uint8_t *buf, const char *str, ...);

#pragma pack(push) //保存对齐状态
#pragma pack(1)

struct Msg_INS_t {
    float quaternion[4];
    /*Y-P-R*/
    float gyro[3];
    uint64_t timestamp;
};

struct Msg_WheelControl_t {
    //meters per second
    bool enable;
    float mps[4];
    uint64_t timestamp;
};

struct Msg_WheelFDB_t {
    //meters per second
    float mps[4];
    uint64_t timestamp;
};

struct Msg_Servo_t {
    //Duty Cycle
    bool enable;
    uint16_t servo[7];
    uint64_t timestamp;
};

struct usb_rx_data_processed_t{
    mavlink_chs_ctrl_info_t chs_ctrl_info;
    mavlink_chs_motor_info_t chs_motor_info;
    mavlink_chs_servos_info_t chs_servos_info;
    mavlink_chs_manage_info_t chs_manage_info;
    bool update_list_in_order[4]; // 0:chs_ctrl_info 1:chs_motor_info 2:chs_servos_info 3:chs_manage_info
};

struct spi_rx_data_processed_t{
    mavlink_chs_motor_info_t chs_motor_info;
    mavlink_chs_servos_info_t chs_servos_info;
    mavlink_chs_manage_info_t chs_manage_info;
    bool update;
};
#pragma pack(pop) //恢复对齐状态
#endif