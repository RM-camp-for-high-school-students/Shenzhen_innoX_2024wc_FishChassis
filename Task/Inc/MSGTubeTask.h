/*
 * @Description: 
 * @Author: qianwan
 * @Date: 2023-09-29 14:19:38
 * @LastEditTime: 2023-10-06 15:55:08
 * @LastEditors: qianwan
 */
#ifndef MSGTUBETASK_H
#define MSGTUBETASK_H

#include "main.h"

#ifdef __cplusplus


extern "C" {
    void usbx_cdc_acm_read_thread_entry(ULONG arg);
    void usbx_cdc_acm_write_thread_entry(ULONG arg);
}
#endif


#endif