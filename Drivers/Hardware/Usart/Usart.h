#ifndef __USART_H
#define __USART_H

#include "sys.h" 

//串口发送与接收缓存字节
#define U1RecLen             16
#define U1SenLen             16
#define U2RecLen             5
#define U2SenLen             8
#define U3RecLen             64
#define U3SenLen             16
#define U4RecLen             10
#define U4SenLen             32
#define U5RecLen             16
#define U5SenLen             16

// 串口1 - USART1
#define USART_CLK_1                     RCC_APB2Periph_USART1
#define USART_GPIO_CLK_1                RCC_APB2Periph_GPIOA
#define USART_TX_PORT_1                 GPIOA
#define USART_RX_PORT_1                 GPIOA
#define USART_TX_GPIO_PIN_1             GPIO_Pin_9
#define USART_RX_GPIO_PIN_1             GPIO_Pin_10

#define USART_IT_1_EN
#if defined(USART_IT_1_EN)
    #define USART_IT_PRI_PRE_1              0
    #define USART_IT_PRI_SUB_1              0
#endif

#define USART_RECE_DMA_1_EN
#if defined(USART_RECE_DMA_1_EN)
    #define USART_RECE_DMA_CLK_1                RCC_AHBPeriph_DMA1
    #define USART_RECE_DMA_CHANNEL_1            DMA1_Channel5

    //#define USART_RECE_DMA_IT_1_EN
    #if defined(USART_RECE_DMA_IT_1_EN)
        #define USART_RECE_DMA_IT_1                 DMA1_Channel5_IRQn
        #define USART_RECE_DMA_IT_PRI_PRE_1         0
        #define USART_RECE_DMA_IT_PRI_SUB_1         0
        #define USART_SEND_DMA_IT_FUNC_1            DMA1_Channel5_IRQHandler
        #define USART_SEND_DMA_IT_FLAG_1            DMA1_IT_TC5
    #endif
#endif

#define USART_SEND_DMA_F_1_EN
#if defined(USART_SEND_DMA_F_1_EN)
    #define USART_SEND_DMA_CLK_1                RCC_AHBPeriph_DMA1
    #define USART_SEND_DMA_CHANNEL_1            DMA1_Channel4
    
    #define USART_SEND_DMA_IT_1_EN
    #if defined(USART_SEND_DMA_IT_1_EN)
        #define USART_SEND_DMA_IT_1                 DMA1_Channel4_IRQn
        #define USART_SEND_DMA_IT_PRI_PRE_1         0
        #define USART_SEND_DMA_IT_PRI_SUB_1         0       
        #define USART_SEND_DMA_IT_FUNC_1            DMA1_Channel4_IRQHandler
        #define USART_SEND_DMA_IT_FLAG_1            DMA1_IT_TC4
    #endif
#endif

// 串口2 - USART2
#define USART_CLK_2                     RCC_APB1Periph_USART2
#define USART_GPIO_CLK_2                RCC_APB2Periph_GPIOA
#define USART_TX_PORT_2                 GPIOA
#define USART_RX_PORT_2                 GPIOA
#define USART_TX_GPIO_PIN_2             GPIO_Pin_2
#define USART_RX_GPIO_PIN_2             GPIO_Pin_3

#define USART_IT_2_EN
#if defined(USART_IT_2_EN)
    #define USART_IT_PRI_PRE_2              0
    #define USART_IT_PRI_SUB_2              0
#endif

#define USART_RECE_DMA_2_EN
#if defined(USART_RECE_DMA_2_EN)
    #define USART_RECE_DMA_CLK_2                RCC_AHBPeriph_DMA1
    #define USART_RECE_DMA_CHANNEL_2            DMA1_Channel6
    
    //#define USART_RECE_DMA_IT_2_EN
    #if defined(USART_RECE_DMA_IT_2_EN)
        #define USART_RECE_DMA_IT_2                 DMA1_Channel6_IRQn
        #define USART_RECE_DMA_IT_PRI_PRE_2         0
        #define USART_RECE_DMA_IT_PRI_SUB_2         0
        #define USART_RECE_DMA_IT_FUNC_2            DMA1_Channel6_IRQHandler
        #define USART_RECE_DMA_IT_FLAG_2            DMA1_IT_TC6
    #endif
#endif

//#define USART_SEND_DMA_2_EN
#if defined(USART_SEND_DMA_2_EN)
    #define USART_SEND_DMA_CLK_2                RCC_AHBPeriph_DMA1
    #define USART_SEND_DMA_CHANNEL_2            DMA1_Channel7

    //#define USART_SEND_DMA_IT_2_EN
    #if defined(USART_SEND_DMA_IT_2_EN)
        #define USART_SEND_DMA_IT_2                 DMA1_Channel7_IRQn
        #define USART_SEND_DMA_IT_PRI_PRE_2         0
        #define USART_SEND_DMA_IT_PRI_SUB_2         0
        #define USART_SEND_DMA_IT_FUNC_2            DMA1_Channel7_IRQHandler
        #define USART_SEND_DMA_IT_FLAG_2            DMA1_IT_TC7
    #endif
