#ifndef __STEERING_H
#define __STEERING_H

#include "sys.h"

//舵机定义
#define STEERING_GPIO_CLK       RCC_APB2Periph_GPIOA
#define STEERING_GPIO_PIN_1     GPIO_Pin_8
#define STEERING_GPIO_PIN_4     GPIO_Pin_11
#define STEERING_PORT           GPIOA

#define STEERING_TIMER_CLK      RCC_APB2Periph_TIM1
#define STEERING_TIMER          TIM1
#define STEERING_TIMER_PSC      0
#define STEERING_CHANNEL_CLIP   (STEERING_TIMER)->CCR1      //抓夹舵机引脚
#define STEERING_CHANNEL_STUFF  (STEERING_TIMER)->CCR4      //物料转盘舵机引脚

//舵机控制参数
#define STEERING_ARR            2499
#define STEERING_PSC            71              //PWM = 36,000,000 /(2499+1)/(71+1) = 200 HZ
#define CLIP_INIT               1100            //抓夹舵机初始位置
#define STUFF_INIT              1485            //物料盘舵机初始位置

typedef enum
{
    Grasp,          //抓紧
    Release,        //释放
    Avoid,          //躲避
    MAX,            //张开最大
    MIN,            //张开最小
}OrderClip; 

void Steering_Init(void);                               //舵机初始化
void ArmClipTakeState(OrderClip ClipState,bool Wait);   //抓夹抓取状态
void MaterialPosition(ColorSele color,bool Wait) ;      //物料盘选择颜色位置

#endif 
