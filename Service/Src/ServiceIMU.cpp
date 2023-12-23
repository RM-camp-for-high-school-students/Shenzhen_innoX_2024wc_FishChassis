#include "main.h"
#include "spi.h"
#include "DL_F407.h"
#include "ServiceIMU.h"
#include "libspi-i-hal-1.0.hpp"
#include "libbmi088-1.0.hpp"
#include "Filter.hpp"
#include "kalman_filter.h"
#include "QuaternionEKF.h"

using namespace SPI;
using namespace BMI088;
using namespace IMUA;

static uint8_t BMI088_Config(cBMI088 &bmi088);
static cBMI088 *imu_handle = nullptr;

SRAM_SET_CCM TX_THREAD IMUThread;
SRAM_SET_CCM uint8_t IMUThreadStack[2048] = {0};
float quaternion[4] = {1.0f, 0.0f, 0.0f, 0.0f};
float accel_f[3];
float gyro_f[3];
float ora[3];
uint32_t freq_a_t;
uint32_t freq_g_t;
uint32_t freq_a;
uint32_t freq_g;


[[noreturn]] void IMUThreadFun(ULONG initial_input) {
    cSPI spi_accel(&hspi1, CS1_ACCLE_GPIO_Port, CS1_ACCLE_Pin, UINT32_MAX);
    cSPI spi_gyro(&hspi1, CS1_GYRO_GPIO_Port, CS1_GYRO_Pin, UINT32_MAX);
    cBMI088 bmi088(&spi_accel, &spi_gyro);
    imu_handle = &bmi088;

    BMI088_Config(bmi088);
    tx_thread_sleep(100);

    IMU_QuaternionEKF_Init(10, 0.001, 10000000, 1, 0.001f);


    int16_t accel[3];
    int16_t gyro[3];
    cFilterBTW2_100Hz filter[3];

    tx_thread_sleep(2000);
    for (;;) {
        bmi088.GetAccel((uint8_t *) accel);
        bmi088.GetGyro((uint8_t *) gyro);
        accel_f[0] = accel[0] * LSB_ACC_16B_12G;
        accel_f[1] = accel[1] * LSB_ACC_16B_12G;
        accel_f[2] = accel[2] * LSB_ACC_16B_12G;

        /*100Hz Lowpass BWT 2-Order*/
        accel_f[0] = filter[0].Update(accel_f[0]);
        accel_f[1] = filter[1].Update(accel_f[1]);
        accel_f[2] = filter[2].Update(accel_f[2]);

        gyro_f[0] = gyro[0] * LSB_GYRO_16B_1000_R;
        gyro_f[1] = gyro[1] * LSB_GYRO_16B_1000_R;
        gyro_f[2] = gyro[2] * LSB_GYRO_16B_1000_R;

        IMU_QuaternionEKF_Update(quaternion, gyro_f[0], gyro_f[1], gyro_f[2], accel_f[0], accel_f[1], accel_f[2]);
        
        
        ora[0] = atan2f(2.0f * (quaternion[0] * quaternion[3] + quaternion[1] * quaternion[2]), 1 - 2.0f * (quaternion[2] * quaternion[2] + quaternion[3] * quaternion[3])) * 57.295779513f;
        ora[1] = atan2f(2.0f * (quaternion[0] * quaternion[1] + quaternion[2] * quaternion[3]), 1 - 2.0f * (quaternion[1] * quaternion[1] + quaternion[2] * quaternion[2])) * 57.295779513f;
        ora[2] = asinf( 2.0f * (quaternion[0] * quaternion[2] - quaternion[1] * quaternion[3])) * 57.295779513f;

        freq_g = freq_g_t;
        freq_a = freq_a_t;
        freq_g_t = 0;
        freq_a_t = 0;
        tx_thread_sleep(1);
    }
}

