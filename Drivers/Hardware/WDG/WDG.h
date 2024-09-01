#ifndef __WDG_H
#define __WDG_H

#include "sys.h"

void IWDG_Init(u8 prer,u16 rlr);    //初始化独立看门狗
void IWDG_Feed(void);               //喂独立看门狗

#endif
