#ifndef __LED_H
#define __LED_H

#include "sys.h"

//板载LED_1定义（输出高-灭）
#define BoardLed_GPIO_CLK_1     RCC_APB2Periph_GPIOC
#define BoardLed_GPIO_PIN_1     GPIO_Pin_0
#define BoardLed_PORT_1         GPIOC

#define BoardLed_OFF_1          do{ BoardLed_PORT_1->BSRR = BoardLed_GPIO_PIN_1; }while(0)
#define BoardLed_ON_1           do{ BoardLed_PORT_1->BRR  = BoardLed_GPIO_PIN_1; }while(0)
#define BoardLed_TOGGLE_1       do{ BoardLed_PORT_1->ODR ^= BoardLed_GPIO_PIN_1; }while(0)

//板载LED_2定义（输出高-灭）
#define BoardLed_GPIO_CLK_2     RCC_APB2Periph_GPIOC
#define BoardLed_GPIO_PIN_2     GPIO_Pin_1
#define BoardLed_PORT_2         GPIOC

#define BoardLed_OFF_2          do{ BoardLed_PORT_2->BSRR = BoardLed_GPIO_PIN_2; }while(0)
#define BoardLed_ON_2           do{ BoardLed_PORT_2->BRR  = BoardLed_GPIO_PIN_2; }while(0)
#define BoardLed_TOGGLE_2       do{ BoardLed_PORT_2->ODR ^= BoardLed_GPIO_PIN_2; }while(0)

//非板载LED_1定义（输出高-亮）
#define NonboardLed_GPIO_CLK_1  RCC_APB2Periph_GPIOB
#define NonboardLed_GPIO_PIN_1  GPIO_Pin_8
#define NonboardLed_PORT_1      GPIOB

#define NonboardLed_OFF_1       do{ NonboardLed_PORT_1->BSRR = NonboardLed_GPIO_PIN_1; }while(0)
#define NonboardLed_ON_1        do{ NonboardLed_PORT_1->BRR  = NonboardLed_GPIO_PIN_1; }while(0)
#define NonboardLed_TOGGLE_1    do{ NonboardLed_PORT_1->ODR ^= NonboardLed_GPIO_PIN_1; }while(0)

//非板载LED_2定义（输出高-亮）
#define NonboardLed_GPIO_CLK_2  RCC_APB2Periph_GPIOB
#define NonboardLed_GPIO_PIN_2  GPIO_Pin_9
#define NonboardLed_PORT_2      GPIOB

#define NonboardLed_OFF_2       do{ NonboardLed_PORT_2->BSRR = NonboardLed_GPIO_PIN_2; }while(0)
#define NonboardLed_ON_2        do{ NonboardLed_PORT_2->BRR  = NonboardLed_GPIO_PIN_2; }while(0)
#define NonboardLed_TOGGLE_2    do{ NonboardLed_PORT_2->ODR ^= NonboardLed_GPIO_PIN_2; }while(0)

//激光定义（输出高-亮）
#define LaserLed_GPIO_CLK       RCC_APB2Periph_GPIOB
#define LaserLed_GPIO_PIN       GPIO_Pin_2
#define LaserLed_PORT           GPIOB

#define LaserLed_OFF            do{ LaserLed_PORT->BSRR = LaserLed_GPIO_PIN; }while(0)
#define LaserLed_ON             do{ LaserLed_PORT->BRR  = LaserLed_GPIO_PIN; }while(0)
#define LaserLed_TOGGLE         do{ LaserLed_PORT->ODR ^= LaserLed_GPIO_PIN; }while(0)


void BoardLed_Init(void);       //初始化板载LED引脚
void NonboardLED_Init(void);    //初始化外部LED引脚
void LaserLed_Init(void);       //初始化激光引脚

#endif
