#include "main.h"
#include "spi.h"
#include "DL_F407.h"
#include "ServiceIMU.h"
#include "libspi-i-hal-1.0.hpp"
#include "libbmi088-1.0.hpp"

using namespace SPI;
using namespace BMI088;
using namespace IMUA;

static void BMI088_Config(cBMI088 &bmi088);

cIMUA *imu;

SRAM_SET_CCM TX_THREAD IMUThread;
SRAM_SET_CCM uint8_t IMUThreadStack[2048] = {0};
float accel_f[3];
float gyro_f[3];
float temperature_f;
[[noreturn]] void IMUThreadFun(ULONG initial_input) {
    tx_thread_sleep(1000);
    cSPI spi_accel(&hspi1, CS1_ACCLE_GPIO_Port, CS1_ACCLE_Pin, UINT32_MAX);
    cSPI spi_gyro(&hspi1, CS1_GYRO_GPIO_Port, CS1_GYRO_Pin, UINT32_MAX);
    cBMI088 bmi088(&spi_accel, &spi_gyro);
    imu = &bmi088;
    BMI088_Config(bmi088);
    tx_thread_sleep(1000);
    int16_t accel[3];
    int16_t gyro[3];
    for (;;) {
        
        bmi088.GetAccel((uint8_t*)accel);
        bmi088.GetGyro((uint8_t*)gyro);
        gyro_f[0] = gyro[0] * 0.030517578125f;
        gyro_f[1] = gyro[1] * 0.030517578125f;
        gyro_f[2] = gyro[2] * 0.030517578125f;

        accel_f[0] = accel[0] * 0.0003662109375f;
        accel_f[1] = accel[1] * 0.0003662109375f;
        accel_f[2] = accel[2] * 0.0003662109375f;

        temperature_f = bmi088.GetTem();
        tx_thread_sleep(200);
    }
}

/*Gyro*/
void EXTI9_5_IRQHandler() {
    imu->UpdateGyro(); 
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_5);
}

/*Accel*/
void EXTI4_IRQHandler() {
    imu->UpdateAccel();
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_4);
}

static void BMI088_Config(cBMI088 &bmi088) {
    uint8_t tmp;
    /*Begin ACC SPI Communication*/
    bmi088.ReadReg(0x00, BMI088_CS::CS_ACCEL);
    tx_thread_sleep(1);


    /*Reset Sensor*/
    bmi088.WriteReg(0x14, 0xB6, BMI088_CS::CS_GYRO);
    bmi088.WriteReg(0x7E, 0xB6,BMI088_CS::CS_ACCEL);
    tx_thread_sleep(100);


    /*Begin ACC SPI Communication*/
    bmi088.ReadReg(0x00, BMI088_CS::CS_ACCEL);
    tx_thread_sleep(1);
    bmi088.ReadReg(0x00, BMI088_CS::CS_ACCEL);
    tx_thread_sleep(1);


    /*Check Chip Connection*/
    while (bmi088.ReadReg(0x00, BMI088_CS::CS_GYRO) != 0x0F) {
        tx_thread_sleep(TX_WAIT_FOREVER);
        //ERROR
    }
    /*Check Chip Connection*/
    while (bmi088.ReadReg(0x00, BMI088_CS::CS_ACCEL) != 0x1E) {
        tx_thread_sleep(TX_WAIT_FOREVER);
        //ERROR
    }
    tx_thread_sleep(10);

    /*Start to config IMU*/
    /*Normal Mode*/
    bmi088.WriteReg(0x7C,0x00,BMI088_CS::CS_ACCEL);
    tx_thread_sleep(10);
    /*Enable accelmemter */
    bmi088.WriteReg(0x7D,0x04,BMI088_CS::CS_ACCEL);
    tx_thread_sleep(10);
    /*Gyro range 1000dps*/
    bmi088.WriteReg(0x0F,0x01,BMI088_CS::CS_GYRO);
    tx_thread_sleep(1);
    /*Accel range +-12G */
    bmi088.WriteReg(0x41,0x02,BMI088_CS::CS_ACCEL);
    tx_thread_sleep(1);
    /*Gyro ODR 1KHz BW 116Hz*/
    bmi088.WriteReg(0x10,0x02,BMI088_CS::CS_GYRO);
    tx_thread_sleep(1);
    /*Accel ODR 1.6KHz BW 280Hz*/
    bmi088.WriteReg(0x40,0xAC,BMI088_CS::CS_ACCEL);
    tx_thread_sleep(1);
    /*Gyro INT DRY*/
    bmi088.WriteReg(0x15,0x80,BMI088_CS::CS_GYRO);
    tx_thread_sleep(1);
    /*Gyro INT3 PP AH*//*error*/
    bmi088.WriteReg(0x16,0x0D,BMI088_CS::CS_GYRO);
    tx_thread_sleep(1);
    /*Accel INT1 PP AH*//*error*/
    bmi088.WriteReg(0x53,0x05,BMI088_CS::CS_ACCEL);
    tx_thread_sleep(1);
    /*Gyro DRY pin to INT3*/
    bmi088.WriteReg(0x18,0x01,BMI088_CS::CS_GYRO);
    tx_thread_sleep(1);
    /*Accel DRY pin to INT1*/
    bmi088.WriteReg(0x58,0x04,BMI088_CS::CS_ACCEL);
    tx_thread_sleep(1);


    /*Enable IDLE*/
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_4);
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_5);
    NVIC_EnableIRQ(EXTI4_IRQn);//ACC
    NVIC_EnableIRQ(EXTI9_5_IRQn);//GYRO
}