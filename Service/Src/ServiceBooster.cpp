/*
 * @Description: 
 * @Author: qianwan
 * @Date: 2023-12-20 16:01:58
 * @LastEditTime: 2023-12-23 19:15:00
 * @LastEditors: qianwan
 */
#include "app_threadx.h"
#include "DL_F407.h"
#include "ServiceBooster.h"
#include "FishMessage.h"
#include "om.h"

/*Communication pool*/
SRAM_SET_CCM TX_BYTE_POOL ComPool;
UCHAR COM_PoolBuf[4096] = {0};

SRAM_SET_CCM TX_BYTE_POOL MsgPool;
SRAM_SET_CCM UCHAR Msg_PoolBuf[4096] = {0};

SRAM_SET_CCM TX_BYTE_POOL MathPool;
SRAM_SET_CCM UCHAR Math_PoolBuf[40960] = {0};
//extern TX_THREAD DebugThread;
//extern uint8_t DebugThreadStack[256];
//extern void DebugThreadFun(ULONG initial_input);

extern TX_THREAD IMUThread;
extern uint8_t IMUThreadStack[1024];
extern void IMUThreadFun(ULONG initial_input);

void Service_Booster(void) {
    /**********Memory pool in ram***********/
    /*Communication pool in ram*/
    tx_byte_pool_create(
            &ComPool,
            (CHAR *) "COM_PoolBuf",
            COM_PoolBuf,
            sizeof(COM_PoolBuf));
    /*Communication pool in ccram*/
    tx_byte_pool_create(
            &MsgPool,
            (CHAR *) "Msg_Pool",
            Msg_PoolBuf,
            sizeof(Msg_PoolBuf));
    /*Math pool in ccram*/
    tx_byte_pool_create(
            &MathPool,
            (CHAR *) "Math_Pool",
            Math_PoolBuf,
            sizeof(Math_PoolBuf));

    /*Enable OneMessage Service*/
    om_init();

/**********信号量***********/
//	tx_semaphore_create(
//		&MotorHS100Sem,
//		(CHAR*)"MotorHS100Sem",
//		0
//		);


/***********互斥量************/

//	tx_mutex_create(
//		&msgtubeLock,
//		(CHAR*)"TubeMutex",
//		TX_NO_INHERIT);

/**********消息队列***********/
//	/*DBUS*/
//	tx_queue_create(
//		&RemoterRXQue,
//		(CHAR*)"REMOTERQUE",
//		4,
//		RemoterQueueStack,
//		sizeof(RemoterQueueStack));

/**********进程***********/

	tx_thread_create(
		&IMUThread,
		(CHAR*)"IMU",
        IMUThreadFun,
		0x0000,
        IMUThreadStack,
		sizeof(IMUThreadStack),
		3,
		3,
		TX_NO_TIME_SLICE,
		TX_AUTO_START);

}

uint32_t fishPrintf(uint8_t *buf, const char *str, ...) {
    /*计算字符串长度,并将字符串输出到数据区*/
    va_list ap;
    va_start(ap, str);
    uint32_t len = vsnprintf((char *) buf, 256, str, ap);
    va_end(ap);
    return len;
}
