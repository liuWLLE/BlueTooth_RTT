#ifndef __DELAY_H
#define __DELAY_H 

#include "sys.h"  

#define OS_RTTHREAD_USED        1   //是否使用RT-Thread

void Delay_init(u8 SysClk);         //初始化延迟函数
void Delay_us(u32 nus);             //延时nus函数
void DelayOS_ms(u16 nms);           //OS延时nms函数
void Delay_ms(u16 nms);             //延时nms函数

#endif

