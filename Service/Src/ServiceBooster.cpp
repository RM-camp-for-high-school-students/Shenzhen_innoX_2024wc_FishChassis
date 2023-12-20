#include "app_threadx.h"
#include "ServiceBooster.h"

TX_BYTE_POOL ComPool;
uint8_t UART_PoolBuf[10240]={0};

//extern TX_THREAD DebugThread;
//extern uint8_t DebugThreadStack[256];
//extern void DebugThreadFun(ULONG initial_input);

void Service_Booster(void)
{
/**********内存池***********/
	tx_byte_pool_create(
		&ComPool,
		(CHAR*)"UART_Pool",
		UART_PoolBuf,
		sizeof(UART_PoolBuf));
	

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

//	tx_thread_create(
//		&DebugThread,
//		(CHAR*)"DEBUG",
//		DebugThreadFun,
//		0x0000,
//		DebugThreadStack,
//		sizeof(DebugThreadStack),
//		10,
//		10,
//		TX_NO_TIME_SLICE,
//		TX_AUTO_START);

}
