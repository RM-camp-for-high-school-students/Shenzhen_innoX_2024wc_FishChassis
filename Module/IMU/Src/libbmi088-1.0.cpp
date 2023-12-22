/*
* @Description: A instantiation of IMU-BMI088
* @Author: qianwan
* @Date: 2023-12-22 12:00:00
 * @LastEditTime: 2023-12-23 00:28:21
 * @LastEditors: qianwan
*/

 /******************************************************************************
  * @attention
  * BSD 3-Clause License
  * Copyright (c) 2023, Qianwan.Jin
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************/
 /*Version 1.0*/
 /*Stepper 0.0*/
#include <cstring>
#include "libbmi088-1.0.hpp"
using namespace BMI088;
uint8_t cBMI088::ReadReg(uint8_t reg, BMI088_CS cs)
{
    _data_buf[0]=reg|0x80;
    _spi[cs]->CS_Enable();
    _spi[cs]->ExchangeByte(_data_buf,_data_buf+10,2);
    _spi[cs]->CS_Disable();
    return _data_buf[11];
}

uint8_t cBMI088::WriteReg(uint8_t reg, uint8_t data, BMI088_CS cs)
{
    _data_buf[0]=reg;
    _data_buf[1]=data;
    _spi[cs]->CS_Enable();
    _spi[cs]->ExchangeByte(_data_buf,_data_buf+10,2);
    _spi[cs]->CS_Disable();
    return 0;
}

uint8_t cBMI088::UpdateTem()
{
    _data_buf[0] = 0x22|0x80;
    _spi[CS_ACCEL]->CS_Enable();
    _data_buf[0] = _spi[CS_ACCEL]->ExchangeByte(_data_buf,_data_buf+10,3);
    _spi[CS_ACCEL]->CS_Disable();

    /*Check spi communication*/
    if(_data_buf[0]!=0){return 0x01;}
    uint16_t tmp = (_data_buf[11]<<3)|(_data_buf[12]>>5);
    if(tmp>1023){
        tmp -= 2048;
    }
    _temperature = tmp*0.125f + 23.0f;
    return 0;
}

uint8_t cBMI088::UpdateAccel()
{
    _data_buf[0] = 0x12|0x80;
    _spi[CS_ACCEL]->CS_Enable();
    _data_buf[0] = _spi[CS_ACCEL]->ExchangeByte(_data_buf,_data_buf+10,7);
    _spi[CS_ACCEL]->CS_Disable();

    /*Check spi communication*/
    if(_data_buf[0]!=0){return 0x01;}

    _accel[0] = (int16_t)(_data_buf[12]<<8) | _data_buf[11];
    _accel[1] = (int16_t)(_data_buf[14]<<8) | _data_buf[13];
    _accel[2] = (int16_t)(_data_buf[16]<<8) | _data_buf[15];
    return 0;
}

uint8_t cBMI088::UpdateGyro()
{
    _data_buf[0] = 0x02|0x80;
    _spi[CS_GYRO]->CS_Enable();
    _data_buf[0] = _spi[CS_GYRO]->ExchangeByte(_data_buf,_data_buf+10,7);
    _spi[CS_GYRO]->CS_Disable();

    /*Check spi communication*/
    if(_data_buf[7]!=0){return 0x01;}

    _gyro[0] = _data_buf[12]<<8 | _data_buf[11];
    _gyro[1] = _data_buf[14]<<8 | _data_buf[13];
    _gyro[2] = _data_buf[16]<<8 | _data_buf[15];
    return 0;
}

uint8_t cBMI088::UpdateAll(void)
{
    UpdateAccel();
    UpdateGyro();
    UpdateTem();
    return 0;
}

void cBMI088::GetAccel(uint8_t* pdata)
{
    memcpy(pdata,_accel,6);
}

void cBMI088::GetGyro(uint8_t* pdata)
{
    memcpy(pdata,_gyro,6);
}

float cBMI088::GetTem()
{
    return _temperature;
}

