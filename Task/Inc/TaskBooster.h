#ifndef TASKBOOSTER_H
#define TASKBOOSTER_H
#include "main.h"

#ifdef __cplusplus
extern "C" {
void Task_Init(void);
void EXTI15_10_IRQHandler(void);
}
#endif
#endif