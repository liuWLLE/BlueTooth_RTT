#include "Steering.h"

/**************************************************************************
函数功能：舵机初始化（定时器）
入口参数：arr：自动重装值、psc：时钟预分频数
返回  值：无
注意事项：CH1--PA8、CH2--PA9、CH3--PA10、CH4--PA11

    //TIM_OC2Init(TIM1,&TIM_OCInitStruct);                          //初始化输出比较参数
    //TIM_OC3Init(TIM1,&TIM_OCInitStruct);                          //初始化输出比较参数
    //TIM_OC2PreloadConfig(TIM1,TIM_OCPreload_Enable);              //CH2使能预装载寄存器
    //TIM_OC3PreloadConfig(TIM1,TIM_OCPreload_Enable);              //CH3使能预装载寄存器
    //TIM1->CCR2 = Stuff_INIT;                                      //物料盘舵机初始位置 
    //TIM1->CCR3 = Stage_INIT;                                      //舵机初始位置 
**************************************************************************/
void Steering_Init(void){
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
    TIM_OCInitTypeDef TIM_OCInitStruct = {0};
    
    RCC_APB2PeriphClockCmd(STEERING_TIMER_CLK,ENABLE);                  //使能定时器时钟
    RCC_APB2PeriphClockCmd(STEERING_GPIO_CLK,ENABLE);                   //使能GPIO的时钟
                    
    GPIO_InitStruct.GPIO_Pin = STEERING_GPIO_PIN_1|STEERING_GPIO_PIN_4;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;                        //复用推挽输出
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;                      //引脚速度
    GPIO_Init(STEERING_PORT,&GPIO_InitStruct);

    TIM_DeInit(STEERING_TIMER);                                         //复位定时器
    TIM_TimeBaseInitStruct.TIM_Period = STEERING_ARR;                   //设定计数器自动重装值 
    TIM_TimeBaseInitStruct.TIM_Prescaler = STEERING_PSC;                //设定预分频器
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;        //TIM向上计数模式
    TIM_TimeBaseInitStruct.TIM_ClockDivision = STEERING_TIMER_PSC;      //设置时钟分割  
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;                   //重复计数(高级定时器特有)    
    TIM_TimeBaseInit(STEERING_TIMER,&TIM_TimeBaseInitStruct);           //初始化定时器
    
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;                      //选择PWM1模式
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;          //指定TIM输出比较状态\极性(正向通道)
    TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable;       //指定TIM输出比较状态\极性(互补通道)
    TIM_OCInitStruct.TIM_Pulse = 0;                                     //指定要加载到捕获比较寄存器中的脉冲值
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;              //指定输出极性
    TIM_OCInitStruct.TIM_OCNPolarity  = TIM_OCNPolarity_High;           //指定互补通道输出极性
    TIM_OCInitStruct.TIM_OCIdleState  = TIM_OCIdleState_Set;            //指定空闲状态下正向通道TIM输出比较引脚的状态
    TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCIdleState_Reset;          //指定空闲状态下互补通道TIM输出比较引脚的状态
    TIM_OC1Init(STEERING_TIMER,&TIM_OCInitStruct);                      //初始化输出比较参数
    TIM_OC4Init(STEERING_TIMER,&TIM_OCInitStruct);                      //初始化输出比较参数
    
    TIM_OC1PreloadConfig(STEERING_TIMER,TIM_OCPreload_Enable);          //CH1使能预装载寄存器
    TIM_OC4PreloadConfig(STEERING_TIMER,TIM_OCPreload_Enable);          //CH4使能预装载寄存器
    
    TIM_CtrlPWMOutputs(STEERING_TIMER,ENABLE);                          //高级定时器输出
    TIM_ARRPreloadConfig(STEERING_TIMER,ENABLE);                        //使能定时器在ARR上的预装载寄存器（默认允许）
    STEERING_CHANNEL_CLIP = CLIP_INIT;                                  //抓夹舵机初始位置 
    STEERING_CHANNEL_STUFF = STUFF_INIT;                                //物料盘舵机初始位置

    TIM_Cmd(STEERING_TIMER,ENABLE);                                     //使能定时器
} 

/**************************************************************************
函数功能：抓夹抓取状态
入口参数：ClipState：物料夹抓取状态、Wait：是否等待
返回  值：无
注意事项：1、寄存器最大值 2200
          2、寄存器最小值 200
**************************************************************************/
void ArmClipTakeState(OrderClip ClipState,bool Wait) 
{
    switch(ClipState){ 
        case Grasp:  STEERING_CHANNEL_CLIP = 1050;break;
        case Release:STEERING_CHANNEL_CLIP = 2050;break;
        case Avoid:  STEERING_CHANNEL_CLIP = 1825;break;
        case MAX:    STEERING_CHANNEL_CLIP = 2200;break;
        case MIN:    STEERING_CHANNEL_CLIP = 200; break;
    }
    if(Wait)
    {
        Delay_ms(450);
    }
}

/**************************************************************************
函数功能：物料盘选择颜色位置
入口参数：color：所需要的颜色、Wait：是否等待
返回  值：无
**************************************************************************/
void MaterialPosition(ColorSele color,bool Wait) 
{
    switch(color){
        case Red_1:  STEERING_CHANNEL_STUFF = 615;break;
        case Green_2:STEERING_CHANNEL_STUFF = STUFF_INIT;break;
        case Blue_3: STEERING_CHANNEL_STUFF = 2415;break;
    }
    if(Wait)
    {
        Delay_ms(700);
    }
} 
