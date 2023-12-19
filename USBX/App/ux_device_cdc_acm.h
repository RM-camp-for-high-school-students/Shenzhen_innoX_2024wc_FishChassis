/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_device_cdc_acm.h
  * @author  MCD Application Team
  * @brief   USBX Device CDC ACM interface header file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UX_DEVICE_CDC_ACM_H__
#define __UX_DEVICE_CDC_ACM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ux_api.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/

/* USER CODE BEGIN EFP */
VOID USBD_CDC_ACM_ParameterChange(VOID *command);
void USBD_CDC_ACM_Deactivate(void *cdc_acm_instance);
void USBD_CDC_ACM_Activate(void *cdc_acm_instance);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE BEGIN PD */
#define APP_RX_DATA_SIZE                          2048
#define APP_TX_DATA_SIZE                          2048

/* Rx/TX flag */
#define RX_NEW_RECEIVED_DATA                      0x01
#define TX_NEW_TRANSMITTED_DATA                   0x02

/* Data length for vcp */
#define VCP_WORDLENGTH8                           8
#define VCP_WORDLENGTH9                           9

/* the minimum baudrate */
#define MIN_BAUDRATE                              9600
/* USER CODE END PD */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

#ifdef __cplusplus
}
#endif
#endif  /* __UX_DEVICE_CDC_ACM_H__ */
