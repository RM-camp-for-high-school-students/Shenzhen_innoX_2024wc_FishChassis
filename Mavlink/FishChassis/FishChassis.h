/** @file
 *  @brief MAVLink comm protocol generated from FishChassis.xml
 *  @see http://mavlink.org
 */
#pragma once
#ifndef MAVLINK_FISHCHASSIS_H
#define MAVLINK_FISHCHASSIS_H

#ifndef MAVLINK_H
    #error Wrong include order: MAVLINK_FISHCHASSIS.H MUST NOT BE DIRECTLY USED. Include mavlink.h from the same directory instead or set ALL AND EVERY defines from MAVLINK.H manually accordingly, including the #define MAVLINK_H call.
#endif

#define MAVLINK_FISHCHASSIS_XML_HASH -2779601141069529883

#ifdef __cplusplus
extern "C" {
#endif

// MESSAGE LENGTHS AND CRCS

#ifndef MAVLINK_MESSAGE_LENGTHS
#define MAVLINK_MESSAGE_LENGTHS {}
#endif

#ifndef MAVLINK_MESSAGE_CRCS
#define MAVLINK_MESSAGE_CRCS {{0, 136, 12, 12, 0, 0, 0}, {1, 8, 8, 8, 0, 0, 0}, {2, 68, 28, 28, 0, 0, 0}, {3, 79, 14, 14, 0, 0, 0}, {4, 255, 3, 3, 0, 0, 0}}
#endif

#include "../protocol.h"

#define MAVLINK_ENABLED_FISHCHASSIS

// ENUM DEFINITIONS


/** @brief Chassis system and ID. */
#ifndef HAVE_ENUM_CHS_SYSTEM_ID
#define HAVE_ENUM_CHS_SYSTEM_ID
typedef enum CHS_SYSTEM_ID
{
   CHS_ID_ORANGE=1, /* OrangePi. | */
   CHS_ID_ESP32=2, /* ESP32 controller. | */
   CHS_ID_CHASSIS=3, /* Chassis. | */
   CHS_SYSTEM_ID_ENUM_END=4, /*  | */
} CHS_SYSTEM_ID;
#endif

// MAVLINK VERSION

#ifndef MAVLINK_VERSION
#define MAVLINK_VERSION 3
#endif

#if (MAVLINK_VERSION == 0)
#undef MAVLINK_VERSION
#define MAVLINK_VERSION 3
#endif

// MESSAGE DEFINITIONS
#include "./mavlink_msg_chs_ctrl_info.h"
#include "./mavlink_msg_chs_motor_info.h"
#include "./mavlink_msg_chs_odom_info.h"
#include "./mavlink_msg_chs_servos_info.h"
#include "./mavlink_msg_chs_manage_info.h"

// base include



#if MAVLINK_FISHCHASSIS_XML_HASH == MAVLINK_PRIMARY_XML_HASH
# define MAVLINK_MESSAGE_INFO {MAVLINK_MESSAGE_INFO_CHS_CTRL_INFO, MAVLINK_MESSAGE_INFO_CHS_MOTOR_INFO, MAVLINK_MESSAGE_INFO_CHS_ODOM_INFO, MAVLINK_MESSAGE_INFO_CHS_SERVOS_INFO, MAVLINK_MESSAGE_INFO_CHS_MANAGE_INFO}
# define MAVLINK_MESSAGE_NAMES {{ "CHS_CTRL_INFO", 0 }, { "CHS_MANAGE_INFO", 4 }, { "CHS_MOTOR_INFO", 1 }, { "CHS_ODOM_INFO", 2 }, { "CHS_SERVOS_INFO", 3 }}
# if MAVLINK_COMMAND_24BIT
#  include "../mavlink_get_info.h"
# endif
#endif

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // MAVLINK_FISHCHASSIS_H
