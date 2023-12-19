#ifndef DL_F407_H
#define DL_F407_H
#ifdef __cplusplus
extern "C" {
#endif
/*
	使用自定分散加载文件时
	不指定: 使用普通FLASH
	特殊指定: 使用CCM-FALSH
*/
#define SRAM_SET_CCM      __attribute__((section(".ccmram")))

#ifdef __cplusplus
}
#endif
#endif