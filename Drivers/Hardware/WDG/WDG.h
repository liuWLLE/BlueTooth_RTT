#ifndef __WDG_H
#define __WDG_H

#include "sys.h"

void IWDG_Init(u8 prer,u16 rlr);    //��ʼ���������Ź�
void IWDG_Feed(void);               //ι�������Ź�

#endif
