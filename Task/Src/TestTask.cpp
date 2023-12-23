#include "main.h"
#include "DL_F407.h"

#include "cstdarg"
#include "functional"
#include "string"

#include "app_threadx.h"

using namespace std;

#include "arm_math.h"
#include "spi.h"
#include "mavlink.h"
#include "om.h"
#include "libspi-i-hal-1.0.hpp"
using namespace SPI;
/*TraceX utilities*/
#define TRC_BUF_SIZE (500 * 256) /* Buffer size */
#define TRC_MAX_OBJ_COUNT (40)   /* Max number of ThreadX objects */
static UCHAR TraceXBuf[TRC_BUF_SIZE];
UINT TraceStatus = 0;

/*UART pool*/
extern TX_BYTE_POOL ComPool;

// extern MSGTube::cMSGTube* msgTube;



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

typedef struct {
    uint32_t num;
} om_com_test_t;

TX_SEMAPHORE topic_test;

om_status_t call_function(om_msg_t* msg, void* arg){
    tx_semaphore_put(&topic_test);
    return OM_OK;
}

[[noreturn]] void HeartBeatThreadFun(ULONG initial_input) {
    tx_thread_sleep(100);
    LL_TIM_EnableAllOutputs(TIM5);
    LL_TIM_CC_EnableChannel(TIM5, LL_TIM_CHANNEL_CH1);
    LL_TIM_CC_EnableChannel(TIM5, LL_TIM_CHANNEL_CH2);
    LL_TIM_CC_EnableChannel(TIM5, LL_TIM_CHANNEL_CH3);
    LL_TIM_EnableCounter(TIM5);

    tx_semaphore_create(
    &topic_test,
    (CHAR*)"topic_test",
    0
    );

    om_suber_t* sub = om_config_suber(NULL,"dt",call_function,NULL,om_find_topic("TEST", UINT32_MAX));

    uint32_t num = 0;
    uint32_t num_last = 0;
    uint32_t dc = 0;

    for (;;) {
        /* 解析接收到的数据 */
        tx_semaphore_get(&topic_test,TX_WAIT_FOREVER);
        dc = dc==1999?0:1999;
        LL_TIM_OC_SetCompareCH1(TIM5, dc);
        LL_TIM_OC_SetCompareCH2(TIM5, dc);
        LL_TIM_OC_SetCompareCH3(TIM5, dc);
    }
}

// UARTLL::cUART* pUARTDebug;
// void DebugThreadFun(ULONG initial_input)
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
// }

ULONG spi2_time = 0;
uint8_t *pspi_tx_data;
uint8_t *pspi_rx_data;
uint8_t spi_tx_data_buf[48];
uint8_t spi_rx_data_buf[48];

[[noreturn]] void Test01ThreadFun(ULONG initial_input) {
    for (uint32_t i = 0; i < 24; i++) {
        spi_tx_data_buf[i] = i;
        spi_tx_data_buf[i + 24] = i;
    }
    tx_thread_sleep(10);
    /* SPI2-CS NVIC enable */
    pspi_tx_data = spi_tx_data_buf;
    pspi_rx_data = spi_rx_data_buf;
    NVIC_SetPriority(EXTI15_10_IRQn,
                     NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(EXTI15_10_IRQn);

    for (;;) {
        if (HAL_SPI_GetState(&hspi2) == HAL_SPI_STATE_READY) {
            HAL_SPI_TransmitReceive_DMA(&hspi2, pspi_tx_data, pspi_rx_data, 24);
        } else if (tx_time_get() - spi2_time > 20) {
            HAL_SPI_Abort(&hspi2);
            spi2_time = tx_time_get();
        }
        tx_thread_sleep(1);
    }
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi == &hspi2) {
        spi2_time = tx_time_get();
    }
}

[[noreturn]] void Test02ThreadFun(ULONG initial_input) {
    /* 创建话题 */
    om_topic_t *recv_topic = om_config_topic(NULL, "ca", "TEST" ,24);
    /* 创建发送数据 */
    uint32_t i = 1;
    while (true) {
        om_publish(recv_topic, &i, sizeof(i), true, false);
        i++;
        tx_thread_sleep(500);
    }
}

[[noreturn]] void Test03ThreadFun(ULONG initial_input) {
    tx_thread_sleep(10);
    for (;;) {

    }
}

