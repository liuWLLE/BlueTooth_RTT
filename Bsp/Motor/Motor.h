#ifndef __MOTOR_H
#define __MOTOR_H

#include "sys.h"

//电机定义
#define MOTOR_OFFLINE_GPIO_CLK          RCC_APB2Periph_GPIOA
#define MOTOR_OFFLINE_GPIO_PIN_1        GPIO_Pin_4
#define MOTOR_OFFLINE_GPIO_PIN_2        GPIO_Pin_5
#define MOTOR_OFFLINE_GPIO_PIN_3        GPIO_Pin_6
#define MOTOR_OFFLINE_GPIO_PIN_4        GPIO_Pin_7
#define MOTOR_OFFLINE_PORT              GPIOA
#define MOTOR_OFFLINE_1                 PAout(4)  
#define MOTOR_OFFLINE_2                 PAout(5)
#define MOTOR_OFFLINE_3                 PAout(6)
#define MOTOR_OFFLINE_4                 PAout(7)

#define MOTOR_DIRECT_GPIO_CLK           RCC_APB2Periph_GPIOB
#define MOTOR_DIRECT_GPIO_PIN_1         GPIO_Pin_12
#define MOTOR_DIRECT_GPIO_PIN_2         GPIO_Pin_13
#define MOTOR_DIRECT_GPIO_PIN_3         GPIO_Pin_0
#define MOTOR_DIRECT_GPIO_PIN_4         GPIO_Pin_15
#define MOTOR_DIRECT_PORT               GPIOB
#define MOTOR_DIRECT_OUT_1              PBout(12)
#define MOTOR_DIRECT_OUT_2              PBout(13)
#define MOTOR_DIRECT_OUT_3              PBout(0)
#define MOTOR_DIRECT_OUT_4              PBout(15)

#define MOTOR_PULSE_GPIO_CLK            RCC_APB2Periph_GPIOC
#define MOTOR_PULSE_GPIO_PIN_1          GPIO_Pin_6
#define MOTOR_PULSE_GPIO_PIN_2          GPIO_Pin_7
#define MOTOR_PULSE_GPIO_PIN_3          GPIO_Pin_8
#define MOTOR_PULSE_GPIO_PIN_4          GPIO_Pin_9
#define MOTOR_PULSE_PORT                GPIOC

#define MOTOR_TIMER_CLK                 RCC_APB2Periph_TIM8
#define MOTOR_TIMER                     TIM8
#define MOTOR_TIMER_ARR                 65534
#define MOTOR_TIMER_PSC                 71
#define MOTOR_TIMER_ARR_REG             (MOTOR_TIMER)->ARR
#define MOTOR_TIMER_PSC_REG             (MOTOR_TIMER)->PSC
#define MOTOR_TIMER_CHANNEL_1           (MOTOR_TIMER)->CCR1
#define MOTOR_TIMER_CHANNEL_2           (MOTOR_TIMER)->CCR2
#define MOTOR_TIMER_CHANNEL_3           (MOTOR_TIMER)->CCR3
#define MOTOR_TIMER_CHANNEL_4           (MOTOR_TIMER)->CCR4

#define MOTOR_TIMER_INTERRUPT           TIM8_UP_IRQn
#define MOTOR_TIMER_IT_FUNC             TIM8_UP_IRQHandler
#define MOTOR_TIMER_IT_FLAG             (MOTOR_TIMER)
#define MOTOR_TIMER_IT_PRI_PRE          0
#define MOTOR_TIMER_IT_PRI_SUB          2

//电机控制参数
#define MOTOR_SINGLE_PULSE              1
#define MOTOR_SPEED                     65435       //升降电机速度（0-65535 越大速度越大）

//matlab模拟参数
#define matlab_xz_pcr                   2           //分频值
#define matlab_sj_pcr                   2           //分频值
#define matlab_Subdivision              8           //电机细分系数

//电机ID
#define MOTOR_ID_1                      1
#define MOTOR_ID_2                      2
#define MOTOR_ID_3                      3
#define MOTOR_ID_4                      4

//电机运动位置
#define ZeroHeight                      1950
#define LowestHeight                    300
#define MaxAngle                        950
#define Division                        1.6842      //((ZeroHeight-LowestHeight)/MaxAngle)

//旋转电机位置定义
typedef enum
{
    Reset_R,                //复位
    Vision                  //识别
}Rotate;

//升降电机位置定义
typedef enum
{
    Reset_L,                //复位
    Grab,                   //降抓取
    Stack,                  //降码垛
    RawArea,                //降原料转盘
    Stuff,                  //降车上托盘
    Aim,                    //降校准
    Rise,                   //上升
    Fall,                   //下降
    Lowest,                 //最低
    Raw,                    //原料区识别
    Places,                 //降放置
    Pile                    //降码垛校准
}Lifting;

typedef struct
{
    float angle;            // 角度
    float axis;             // 坐标（单位: mm）
}motor_quality;

typedef struct  
{
    motor_quality id[5];    // 4个电机的属性，0号不用 
    long pulse_num;         // 每次任务装载的总脉冲数
    u16 Subdivision;        // 电机细分系数
    u16 step_max;           // 单圈脉冲数,最大为 200 * Subdivision / single_pause_NUM
    char sign_1_ok_step;    // 设定步数完成标志
}motor_set;
extern motor_set motor;

void Motor_Init(void);                                      //电机初始化
void MotorAngleCtrl_S(float target_angle,char id);          //角度控制（S加减速）
void MotorAngleCtrl(u16 speed,float target_angle,char id);  //角度控制

u32 MotorMotion_Lift(Lifting Pos,bool Wait);                //升降电机位置选择
void MotorMotion_Rotate(Rotate Pos,bool Wait);              //旋转电机运动

#endif 
