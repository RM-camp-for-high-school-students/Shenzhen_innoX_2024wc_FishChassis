#include "main.h"

#include "DL_F407.h"
#include "TaskBooster.h"

#include "functional"
#include "cstdarg"
#include "string"

#include "app_threadx.h"
#include "algorithm"
#include "cmsis_gcc.h"

using namespace std;

#include "arm_math.h"
#include "spi.h"



/*TraceX utilities*/
#define TRC_BUF_SIZE (500*256)            /* Buffer size */
#define TRC_MAX_OBJ_COUNT (40)         /* Max number of ThreadX objects */
static UCHAR TraceXBuf[TRC_BUF_SIZE];
UINT TraceStatus = 0;

/*UART pool*/
extern TX_BYTE_POOL ComPool;

//extern MSGTube::cMSGTube* msgTube;

uint8_t DebugPrintf(uint8_t *buf, const char *str, ...) {
    /*计算字符串长度,并将字符串输出到数据区*/
    va_list ap;
    va_start(ap, str);
    uint8_t len = vsnprintf((char *) buf, 256, str, ap);
    va_end(ap);
    return len;
}

SRAM_SET_CCM TX_THREAD Test01Thread;
SRAM_SET_CCM uint8_t Test01ThreadStack[2048] = {0};

SRAM_SET_CCM TX_THREAD Test02Thread;
SRAM_SET_CCM uint8_t Test02ThreadStack[2048] = {0};

SRAM_SET_CCM TX_THREAD Test03Thread;
SRAM_SET_CCM uint8_t Test03ThreadStack[2048] = {0};

SRAM_SET_CCM TX_THREAD HeartBeatThread;
SRAM_SET_CCM uint8_t HeartBeatThreadStack[2048] = {0};

SRAM_SET_CCM TX_THREAD DebugThread;
SRAM_SET_CCM uint8_t DebugThreadStack[256] = {0};

[[noreturn]] void HeartBeatThreadFun(ULONG initial_input) {

    LL_TIM_EnableAllOutputs(TIM5);
    LL_TIM_CC_EnableChannel(TIM5, LL_TIM_CHANNEL_CH1);
    LL_TIM_CC_EnableChannel(TIM5, LL_TIM_CHANNEL_CH2);
    LL_TIM_CC_EnableChannel(TIM5, LL_TIM_CHANNEL_CH3);
    LL_TIM_EnableCounter(TIM5);
    for (;;) {
        LL_TIM_OC_SetCompareCH1(TIM5, 1999);
        LL_TIM_OC_SetCompareCH2(TIM5, 1999);
        LL_TIM_OC_SetCompareCH3(TIM5, 1999);
        tx_thread_sleep(500);
        LL_TIM_OC_SetCompareCH1(TIM5, 0);
        LL_TIM_OC_SetCompareCH2(TIM5, 0);
        LL_TIM_OC_SetCompareCH3(TIM5, 0);
        tx_thread_sleep(500);
    }
}

//UARTLL::cUART* pUARTDebug;
//void DebugThreadFun(ULONG initial_input)
//{
//
//	UARTLL::cUART UARTDebug;
//	pUARTDebug = &UARTDebug;
//	/*对UART初始化*/
//	UARTDebug.UART_Init(USART6,DMA2,LL_DMA_STREAM_6);
//
//	tx_thread_sleep(100);
//	/*Transmit buf*/
//
//	for(;;)
//	{
//		//Txlen = DebugPrintf(pbufx,"Hello");
//		// UARTDebug.Transmit_DMA(pbufx,Txlen);
//		tx_thread_sleep(50);
//	}
//}

ULONG spi2_time=0;
uint8_t *pspi_tx_data;
uint8_t *pspi_rx_data;
uint8_t spi_tx_data_buf[48];
uint8_t spi_rx_data_buf[48];

[[noreturn]] void Test01ThreadFun(ULONG initial_input) {
    for (uint32_t i = 0; i < 24; i++) {
        spi_tx_data_buf[i] = i;
        spi_tx_data_buf[i+24] = i;
    }
    tx_thread_sleep(10);
    /* SPI2-CS NVIC enable */
    pspi_tx_data = spi_tx_data_buf;
    pspi_rx_data = spi_rx_data_buf;
    NVIC_SetPriority(EXTI15_10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(EXTI15_10_IRQn);

    for (;;) {
        if(HAL_SPI_GetState(&hspi2)==HAL_SPI_STATE_READY){
            HAL_SPI_TransmitReceive_DMA(&hspi2,pspi_tx_data,pspi_rx_data,24);
        }
        else if(tx_time_get()-spi2_time>20){
            HAL_SPI_Abort(&hspi2);
            spi2_time = tx_time_get();
        }
        tx_thread_sleep(1);
    }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi){
    if(hspi==&hspi2){
        spi2_time = tx_time_get();
    }
}


[[noreturn]] void Test02ThreadFun(ULONG initial_input) {
    while (true) {

    }
}

[[noreturn]] void Test03ThreadFun(ULONG initial_input) {
    tx_thread_sleep(10);
    for (;;) {

    }
}