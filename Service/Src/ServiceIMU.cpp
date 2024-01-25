#include "main.h"
#include "spi.h"
#include "DL_F407.h"
#include "DWT.hpp"
#include "om.h"
#include "Flash.hpp"
#include "Filter.hpp"
#include "FishMessage.h"
#include "ServiceIMU.h"
#include "libspi-i-hal-1.0.hpp"
#include "libpid-i-1.0.hpp"
#include "libbmi088-1.0.hpp"
#include "QuaternionEKF.h"

using namespace SPI;
using namespace BMI088;
using namespace IMUA;
using namespace PID;

static uint8_t BMI088_Config(cBMI088 &bmi088);

extern bool imu_rst;
static cIMUA *imu_handle = nullptr;

SRAM_SET_CCM TX_THREAD IMUThread;
SRAM_SET_CCM uint8_t IMUThreadStack[1024] = {0};

[[noreturn]] void IMUThreadFun(ULONG initial_input) {
    UNUSED(initial_input);
    /* INS Topic */
    om_topic_t *ins_topic = om_config_topic(nullptr, "CA", "INS", sizeof(Msg_INS_t));

    cDWT dwt;
    bool calibrate = !LL_GPIO_IsInputPinSet(KEY_GPIO_Port, KEY_Pin);
    int16_t accel[3];
    int16_t gyro[3];
    float accel_f[3];
    float gyro_f[3];
    float gyro_offset[3] = {0};
    float quaternion[4] = {1.0f, 0.0f, 0.0f, 0.0f};
    cFilterBTW2_100Hz filter[3];
    Msg_INS_t msg_ins{};

    flashCore.flash_memcpy(Flash::Element_ID_GYRO, (uint8_t *) gyro_offset);

    cSPI spi_accel(&hspi1, CS1_ACCLE_GPIO_Port, CS1_ACCLE_Pin, UINT32_MAX);
    cSPI spi_gyro(&hspi1, CS1_GYRO_GPIO_Port, CS1_GYRO_Pin, UINT32_MAX);
    cBMI088 bmi088(&spi_accel, &spi_gyro);
    imu_handle = &bmi088;
    BMI088_Config(bmi088);

    if (calibrate) {
        if (!LL_TIM_IsEnabledCounter(TIM4)) {
            LL_TIM_CC_EnableChannel(TIM4, LL_TIM_CHANNEL_CH3);
            LL_TIM_EnableAllOutputs(TIM4);
            LL_TIM_EnableCounter(TIM4);
        }
        accel[0] = 199;
        accel[1] = 0;
        LL_TIM_OC_SetCompareCH3(TIM4, accel[0]);
        tx_thread_sleep(300);
        LL_TIM_OC_SetCompareCH3(TIM4, 0);

        gyro_offset[0] = 0.0f;
        gyro_offset[1] = 0.0f;
        gyro_offset[2] = 0.0f;

        while (bmi088.GetTem() < 49.0f) {
            tx_thread_sleep(100);
        }
        tx_thread_sleep(5000);
        /*Calibrate Gyro for 100 s*/
        for (uint16_t i = 0; i < 10000; i++) {
            bmi088.GetGyro((uint8_t *) gyro);
            gyro_offset[0] -= (float) gyro[0] / 10000.0f;
            gyro_offset[1] -= (float) gyro[1] / 10000.0f;
            gyro_offset[2] -= (float) gyro[2] / 10000.0f;
            if (++accel[1] == 100) {
                LL_TIM_OC_SetCompareCH3(TIM4, accel[0]);
                accel[0] = accel[0] ? 0 : 199;
                accel[1] = 0;
            }
            tx_thread_sleep(10);
        }
        LL_TIM_OC_SetCompareCH3(TIM4, accel[0]);
        flashCore.config_data(Flash::Element_ID_GYRO, (uint8_t *) gyro_offset, sizeof(gyro_offset));
        __disable_interrupts();
        flashCore.rebuild();
        NVIC_SystemReset();
    }

    IMU_QuaternionEKF_Init(10, 0.001, 10000000, 1);
    tx_thread_sleep(1000);
    dwt.update();
    for (;;) {

        /*Rst quaternion*/
        if (imu_rst) {
            imu_rst = false;
            IMU_QuaternionEKF_Reset();
        }

        tx_thread_sleep(1);
        bmi088.GetAccel((uint8_t *) accel);
        bmi088.GetGyro((uint8_t *) gyro);
        accel_f[0] = (float) accel[0] * LSB_ACC_16B_12G;
        accel_f[1] = (float) accel[1] * LSB_ACC_16B_12G;
        accel_f[2] = (float) accel[2] * LSB_ACC_16B_12G;

        /*100Hz LowPass BWT 2-Order*/
        /*Watch out! Orientation R-F-U*/
        accel_f[0] = filter[0].Update(accel_f[0]);
        accel_f[1] = filter[1].Update(accel_f[1]);
        accel_f[2] = filter[2].Update(accel_f[2]);

        gyro_f[0] = ((float) gyro[0] + gyro_offset[0]) * LSB_GYRO_16B_1000_R;
        gyro_f[1] = ((float) gyro[1] + gyro_offset[1]) * LSB_GYRO_16B_1000_R;
        gyro_f[2] = ((float) gyro[2] + gyro_offset[2]) * LSB_GYRO_16B_1000_R;

        IMU_QuaternionEKF_Update(quaternion, gyro_f[0], gyro_f[1], gyro_f[2], accel_f[0], accel_f[1], accel_f[2],
                                 dwt.dt_sec());

        /*Message*/
        msg_ins.timestamp = tx_time_get();
        memcpy(msg_ins.quaternion, quaternion, sizeof(quaternion));
        /*R-F-U*/
//        msg_ins.Euler[0] = atan2f(2.0f * (quaternion[0] * quaternion[3] + quaternion[1] * quaternion[2]),
//                                  2.0f * (quaternion[0] * quaternion[0] + quaternion[1] * quaternion[1]) - 1.0f) *
//                           57.295779513f;
//        msg_ins.Euler[1] = atan2f(2.0f * (quaternion[0] * quaternion[1] + quaternion[2] * quaternion[3]),
//                                  2.0f * (quaternion[0] * quaternion[0] + quaternion[3] * quaternion[3]) - 1.0f) *
//                           57.295779513f;
//        msg_ins.Euler[2] =
//                asinf(-2.0f * (quaternion[1] * quaternion[3] - quaternion[0] * quaternion[2])) * 57.295779513f;

        msg_ins.accel[0] = accel_f[0];
        msg_ins.accel[1] = accel_f[1];
        msg_ins.accel[2] = accel_f[2];
        msg_ins.gyro[0] = gyro_f[0];
        msg_ins.gyro[1] = gyro_f[1];
        msg_ins.gyro[2] = gyro_f[2];

        om_publish(ins_topic, &msg_ins, sizeof(msg_ins), true, false);
    }
}

