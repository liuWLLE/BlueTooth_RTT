#include "Led.h"

/**************************************************************************
函数功能：初始化板载LED引脚
入口参数: 无
返回  值：无
**************************************************************************/
void BoardLed_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(BoardLed_GPIO_CLK_1,ENABLE);     //使能时钟
    RCC_APB2PeriphClockCmd(BoardLed_GPIO_CLK_2,ENABLE);     //使能时钟
    
    GPIO_InitStructure.GPIO_Pin = BoardLed_GPIO_PIN_1;      //端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;       //10MHz
    GPIO_Init(BoardLed_PORT_1,&GPIO_InitStructure);
    
    GPIO_SetBits(BoardLed_PORT_1,BoardLed_GPIO_PIN_1);      //输出高-灭

    GPIO_InitStructure.GPIO_Pin = BoardLed_GPIO_PIN_2;      //端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;       //10MHz
    GPIO_Init(BoardLed_PORT_2,&GPIO_InitStructure);

    GPIO_SetBits(BoardLed_PORT_2,BoardLed_GPIO_PIN_2);      //输出高-灭
}
 
/**************************************************************************
函数功能：初始化外部LED引脚
入口参数: 无
返回  值：无
**************************************************************************/
void NonboardLED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(NonboardLed_GPIO_CLK_1,ENABLE);      //使能时钟
    RCC_APB2PeriphClockCmd(NonboardLed_GPIO_CLK_2,ENABLE);      //使能时钟
    
    GPIO_InitStructure.GPIO_Pin = NonboardLed_GPIO_PIN_1;       //端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;            //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;           //10MHz
    GPIO_Init(NonboardLed_PORT_1,&GPIO_InitStructure);
    
    GPIO_ResetBits(NonboardLed_PORT_1,NonboardLed_GPIO_PIN_1);  //输出低-灭

    GPIO_InitStructure.GPIO_Pin = NonboardLed_GPIO_PIN_2;       //端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;            //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;           //10MHz
    GPIO_Init(NonboardLed_PORT_2,&GPIO_InitStructure);
    
    GPIO_ResetBits(NonboardLed_PORT_2,NonboardLed_GPIO_PIN_2);  //输出低-灭
}

/**************************************************************************
函数功能：初始化激光灯引脚
入口参数: 无
返回  值：无
**************************************************************************/
void LaserLed_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(LaserLed_GPIO_CLK,ENABLE);           //使能时钟
  
    GPIO_InitStructure.GPIO_Pin = LaserLed_GPIO_PIN;            //端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;            //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;           //10MHz
    GPIO_Init(LaserLed_PORT,&GPIO_InitStructure);
    
    GPIO_SetBits(LaserLed_PORT,LaserLed_GPIO_PIN);              //输出高-亮
}