/*A example of configuration*/
// static void IMU_Init(cICM42688* icm42688)
// {
// 	uint8_t buf = 0;
// 	/*Set at Bank0*/
// 	icm42688->WriteReg(0x76,0x00);
// 	/*Soft reset*/
// 	icm42688->WriteReg(0x11,0x01);
//     tx_thread_sleep(5);
// 	/*Read interrput flag to set spi protocol*/
// 	buf = icm42688->ReadReg(0x2D);

// 	/*Set at Bank0*/
// 	icm42688->WriteReg(0x76,0x00);
// 	/*Interrupt pin set*/
// 	icm42688->WriteReg(0x14,0x12);//INT1 INT2 Pulse, Low is available
// 	/*Set gyro*/
// 	icm42688->WriteReg(0x4F,0x06);//2000dps 1KHz
// 	/*Set accel*/
// 	icm42688->WriteReg(0x50,0x06);//16G 1KHz

// 	/*INT_CONFIG0*/
// 	icm42688->WriteReg(0x63,0x00);//Null
// 	/*INT_CONFIG1*/
// 	icm42688->WriteReg(0x64,0x00);//Enable interrupt pin
// 	/*INT_SOURCE0*/
// 	icm42688->WriteReg(0x65,0x08);//DRDY INT1
// 	/*INT_SOURCE1*/
// 	icm42688->WriteReg(0x66,0x00);//Null
// 	/*INT_SOURCE3*/
// 	icm42688->WriteReg(0x68,0x00);//Null
// 	/*INT_SOURCE3*/
// 	icm42688->WriteReg(0x69,0x00);//Null

// /*****Anti-Aliasing Filter@536Hz*****/

// 	/*GYRO Anti-Aliasing Filter config*/
// 	/*Set to Bank1*/
// 	icm42688->WriteReg(0x76,0x01);
// 	/*Config anti-aliasing-filter of gyro*/
// 	icm42688->WriteReg(0x0B,0xA0);//Enable AAF and Notch filter
// 	icm42688->WriteReg(0x0C,0x0C);//GYRO_AAF_DELT 12 (default 13)
// 	icm42688->WriteReg(0x0D,0x90);//GYRO_AAF_DELTSQR 144 (default 170)
// 	icm42688->WriteReg(0x0E,0x80);//GYRO_AAF_BITSHIFT 8 (default 8)

// 	/*ACCEL Anti-Aliasing Filter config*/
// 	/*Set to bank2*/
// 	icm42688->WriteReg(0x76,0x02);
// 	/*Config anti-aliasing-filter of accel*/
// 	icm42688->WriteReg(0x03,0x18);//Enable Notch filter ACCEL_AFF_DELT 12 (default 24)
// 	icm42688->WriteReg(0x04,0x90);//ACCEL_AFF_DELTSQR 144 (default 64)
// 	icm42688->WriteReg(0x05,0x80);//ACCEL_AAF_BITSHIFT 8 (default 6)

// /********User filter********/

// 	/*Set to bank 0*/
// 	icm42688->WriteReg(0x76,0x00);
// 	/*Set filters order*/
// 	icm42688->WriteReg(0x51,0x80);//Temp@20Hz GYRO_UI_FILTER@1nd-order
// 	icm42688->WriteReg(0x53,0x0D);//GYRO_UI_FILTER@1nd-order
// 	/*User filter set*/
// 	icm42688->WriteReg(0x52,0xFF);//GYRO/ACCLE_UI_FILT_BW@0 -3BW=2096.3Hz NBW=2204.6Hz GroupLatancy=0.2ms


// /*****Set to normal mode*****/
//  /*Config mcu gpio interrupt*/
// 	NVIC_EnableIRQ(EXTI15_10_IRQn);
// 	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_11);
// 	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_11);
// 	NVIC_EnableIRQ(EXTI1_IRQn);
// 	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_1);
// 	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_1);

// 	/*Set to bank0*/
// 	icm42688->WriteReg(0x76,0x00);
// 	/*Set power manager*/
// 	icm42688->WriteReg(0x4E,0x0F);//ACC GYRO enable at LowNoise Mode
// }