#endif

// 串口3 - USART3
#define USART_CLK_3                     RCC_APB1Periph_USART3
#define USART_GPIO_CLK_3                RCC_APB2Periph_GPIOB
#define USART_TX_PORT_3                 GPIOB
#define USART_RX_PORT_3                 GPIOB
#define USART_TX_GPIO_PIN_3             GPIO_Pin_10
#define USART_RX_GPIO_PIN_3             GPIO_Pin_11

#define USART_IT_3_EN
#if defined(USART_IT_3_EN)
    #define USART_IT_PRI_PRE_3              0
    #define USART_IT_PRI_SUB_3              0
#endif

//#define USART_RECE_DMA_3_EN
#if defined(USART_RECE_DMA_3_EN)
    #define USART_RECE_DMA_CLK_3                RCC_AHBPeriph_DMA1
    #define USART_RECE_DMA_CHANNEL_3            DMA1_Channel3

    #define USART_RECE_DMA_IT_3_EN
    #if defined(USART_RECE_DMA_IT_3_EN)
        #define USART_RECE_DMA_IT_3                 DMA1_Channel3_IRQn
        #define USART_RECE_DMA_IT_PRI_PRE_3         0
        #define USART_RECE_DMA_IT_PRI_SUB_3         0
        #define USART_RECE_DMA_IT_FUNC_3            DMA1_Channel3_IRQHandler
        #define USART_RECE_DMA_IT_FLAG_3            DMA1_IT_TC3
    #endif
#endif

//#define USART_SEND_DMA_3_EN
#if defined(USART_SEND_DMA_3_EN)
    #define USART_SEND_DMA_CLK_3                RCC_AHBPeriph_DMA1
    #define USART_SEND_DMA_CHANNEL_3            DMA1_Channel2

    #define USART_SEND_DMA_IT_3_EN
    #if defined(USART_SEND_DMA_IT_3_EN)
        #define USART_SEND_DMA_IT_3                 DMA1_Channel2_IRQn
        #define USART_SEND_DMA_IT_PRI_PRE_3         0
        #define USART_SEND_DMA_IT_PRI_SUB_3         0
        #define USART_SEND_DMA_IT_FUNC_3            DMA1_Channel2_IRQHandler
        #define USART_SEND_DMA_IT_FLAG_3            DMA1_IT_TC2
    #endif
#endif

// 串口4 - UART4
#define UART_CLK_4                      RCC_APB1Periph_UART4
#define UART_GPIO_CLK_4                 RCC_APB2Periph_GPIOC
#define UART_TX_PORT_4                  GPIOC
#define UART_TX_PORT_4                  GPIOC
#define UART_TX_GPIO_PIN_4              GPIO_Pin_10
#define UART_RX_GPIO_PIN_4              GPIO_Pin_11

#define UART_IT_4_EN
#if defined(UART_IT_4_EN)
    #define UART_IT_PRI_PRE_4               0
    #define UART_IT_PRI_SUB_4               0
#endif

#define UART_RECE_DMA_4_EN
#if defined(UART_RECE_DMA_4_EN)
    #define UART_RECE_DMA_CLK_4             RCC_AHBPeriph_DMA2
    #define UART_RECE_DMA_CHANNEL_4         DMA2_Channel3

    //#define UART_RECE_DMA_IT_4_EN
    #if defined(UART_RECE_DMA_IT_4_EN)
        #define UART_RECE_DMA_IT_4              DMA2_Channel3_IRQn
        #define UART_RECE_DMA_IT_PRI_PRE_4      0
        #define UART_RECE_DMA_IT_PRI_SUB_4      0
        #define USART_RECE_DMA_IT_FUNC_4        DMA2_Channel3_IRQHandler
        #define USART_RECE_DMA_IT_FLAG_4        DMA2_IT_TC3
    #endif
#endif

#define UART_SEND_DMA_4_EN
#if defined(UART_SEND_DMA_4_EN)
    #define UART_SEND_DMA_CLK_4             RCC_AHBPeriph_DMA2
    #define UART_SEND_DMA_CHANNEL_4         DMA2_Channel5

    #define UART_SEND_DMA_IT_4_EN
    #if defined(UART_SEND_DMA_IT_4_EN)
        #define UART_SEND_DMA_IT_4              DMA2_Channel4_5_IRQn
        #define UART_SEND_DMA_IT_PRI_PRE_4      0
        #define UART_SEND_DMA_IT_PRI_SUB_4      0
        #define USART_SEND_DMA_IT_FUNC_4        DMA2_Channel4_5_IRQHandler
        #define USART_SEND_DMA_IT_FLAG_4        DMA2_IT_TC5
    #endif
