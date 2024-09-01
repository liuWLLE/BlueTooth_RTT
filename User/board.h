#ifndef __BOARD_H__
#define __BOARD_H__

#include "sys.h"
 
void rt_hw_board_init(void);    //开发板硬件初始化函数
void SysTick_Handler(void);     //SysTick 中断服务函数

#endif