SRAM_SET_CCM TX_THREAD IMUTemThread;
SRAM_SET_CCM uint8_t IMUTemThreadStack[512] = {0};

[[noreturn]] void IMUTemThreadFun(ULONG initial_input) {
    UNUSED(initial_input);
    cDWT dwt;
    PID_Inc_f pid(100.0f, 8.0f, 30.0f, 0.0f, 0.32, 1999, 399, false, 0, true, 0.5f);
    LL_TIM_EnableAllOutputs(TIM10);
    LL_TIM_CC_EnableChannel(TIM10, LL_TIM_CHANNEL_CH1);
    LL_TIM_OC_SetCompareCH1(TIM10, 1999);
    LL_TIM_EnableCounter(TIM10);
    tx_thread_sleep(3000);
    /*Assert*/
    if (imu_handle == nullptr) {
        LL_TIM_OC_SetCompareCH1(TIM10, 0);
        tx_thread_suspend(&IMUTemThread);
    }
    while (imu_handle->GetTem() < 47.5f) {
        tx_thread_sleep(320);
    }
    pid.SetRef(50.0f);
    dwt.update();
    for (;;) {
        tx_thread_sleep(320);
        LL_TIM_OC_SetCompareCH1(TIM10, (uint32_t) pid.Calculate(imu_handle->GetTem(), dwt.dt_sec()));
    }
}

/*Gyro*/
void EXTI9_5_IRQHandler() {
    static uint16_t cnt;
    if (cnt++ == 640) {
        cnt = 0;
        imu_handle->UpdateTem();
    }
    imu_handle->UpdateGyro();
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_5);
}

/*Accel*/
void EXTI4_IRQHandler() {
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
    /*Enable accelerometer */
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