/*Gyro*/
void EXTI9_5_IRQHandler() {
    static uint16_t cnt;
    freq_g_t++;
    if (cnt++ == 640) {
        cnt = 0;
        imu_handle->UpdateTem();
    }
    imu_handle->UpdateGyro();
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_5);
}

/*Accel*/
void EXTI4_IRQHandler() {
    // freq_a_t++;
    imu_handle->UpdateAccel();
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_4);
}

static uint8_t BMI088_Config(cBMI088 &bmi088) {
    /*Begin ACC SPI Communication*/
    bmi088.ReadReg(0x00, BMI088_CS::CS_ACCEL);
    tx_thread_sleep(1);
    /*Reset Sensor*/
    bmi088.WriteReg(0x14, 0xB6, BMI088_CS::CS_GYRO);
    tx_thread_sleep(100);
    bmi088.WriteReg(0x7E, 0xB6, BMI088_CS::CS_ACCEL);
    tx_thread_sleep(100);
    /*Begin ACC SPI Communication*/
    bmi088.ReadReg(0x00, BMI088_CS::CS_ACCEL);
    tx_thread_sleep(1);
    bmi088.ReadReg(0x00, BMI088_CS::CS_ACCEL);
    tx_thread_sleep(1);

    /*Check Chip Connection*/
    while (bmi088.ReadReg(0x00, BMI088_CS::CS_GYRO) != 0x0F) {
        return 0x01;
        //ERROR
    }
    /*Check Chip Connection*/
    while (bmi088.ReadReg(0x00, BMI088_CS::CS_ACCEL) != 0x1E) {
        return 0x02;
        //ERROR
    }
    tx_thread_sleep(10);

    /*Start to config IMU*/
    /*Enable accelmemter */
    bmi088.WriteReg(0x7D, 0x04, BMI088_CS::CS_ACCEL);
    tx_thread_sleep(10);
    /*Normal Mode*/
    bmi088.WriteReg(0x7C, 0x00, BMI088_CS::CS_ACCEL);
    tx_thread_sleep(10);
    /*Accel range +-12G */
    bmi088.WriteReg(0x41, 0x02, BMI088_CS::CS_ACCEL);
    tx_thread_sleep(5);
    /*Accel ODR 1.6KHz BW 280Hz*/
    bmi088.WriteReg(0x40, 0xAC, BMI088_CS::CS_ACCEL);
    tx_thread_sleep(5);
    /*Accel INT1 PP AL*/
    bmi088.WriteReg(0x53, 0x08, BMI088_CS::CS_ACCEL);
    tx_thread_sleep(5);
    /*Accel DRY pin to INT1*/
    bmi088.WriteReg(0x58, 0x04, BMI088_CS::CS_ACCEL);
    tx_thread_sleep(5);

    /*Gyro range 1000dps*/
    bmi088.WriteReg(0x0F, 0x01, BMI088_CS::CS_GYRO);
    tx_thread_sleep(5);
    /*Gyro ODR 1KHz BW 116Hz*/
    bmi088.WriteReg(0x10, 0x02, BMI088_CS::CS_GYRO);
    tx_thread_sleep(5);
    /*Gyro Normal Mode*/
    bmi088.WriteReg(0x11, 0x00, BMI088_CS::CS_GYRO);
    tx_thread_sleep(5);
    /*Gyro INT DRY*/
    bmi088.WriteReg(0x15, 0x80, BMI088_CS::CS_GYRO);
    tx_thread_sleep(5);
    /*Gyro INT3 PP AL*/
    bmi088.WriteReg(0x16, 0x0C, BMI088_CS::CS_GYRO);
    tx_thread_sleep(5);
    /*Gyro DRY pin to INT3*/
    bmi088.WriteReg(0x18, 0x01, BMI088_CS::CS_GYRO);
    tx_thread_sleep(5);

    /*Enable IDLE*/
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_4);
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_5);
    NVIC_EnableIRQ(EXTI4_IRQn);//ACC
    NVIC_EnableIRQ(EXTI9_5_IRQn);//GYRO
    return 0;
}
