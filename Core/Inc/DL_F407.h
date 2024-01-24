#ifndef DL_F407_H
#define DL_F407_H
#ifdef __cplusplus
extern "C" {
#endif
/*
	使用自定分散加载文件时
	不指定: 使用普通RAM
	SRAM_SET_CCM: 使用CCM-RAM 并保存bss等字段进入flash
    SRAM_SET_CCM_UNINT: 使用CCM-RAM 但不保存bss等字段进入flash
*/
#define SRAM_SET_CCM        __attribute__((section(".ccmram")))
#define SRAM_SET_CCM_UNINT  __attribute__((section(".ccmram_uninit")))
#ifdef __cplusplus
}
#endif
#endif