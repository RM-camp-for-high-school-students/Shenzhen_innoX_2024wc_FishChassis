/*
 * @Description: Abstract layer of IMU driver
 * @Author: qianwan
 * @Date: 2023-11-09 12:24:41
 * @LastEditTime: 2023-11-09 20:59:23
 * @LastEditors: qianwan
 */
 /******************************************************************************
  * @attention
  * BSD 3-Clause License
  * Copyright (c) 2023, Qianwan.Jin
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************/
 /*Version 1.0*/
 /*Stepper 0.1*/
#pragma once
#ifndef LIB_IMU_A_
#define LIB_IMU_A_
#include <cstdint>
namespace IMUA {
    /*G = 9.7803 in Shenzhen*/
    #define LSB_ACC_16B_16G 0.004775537109
    #define LSB_ACC_16B_8G  0.002387768555
    #define LSB_ACC_16B_4G  0.001193884277
    #define LSB_ACC_16B_2G  0.000569421387

    /*Turn Into Radian*/
    #define LSB_GYRO_16B_2000_R 0.0010652644
    #define LSB_GYRO_16B_1000_R 0.00053263222
    #define LSB_GYRO_16B_500_R  0.00026631611
    #define LSB_GYRO_16B_250_R  0.00013315805
    #define LSB_GYRO_16B_125D_R 0.000066579027

    class cIMUA {
    public:
        /*Read raw acceleration data from imu*/
        virtual uint8_t UpdateAccel()=0; 
        /*Read raw gyroscope data from imu*/
        virtual uint8_t UpdateGyro()=0;
        /*Read temperature data from imu(optional)*/
        virtual uint8_t UpdateTem(){return 0x01;}
        /*Read all data*/
        virtual uint8_t UpdateAll()=0;
        /*Get data from class*/
        virtual void GetAccel(uint8_t* pdata) = 0;
        /*Get data from class*/
        virtual void GetGyro(uint8_t* pdata) = 0;
        /*Get temperature from class*/
        virtual float GetTem() { return 0xFF; }
    };
}  // namespace IMUA
#endif