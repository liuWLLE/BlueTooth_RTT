#ifndef __DELAY_H
#define __DELAY_H 

#include "sys.h"  

#define OS_RTTHREAD_USED        1   //�Ƿ�ʹ��RT-Thread

void Delay_init(u8 SysClk);         //��ʼ���ӳٺ���
void Delay_us(u32 nus);             //��ʱnus����
void DelayOS_ms(u16 nms);           //OS��ʱnms����
void Delay_ms(u16 nms);             //��ʱnms����

#endif

