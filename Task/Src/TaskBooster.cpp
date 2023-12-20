#include "app_threadx.h"
#include "TaskBooster.h"

extern TX_THREAD Test01Thread;
extern uint8_t Test01ThreadStack[2048];
extern void Test01ThreadFun(ULONG initial_input);

extern TX_THREAD Test02Thread;
extern uint8_t Test02ThreadStack[2048];
extern void Test02ThreadFun(ULONG initial_input);

extern TX_THREAD Test03Thread;
extern uint8_t Test03ThreadStack[2048];
extern void Test03ThreadFun(ULONG initial_input);

extern TX_THREAD HeartBeatThread;
extern uint8_t HeartBeatThreadStack[2048];

extern void HeartBeatThreadFun(ULONG initial_input);

void Task_Booster()
{


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
	 	&Test01Thread,
	 	(CHAR*)"TEST1",
	 	Test01ThreadFun,
	 	0x0000,
	 	Test01ThreadStack,
	 	sizeof(Test01ThreadStack),
	 	5,
	 	5,
	 	TX_NO_TIME_SLICE,
	 	TX_AUTO_START);

//	tx_thread_create(
//		&Test02Thread,
//		(CHAR*)"TEST2",
//		Test02ThreadFun,
//		0x0000,
//		Test02ThreadStack,
//		sizeof(Test02ThreadStack),
//		5,
//		5,
//		TX_NO_TIME_SLICE,
//		TX_AUTO_START);


//	 tx_thread_create(
//	 	&Test03Thread,
//	 	(CHAR*)"TEST3",
//	 	Test03ThreadFun,
//	 	0x0000,
//	 	Test03ThreadStack,
//	 	sizeof(Test03ThreadStack),
//	 	3,
//	 	3,
//	 	TX_NO_TIME_SLICE,
//	 	TX_AUTO_START);

		tx_thread_create(
		&HeartBeatThread, 
		(CHAR*)"HeatBeat",
		HeartBeatThreadFun, 
		0x0000,
		HeartBeatThreadStack,
		sizeof(HeartBeatThreadStack),
		15,
		15,
		TX_NO_TIME_SLICE,
		TX_AUTO_START);	
}
