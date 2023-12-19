/*
 * @Description:
 * @Author: qianwan
 * @Date: 2023-09-29 14:19:38
 * @LastEditTime: 2023-12-18 23:30:25
 * @LastEditors: qianwan
 */

#include "app_usbx_device.h"
#include "ux_device_cdc_acm.h"
#include "ux_device_class_cdc_acm.h"

#include "MSGTubeTask.h"
#include "main.h"

#define USBMAXLEN UX_SLAVE_REQUEST_DATA_MAX_LENGTH


/*USB CDC-ACM pointer*/
extern UX_SLAVE_CLASS_CDC_ACM *cdc_acm;

/*Error*/
extern void Error_Handler(void);

/* Data received are stored in this buffer */
SRAM_SET_CCM uint8_t usbrxbuf[USBMAXLEN];

/* Data to send over USB CDC are stored in this buffer   */
SRAM_SET_CCM uint8_t usbtxbuf[USBMAXLEN] = {0x01, 0x02, 0x03};


uint16_t heartbyte = 0;


/**
  * @brief  Function implementing usbx_cdc_acm_thread_entry.
  * @param arg: Not used
  * @retval None
  */
VOID usbx_cdc_acm_read_thread_entry(ULONG arg) {
    ULONG actual_length;
    UX_SLAVE_DEVICE *device;

    UX_PARAMETER_NOT_USED(arg);
    tx_thread_sleep(100);

    /* Get device */
    device = &_ux_system_slave->ux_system_slave_device;

    while (true) {
        /* Check if device is configured */
        if ((device->ux_slave_device_state == UX_DEVICE_CONFIGURED) && (cdc_acm != UX_NULL)) {

#ifndef UX_DEVICE_CLASS_CDC_ACM_TRANSMISSION_DISABLE

            /* Set transmission_status to UX_FALSE for the first time */
            cdc_acm->ux_slave_class_cdc_acm_transmission_status = UX_FALSE;

#endif /* UX_DEVICE_CLASS_CDC_ACM_TRANSMISSION_DISABLE */

            /* Read the received data in blocking mode */
            ux_device_class_cdc_acm_read(cdc_acm, (UCHAR *) usbrxbuf, 512,
                                         &actual_length);
            /*Have read datas*/
            if (actual_length != 0) {

            }
        } else {
            /* Sleep thread for 1ms */
            tx_thread_sleep(10);
        }
    }
}


/**
  * @brief  Function implementing usbx_cdc_acm_write_thread_entry.
  * @param  thread_input: Not used
  * @retval none
  */
VOID usbx_cdc_acm_write_thread_entry(ULONG thread_input) {
    ULONG actual_length;
    UX_SLAVE_DEVICE *device;
    device = &_ux_system_slave->ux_system_slave_device;

    UX_PARAMETER_NOT_USED(thread_input);
    /*Wait for mutex*/
    tx_thread_sleep(100);

    while (true) {
        /* Check if device is configured */
        if ((device->ux_slave_device_state == UX_DEVICE_CONFIGURED) && (cdc_acm != UX_NULL)) {

#ifndef UX_DEVICE_CLASS_CDC_ACM_TRANSMISSION_DISABLE
            /* Set transmission_status to UX_FALSE for the first time */
            cdc_acm->ux_slave_class_cdc_acm_transmission_status = UX_FALSE;
#endif

            /*Get data and transmit*/
//            while (true) {
//                /*Send data to usbx*/
                ux_device_class_cdc_acm_write(cdc_acm, (UCHAR *) usbtxbuf, 3, &actual_length);
                tx_thread_sleep(100);
//            }
//            tx_thread_sleep(1);
        } else {
            /*Sleep 1ms*/
            tx_thread_sleep(1);
        }
    }
}