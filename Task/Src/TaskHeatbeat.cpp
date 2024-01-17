/*
 * @Description: 
 * @Author: qianwan
 * @Date: 2023-12-17 23:50:03
 * @LastEditTime: 2024-01-18 03:16:08
 * @LastEditors: qianwan
 */
#include "main.h"
#include "DL_F407.h"
#include "app_threadx.h"

/*TraceX utilities*/
#define TRC_BUF_SIZE (2048 * 32) /* Buffer size */
#define TRC_MAX_OBJ_COUNT (40)   /* Max number of ThreadX objects */
UCHAR TraceXBuf[TRC_BUF_SIZE];

SRAM_SET_CCM TX_THREAD HeartBeatThread;
SRAM_SET_CCM uint8_t HeartBeatThreadStack[256] = {0};

void HeartBeatThreadFun(ULONG initial_input) {
    UNUSED(initial_input);
    tx_thread_sleep(100);
    LL_TIM_EnableAllOutputs(TIM5);
    LL_TIM_CC_EnableChannel(TIM5, LL_TIM_CHANNEL_CH1);
    LL_TIM_CC_EnableChannel(TIM5, LL_TIM_CHANNEL_CH2);
    LL_TIM_CC_EnableChannel(TIM5, LL_TIM_CHANNEL_CH3);
    LL_TIM_EnableCounter(TIM5);

    uint32_t dc = 0;

    for (;;) {
        /* 解析接收到的数据 */
        tx_thread_sleep(500);
        dc = dc == 0 ? 499 : 0;

        LL_TIM_OC_SetCompareCH1(TIM5, dc);
        LL_TIM_OC_SetCompareCH2(TIM5, dc);
        LL_TIM_OC_SetCompareCH3(TIM5, dc);

//        tx_trace_enable(&TraceXBuf, TRC_BUF_SIZE, TRC_MAX_OBJ_COUNT);
    }
}
