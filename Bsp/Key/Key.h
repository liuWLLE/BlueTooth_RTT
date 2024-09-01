#ifndef __KEY_H
#define __KEY_H

#include "sys.h"

//板载按键定义
#define KEY_GPIO_CLK        RCC_APB2Periph_GPIOC
#define KEY_GPIO_PIN        GPIO_Pin_2
#define KEY_PORT            GPIOC

//按键检测结果定义
#define KEY_PRESS           Bit_RESET
#define KEY_LOOSE           Bit_SET

//按键控制定义
#define KEY_TIME_RATIO      10                  //扫描时间系数（频率的倒数）
#define KEY_LOOSE_COUNT     10                  //按键释放多少次认为完全松开
#define KEY_SHAKE_TIME      20                  //消抖时间（ms）
#define KEY_SHORT_TIME      350                 //短按判定时间（ms）
#define KEY_LONG_TIME       900                 //长按松开判定时间（ms）
#define KEY_PERSIST_TIME    1500                //持续长按判定时间（ms）

//按键状态值
typedef enum
{
    KeyRelease,                                 //按键释放 0
    KeyShort,                                   //按键短按 1
    KeyLong,                                    //按键长按松开 2
    KeyPersist,                                 //按键持续按下 3
}KeyState;

void KeyInit(void);                             //按键IO口初始化
u8 KeyScanSingle(bool mode);                    //按键扫描（单次）
u8 KeyScanFreq(void);                           //按键扫描（频率）
void KeyDeviceRunning(void);                    //按键检测运行
void AddKeyCallBack(void (*KeyAppFnc)(u8));     //回调函数注册

#endif