#endif

// 串口5 - UART5
#define UART_CLK_5                      RCC_APB1Periph_UART5
#define UART_GPIO_CLK_TX_5              RCC_APB2Periph_GPIOC
#define UART_GPIO_CLK_RX_5              RCC_APB2Periph_GPIOD
#define UART_TX_PORT_5                  GPIOC
#define UART_RX_PORT_5                  GPIOD
#define UART_TX_GPIO_PIN_5              GPIO_Pin_12
#define UART_RX_GPIO_PIN_5              GPIO_Pin_2

#define UART_IT_5_EN
#if defined(UART_IT_5_EN)
    #define UART_IT_PRI_PRE_5               0
    #define UART_IT_PRI_SUB_5               0
#endif

//选择Printf要打印输出的串口
#define USART_Printf            UART4

// 串口控制结构体
typedef struct
{
    volatile char *StrAddr;         //暂存用户字符串地址
    volatile bool State;            //接收数据标志位   
    volatile u16 RxCnt;             //接收的字节数
    volatile u16 TxCnt;             //发送的字节数
}Usart_Control;
extern Usart_Control U1Para;
extern Usart_Control U2Para;
extern Usart_Control U3Para;
extern Usart_Control U4Para;
extern Usart_Control U5Para;

// 串口1 - USART1
typedef struct
{
    volatile char RecBuf[U1RecLen];     //接收缓冲,最大U1RecLen个字节
    volatile char SenBuf[U1SenLen];     //发送缓冲,最大U1SenLen个字节
}Usart1_Buffer;
extern Usart1_Buffer U1Buf;

// 串口2 - USART2
typedef struct
{
    volatile char RecBuf[U2RecLen];     //接收缓冲,最大U2RecLen个字节
    volatile char SenBuf[U2SenLen];     //发送缓冲,最大U2SenLen个字节
    volatile char ShowBuf[U2RecLen];    //显示缓冲,最大U2RecLen个字节
}Usart2_Buffer;
extern Usart2_Buffer U2Buf;

// 串口3 - USART3
typedef struct
{
    volatile char RecBuf[U3RecLen];     //接收缓冲,最大U3RecLen个字节
    volatile char SenBuf[U3SenLen];     //发送缓冲,最大U3SenLen个字节
    volatile char ShowBuf[U3RecLen];    //显示缓冲,最大U3RecLen个字节
}Usart3_Buffer;
extern Usart3_Buffer U3Buf;

// 串口4 - UART4
typedef struct
{
    volatile char RecBuf[U4RecLen];     //接收缓冲,最大U4RecLen个字节
    volatile char SenBuf[U4SenLen];     //发送缓冲,最大U4SenLen个字节
    volatile char ShowBuf[U4SenLen];    //显示缓冲,最大U4RecLen个字节
}Uart4_Buffer;
extern Uart4_Buffer U4Buf;

// 串口5 - UART5
typedef struct
{
    volatile char RecBuf[U5RecLen];     //接收缓冲,最大U5RecLen个字节
    volatile char SenBuf[U5SenLen];     //发送缓冲,最大U5SenLen个字节
}Uart5_Buffer;
extern Uart5_Buffer U5Buf;

void USART_Config_1(u32 bound);     //串口1配置
void USART_Config_2(u32 bound);     //串口2配置
void USART_Config_3(u32 bound);     //串口3配置
void UART_Config_4(u32 bound);      //串口4配置
void UART_Config_5(u32 bound);      //串口5配置

void DMA_Transfer(DMA_Channel_TypeDef* DMA_CHx,u16 buff);                           //开启一次DMA传输
u8 Usart_ReceByte(USART_TypeDef *pUSARTx);                                          //接收一个字节（返回字节）
bool Usart_ReceByte_P(USART_TypeDef *pUSARTx,u8 *byte);                             //接收一个字节（指针保存）
void Usart_SendByte(USART_TypeDef *pUSARTx,char byte,u8 SendCount,u16 gap);         //发送一个字节
void Usart_SendString(USART_TypeDef *pUSARTx,char *str,u8 SendCount,u16 gap);       //发送一串字符（阻塞）
void Usart_SendStrFix(USART_TypeDef *pUSARTx,u8 *buf, u8 len,u8 SendCount,u16 gap); //发送一串字符（定长）
void Usart_SendStrIT(USART_TypeDef* pUSARTx,char *str,u16 count);                   //发送一串字符（中断）

#endif
