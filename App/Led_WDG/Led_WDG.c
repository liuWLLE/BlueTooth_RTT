#include "Led_WDG.h"

// Hardware
#include "WDG.h"

// Bsp
#include "Led.h" 

/**************************************************************************
函数功能：LED与WDG定时器回调函数
入口参数：无
返回  值：无
注意事项：无
**************************************************************************/
void Led_WDG_callback(void* parameter)
{
    //LED翻转
    BoardLed_TOGGLE_1;
    BoardLed_TOGGLE_2;
    //喂狗
    IWDG_Feed();
}

