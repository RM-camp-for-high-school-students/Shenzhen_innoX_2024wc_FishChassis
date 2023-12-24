/*
 * @Description:
 * @Author: qianwan
 * @Date: 2023-09-29 14:19:38
 * @LastEditTime: 2023-12-18 23:30:25
 * @LastEditors: qianwan
 */
#include "main.h"
#include "app_usbx_device.h"
#include "ux_device_class_cdc_acm.h"

#include "ServiceMsgCDC.h"
#include "FishMessage.h"

#define USB_MAX_LEN UX_SLAVE_REQUEST_DATA_MAX_LENGTH

/*USB CDC-ACM pointer*/
extern UX_SLAVE_CLASS_CDC_ACM *cdc_acm;

/* Data received are stored in this buffer */
SRAM_SET_CCM_UNINT uint8_t usb_rx_buf[USB_MAX_LEN];

/* Data to send over USB CDC are stored in this buffer   */
SRAM_SET_CCM_UNINT uint8_t usb_tx_buf[USB_MAX_LEN] = {0x01, 0x02, 0x03};

/**
  * @brief  Function implementing usbx_cdc_acm_thread_entry.
  * @param arg: Not used
  * @retval None
  */
[[noreturn]] VOID usbx_cdc_acm_read_thread_entry(ULONG arg) {
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
            ux_device_class_cdc_acm_read(cdc_acm, (UCHAR *) usb_rx_buf, 512,
                                         &actual_length);
            /*Have read data*/
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
[[noreturn]] VOID usbx_cdc_acm_write_thread_entry(ULONG thread_input) {
    ULONG actual_length;
    UX_SLAVE_DEVICE *device;
    device = &_ux_system_slave->ux_system_slave_device;
    UX_PARAMETER_NOT_USED(thread_input);

    /*INS Message*/
    uint32_t len;
    Msg_Ins_t msg_ins;
    om_suber_t *suber_ins = om_subscribe(om_find_topic("INS", UINT32_MAX));

    /*Wait for mutex*/
    tx_thread_sleep(5000);

    while (true) {
        /* Check if device is configured */
        while ((device->ux_slave_device_state == UX_DEVICE_CONFIGURED) && (cdc_acm != UX_NULL)) {

#ifndef UX_DEVICE_CLASS_CDC_ACM_TRANSMISSION_DISABLE
            /* Set transmission_status to UX_FALSE for the first time */
            cdc_acm->ux_slave_class_cdc_acm_transmission_status = UX_FALSE;
#endif

            om_suber_export(suber_ins, &msg_ins, false);
            len = fishPrintf(usb_tx_buf, "Y=%.2f,R=%.2f,P=%.2f,S=%d\r\n", msg_ins.Euler[0], msg_ins.Euler[1],
                             msg_ins.Euler[2], msg_ins.timestamp);
            ux_device_class_cdc_acm_write(cdc_acm, (UCHAR *) usb_tx_buf, len, &actual_length);
            tx_thread_sleep(100);
        }
        /*Sleep 1ms*/
        tx_thread_sleep(100);
    }

}