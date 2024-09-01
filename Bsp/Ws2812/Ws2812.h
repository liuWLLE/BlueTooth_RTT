#ifndef __WS2812_H
#define __WS2812_H

#include "sys.h" 

//WS2812定义
#define WS2812B_GPIO_CLK        RCC_APB2Periph_GPIOC
#define WS2812B_GPIO_PIN        GPIO_Pin_1
#define WS2812B_PORT            GPIOA
#define WS2812B_HIGH            GPIO_SetBits(WS2812B_PORT,WS2812B_GPIO_PIN)
#define WS2812B_LOW             GPIO_ResetBits(WS2812B_PORT,WS2812B_GPIO_PIN)

#define WS2812B_TIMER_CLK       RCC_APB1Periph_TIM2
#define WS2812B_TIMER           TIM2
#define WS2812B_TIMER_ARR       90
#define WS2812B_TIMER_PSC       0
#define WS2812B_TIMER_CHANNEL   (WS2812B_TIMER)->CCR2
#define WS2812B_TIMER_DMA       TIM_DMA_CC2

#define WS2812B_DMA_CLK         RCC_AHBPeriph_DMA1
#define WS2812B_DMA_CHANNEL     DMA1_Channel7

#define WS2812B_DMA_INTERRUPT   DMA1_Channel7_IRQn
#define WS2812B_DMA_IT_FUNC     DMA1_Channel7_IRQHandler
#define WS2812B_DMA_IT_FLAG     DMA1_IT_TC7
#define WS2812B_DMA_IT_PRI_PRE  0
#define WS2812B_DMA_IT_PRI_SUB  0

//WS2812控制参数
#define WS2812B_T0H             30              //0编码高电平时间占1/3
#define WS2812B_T1H             60              //1编码高电平时间占2/3

//WS2812硬件参数
#define WS2812B_LED_NUM         12              //灯珠亮的个数
#define WS2812B_DATA_BUFF       Ws2812b_buf     //缓存数组
#define WS2812B_DATA_SIZE       24              //单个编码的bit数，3*8

//颜色码定义
typedef enum{
    Red,        //红
    Green,      //绿
    Blue,       //蓝
    White,      //白
    None        //空
}Color_WS2812B;

void WS2812B_Init(void);                                                        //初始化WS2812
void WS2812B_Reset(void);                                                       //复位WS2812
void WS2812B_SetColor(Color_WS2812B Color);                                     //灯环整体设置颜色
void WS2812B_SetColor_N(uint8_t Num,uint8_t red,uint8_t green,uint8_t blue);    //灯环第n位设置颜色

#endif
