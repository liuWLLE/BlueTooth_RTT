#include "Usart.h"

Usart_Control U1Para = {NULL,false,0,0};
Usart_Control U2Para = {NULL,false,0,0};
Usart_Control U3Para = {NULL,false,0,0};
Usart_Control U4Para = {NULL,false,0,0};
Usart_Control U5Para = {NULL,false,0,0};

Usart1_Buffer U1Buf = {{0},{0}};
Usart2_Buffer U2Buf = {{0},{0},{0}};
Usart3_Buffer U3Buf = {{0},{0},{0}};
Uart4_Buffer U4Buf = {{0},{0},{0}};
Uart5_Buffer U5Buf = {{0},{0}};

/**************************************************************************
函数功能：串口1初始化配置
入口参数：bound：波特率
返回  值：无
**************************************************************************/
void USART_Config_1(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    DMA_InitTypeDef DMA_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    
    RCC_APB2PeriphClockCmd(USART_GPIO_CLK_1,ENABLE);                                        //开启引脚时钟
    RCC_APB2PeriphClockCmd(USART_CLK_1,ENABLE);                                             //开启串口时钟

    GPIO_InitStructure.GPIO_Pin = USART_TX_GPIO_PIN_1;                                      //推挽复用模式
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       
    GPIO_Init(USART_TX_PORT_1, &GPIO_InitStructure);        

    GPIO_InitStructure.GPIO_Pin = USART_RX_GPIO_PIN_1;                                      //浮空输入模式
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USART_RX_PORT_1, &GPIO_InitStructure);

    USART_DeInit(USART1);                                                                   //复位串口    
    USART_InitStructure.USART_BaudRate = bound;                                             //配置波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                             //配置帧数据字长
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                                  //配置停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                                     //配置校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;         //配置硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                         //配置收发一起模式
    USART_Init(USART1, &USART_InitStructure);                                               //完成串口的初始化配置

#if defined(USART_IT_1_EN)                                                                  //串口中断
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;                                       //配置中断源
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART_IT_PRI_PRE_1;              //抢断优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART_IT_PRI_SUB_1;                     //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                         //使能中断
    NVIC_Init(&NVIC_InitStructure);                                                         //初始化配置 NVIC
    
    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);                                            //使能串口接收中断
    USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);                                            //使能串口空闲中断
#endif

#if defined(USART_RECE_DMA_1_EN)                                                            //DMA方式接收数据
    RCC_AHBPeriphClockCmd(USART_RECE_DMA_CLK_1, ENABLE);                                    //开启DMA时钟

    DMA_DeInit(USART_RECE_DMA_CHANNEL_1);                                                   //复位DMA通道
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR;                            //外设基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)U1Buf.RecBuf;                               //内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                                      //方向:从外设接收到内存
    DMA_InitStructure.DMA_BufferSize = U1RecLen;                                            //缓存区长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                        //外设地址不变（外设的寄存器是固定的）
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                                 //内存地址自增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;                 //外设数据单位 8bit
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                         //内存数据单位 8bit
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;                                          //DMA模式：单次
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                                   //优先级：中
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                            //禁止内存到内存的传输
    DMA_Init(USART_RECE_DMA_CHANNEL_1,&DMA_InitStructure);                                  //配置DMA1的通道

    #if defined(USART_RECE_DMA_IT_1_EN)                                                     //DMA接收中断
        NVIC_InitStructure.NVIC_IRQChannel = USART_RECE_DMA_IT_1;                           //配置中断源
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART_RECE_DMA_IT_PRI_PRE_1; //抢断优先级
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART_RECE_DMA_IT_PRI_SUB_1;        //子优先级
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                     //使能中断
        NVIC_Init(&NVIC_InitStructure);                                                     //初始化配置 NVIC
        
        DMA_ITConfig(USART_RECE_DMA_CHANNEL_1,DMA_IT_TC,ENABLE);                            //使能DMA传输完成中断
    #endif
        
    DMA_Cmd(USART_RECE_DMA_CHANNEL_1,ENABLE);                                               //使能DMA
    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);                                            //使能串口DMA接收功能
#endif

#if defined(USART_SEND_DMA_F_1_EN)                                                          //DMA方式发送数据
    RCC_AHBPeriphClockCmd(USART_SEND_DMA_CLK_1,ENABLE);                                     //开启DMA时钟

    DMA_DeInit(USART_SEND_DMA_CHANNEL_1);                                                   //复位DMA通道
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR;                            //外设基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)U1Buf.SenBuf;                               //内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                                      //方向：从内存读取发送到外设
    DMA_InitStructure.DMA_BufferSize = U1SenLen;                                            //缓存区长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                        //外设地址不变（外设的寄存器是固定的）
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                                 //内存地址自增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;                 //外设数据单位 8bit
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                         //内存数据单位 8bit
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;                                          //DMA模式：单次
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                                   //优先级：中
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                            //禁止内存到内存的传输
    DMA_Init(USART_SEND_DMA_CHANNEL_1,&DMA_InitStructure);                                  //配置DMA1的通道

    #if defined(USART_SEND_DMA_IT_1_EN)                                                     //DMA发送中断
        NVIC_InitStructure.NVIC_IRQChannel = USART_SEND_DMA_IT_1;                           //配置中断源
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART_SEND_DMA_IT_PRI_PRE_1; //抢断优先级
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART_SEND_DMA_IT_PRI_SUB_1;        //子优先级
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                     //使能中断
        NVIC_Init(&NVIC_InitStructure);                                                     //初始化配置 NVIC
        
        DMA_ITConfig(USART_SEND_DMA_CHANNEL_1,DMA_IT_TC,ENABLE);                            //使能DMA传输完成中断
    #endif

    DMA_Cmd(USART_SEND_DMA_CHANNEL_1,DISABLE);                                              //失能DMA
    USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);                                            //使能串口DMA发送功能
#endif

    USART_Cmd(USART1, ENABLE);                                                              //使能串口
}

/**************************************************************************
函数功能：串口2初始化配置
入口参数：bound：波特率
返回  值：无
**************************************************************************/
void USART_Config_2(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    DMA_InitTypeDef DMA_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    
    RCC_APB2PeriphClockCmd(USART_GPIO_CLK_2,ENABLE);                                        //打开引脚时钟
    RCC_APB1PeriphClockCmd(USART_CLK_2, ENABLE);                                            //打开串口时钟

    GPIO_InitStructure.GPIO_Pin = USART_TX_GPIO_PIN_2;                                      //推挽复用模式
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART_TX_PORT_2, &GPIO_InitStructure); 

    GPIO_InitStructure.GPIO_Pin = USART_RX_GPIO_PIN_2;                                      //浮空输入模式
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USART_RX_PORT_2, &GPIO_InitStructure);

    USART_DeInit(USART2);                                                                   //复位串口
    USART_InitStructure.USART_BaudRate = bound;                                             //配置波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                             //配置帧数据字长
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                                  //配置停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                                     //配置校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;         //配置硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                         //配置收发一起模式
    USART_Init(USART2, &USART_InitStructure);                                               //完成串口的初始化配置
    
#if defined(USART_IT_2_EN)                                                                  //串口中断
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;                                       //配置中断源
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;                               //抢断优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                                      //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                         //使能中断
    NVIC_Init(&NVIC_InitStructure);                                                         //初始化配置 NVIC
    
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                                          //使能串口接收中断
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);                                          //使能串口空闲中断
#endif


#if defined(USART_RECE_DMA_2_EN)                                                            //DMA方式接收
    RCC_AHBPeriphClockCmd(USART_RECE_DMA_CLK_2, ENABLE);                                    //开启DMA时钟
    
    DMA_DeInit(USART_RECE_DMA_CHANNEL_2);                                                   //复位DMA通道
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;                            //外设基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)U2Buf.RecBuf;                               //内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                                      //方向:从外设接收到内存
    DMA_InitStructure.DMA_BufferSize = U2RecLen;                                            //缓存区长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                        //外设地址不变（外设的寄存器是固定的）
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                                 //内存地址自增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;                 //外设数据单位 8bit
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                         //内存数据单位 8bit
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;                                          //DMA模式：单次
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                                   //优先级：中
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                            //禁止内存到内存的传输
    DMA_Init(USART_RECE_DMA_CHANNEL_2,&DMA_InitStructure);                                  //配置DMA的通道

    #if defined(USART_RECE_DMA_IT_2_EN)                                                     //DMA接收中断
        NVIC_InitStructure.NVIC_IRQChannel = USART_RECE_DMA_IT_2;                           //配置中断源
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART_RECE_DMA_IT_PRI_PRE_2; //抢断优先级
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART_RECE_DMA_IT_PRI_SUB_2;        //子优先级
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                     //使能中断
        NVIC_Init(&NVIC_InitStructure);                                                     //初始化配置 NVIC

        DMA_ITConfig(USART_RECE_DMA_CHANNEL_2,DMA_IT_TC,ENABLE);                            //使能DMA传输完成中断
    #endif

    DMA_Cmd(USART_RECE_DMA_CHANNEL_2,ENABLE);                                               //使能DMA
    USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);                                            //使能串口DMA接收功能
#endif


#if defined(USART_SEND_DMA_2_EN)                                                            //DMA方式发送
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);                                      //开启DMA时钟
    
    DMA_DeInit(USART_SEND_DMA_CHANNEL_2);                                                   //复位DMA通道
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;                            //外设基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)U2Buf.SenBuf;                               //内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                                      //方向：从内存读取发送到外设
    DMA_InitStructure.DMA_BufferSize = U2SenLen;                                            //缓存区长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                        //外设地址不变（外设的寄存器是固定的）
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                                 //内存地址自增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;                 //外设数据单位 8bit
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                         //内存数据单位 8bit
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;                                          //DMA模式：单次
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                                   //优先级：中
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                            //禁止内存到内存的传输
    DMA_Init(USART_SEND_DMA_CHANNEL_2,&DMA_InitStructure);                                  //配置DMA的通道
        
    #if defined(USART_SEND_DMA_IT_2_EN)                                                     //DMA发送中断
        NVIC_InitStructure.NVIC_IRQChannel = USART_SEND_DMA_IT_2;                           //配置中断源
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART_SEND_DMA_IT_PRI_PRE_2; //抢断优先级
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART_SEND_DMA_IT_PRI_SUB_2;        //子优先级
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                     //使能中断
        NVIC_Init(&NVIC_InitStructure);                                                     //初始化配置 NVIC
        
        DMA_ITConfig(USART_SEND_DMA_CHANNEL_2,DMA_IT_TC,ENABLE);                            //使能DMA传输完成中断
    #endif
    
    DMA_Cmd(USART_SEND_DMA_CHANNEL_2,DISABLE);                                              //失能DMA
    USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);                                            //使能串口DMA发送功能
#endif

    USART_Cmd(USART2, ENABLE);                                                              //使能串口
}

/**************************************************************************
函数功能：串口3初始化配置
入口参数：bound：波特率
返回  值：无
**************************************************************************/
void USART_Config_3(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    DMA_InitTypeDef DMA_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(USART_GPIO_CLK_3,ENABLE);                                        //打开引脚时钟
    RCC_APB1PeriphClockCmd(USART_CLK_3,ENABLE);                                             //打开串口时钟

    GPIO_InitStructure.GPIO_Pin = USART_TX_GPIO_PIN_3;                                      //推挽复用模式
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART_TX_PORT_3, &GPIO_InitStructure); 

    GPIO_InitStructure.GPIO_Pin = USART_RX_GPIO_PIN_3;                                      //浮空输入模式
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USART_RX_PORT_3, &GPIO_InitStructure);

    USART_DeInit(USART3);                                                                   //复位串口
    USART_InitStructure.USART_BaudRate = bound;                                             //配置波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                             //配置帧数据字长
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                                  //配置停止位
    USART_InitStructure.USART_Parity = USART_Parity_No ;                                    //配置校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;         //配置硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                         //配置收发一起模式
    USART_Init(USART3, &USART_InitStructure);                                               //完成串口的初始化配置

#if defined(USART_IT_3_EN)                                                                  //串口中断
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;                                       //配置中断源
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART_IT_PRI_PRE_3;              //抢断优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART_IT_PRI_SUB_3;                     //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                         //使能中断
    NVIC_Init(&NVIC_InitStructure);                                                         //初始化配置 NVIC

    USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);                                            //使能串口接收中断
    USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);                                            //使能串口空闲中断
#endif

#if defined(USART_RECE_DMA_3_EN)                                                            //DMA方式接收数据
    RCC_AHBPeriphClockCmd(USART_RECE_DMA_CLK_3,ENABLE);                                     //开启DMA时钟
    
    DMA_DeInit(USART_RECE_DMA_CHANNEL_3);                                                   //复位DMA通道
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART3->DR;                            //外设基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)U3Buf.RecBuf;                               //内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                                      //方向:从外设接收到内存
    DMA_InitStructure.DMA_BufferSize = U3RecLen;                                            //缓存区长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                        //外设地址不变（外设的寄存器是固定的）
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                                 //内存地址自增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;                 //外设数据单位 8bit
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                         //内存数据单位 8bit
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;                                          //DMA模式：单次
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                                   //优先级：中
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                            //禁止内存到内存的传输
    DMA_Init(USART_RECE_DMA_CHANNEL_3,&DMA_InitStructure);                                  //配置DMA的通道

    #if defined(USART_RECE_DMA_IT_3_EN)                                                     //DMA接收中断
        NVIC_InitStructure.NVIC_IRQChannel = USART_RECE_DMA_IT_3;                           //配置中断源
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART_RECE_DMA_IT_PRI_PRE_3; //抢断优先级
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART_RECE_DMA_IT_PRI_SUB_3;        //子优先级
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                     //使能中断
        NVIC_Init(&NVIC_InitStructure);                                                     //初始化配置 NVIC

        DMA_ITConfig(USART_RECE_DMA_CHANNEL_3,DMA_IT_TC,ENABLE);                            //使能DMA传输完成中断
    #endif

        DMA_Cmd(USART_RECE_DMA_CHANNEL_3,ENABLE);                                           //使能DMA
        USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);                                        //使能串口DMA接收功能
#endif

#if defined(USART_SEND_DMA_3_EN)                                                            //DMA方式发送数据
    RCC_AHBPeriphClockCmd(USART_SEND_DMA_CLK_3,ENABLE);                                     //开启DMA时钟
    
    DMA_DeInit(USART_SEND_DMA_CHANNEL_3);                                                   //复位DMA通道
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART3->DR;                            //外设基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)U3Buf.SenBuf;                               //内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                                      //方向：从内存读取发送到外设
    DMA_InitStructure.DMA_BufferSize = U3SenLen;                                            //缓存区长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                        //外设地址不变（外设的寄存器是固定的）
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                                 //内存地址自增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;                 //外设数据单位 8bit
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                         //内存数据单位 8bit
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;                                          //DMA模式：单次
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                                   //优先级：中
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                            //禁止内存到内存的传输
    DMA_Init(USART_SEND_DMA_CHANNEL_3, &DMA_InitStructure);                                 //配置DMA的通道

    #if defined(USART_SEND_DMA_IT_3_EN)                                                     //DMA发送中断
        NVIC_InitStructure.NVIC_IRQChannel = USART_SEND_DMA_IT_3;                           //配置中断源
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART_SEND_DMA_IT_PRI_PRE_3; //抢断优先级
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = USART_SEND_DMA_IT_PRI_SUB_3;        //子优先级
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                     //使能中断
        NVIC_Init(&NVIC_InitStructure);                                                     //初始化配置 NVIC
        
        DMA_ITConfig(USART_SEND_DMA_CHANNEL_3,DMA_IT_TC,ENABLE);                            //使能DMA传输完成中断
     #endif

    DMA_Cmd(USART_SEND_DMA_CHANNEL_3,DISABLE);                                              //失能DMA
    USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);                                            //使能串口DMA发送功能
#endif

    USART_Cmd(USART3,ENABLE);                                                               //使能串口
}

/**************************************************************************
函数功能：串口4初始化配置
入口参数：bound：波特率
返回  值：无
**************************************************************************/
void UART_Config_4(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    DMA_InitTypeDef DMA_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(UART_GPIO_CLK_4,ENABLE);                                         //打开引脚时钟
    RCC_APB1PeriphClockCmd(UART_CLK_4,ENABLE);                                              //打开串口时钟
    
    GPIO_InitStructure.GPIO_Pin = UART_TX_GPIO_PIN_4;                                       //推挽复用模式
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(UART_TX_PORT_4, &GPIO_InitStructure); 

    GPIO_InitStructure.GPIO_Pin = UART_RX_GPIO_PIN_4;                                       //浮空输入模式
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(UART_TX_PORT_4, &GPIO_InitStructure);

    USART_DeInit(UART4);                                                                    //复位串口
    USART_InitStructure.USART_BaudRate = bound;                                             //配置波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                             //配置帧数据字长
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                                  //配置停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                                     //配置校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;         //配置硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                         //配置收发一起模式
    USART_Init(UART4, &USART_InitStructure);                                                //完成串口的初始化配置

#if defined(UART_IT_4_EN)                                                                   //串口中断
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;                                        //配置中断源
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART_IT_PRI_PRE_4;               //抢断优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART_IT_PRI_SUB_4;                      //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                         //使能中断
    NVIC_Init(&NVIC_InitStructure);                                                         //初始化配置 NVIC

    USART_ITConfig(UART4,USART_IT_RXNE,ENABLE);                                             //使能串口接收中断
    USART_ITConfig(UART4,USART_IT_IDLE,ENABLE);                                             //使能串口空闲中断
#endif

#if defined(UART_RECE_DMA_4_EN)                                                             //DMA方式接收数据
    RCC_AHBPeriphClockCmd(UART_RECE_DMA_CLK_4,ENABLE);                                      //开启DMA时钟
    
    DMA_DeInit(UART_RECE_DMA_CHANNEL_4);                                                    //复位DMA通道
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&UART4->DR;                             //外设基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)U4Buf.RecBuf;                               //内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                                      //方向:从外设接收到内存
    DMA_InitStructure.DMA_BufferSize = U4RecLen;                                            //缓存区长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                        //外设地址不变（外设的寄存器是固定的）
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                                 //内存地址自增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;                 //外设数据单位 8bit
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                         //内存数据单位 8bit
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;                                          //DMA模式：单次
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                                   //优先级：中
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                            //禁止内存到内存的传输
    DMA_Init(UART_RECE_DMA_CHANNEL_4,&DMA_InitStructure);                                   //配置DMA的通道

    #if defined(UART_RECE_DMA_IT_4_EN)
        NVIC_InitStructure.NVIC_IRQChannel = UART_RECE_DMA_IT_4;                            //配置中断源
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART_RECE_DMA_IT_PRI_PRE_4;  //抢断优先级
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART_RECE_DMA_IT_PRI_SUB_4;         //子优先级
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                     //使能中断
        NVIC_Init(&NVIC_InitStructure);                                                     //初始化配置 NVIC
        
        DMA_ITConfig(UART_RECE_DMA_CHANNEL_4,DMA_IT_TC,ENABLE);                             //使能DMA传输完成中断
    #endif
    
    DMA_Cmd(UART_RECE_DMA_CHANNEL_4,ENABLE);                                                //使能DMA
    USART_DMACmd(UART4,USART_DMAReq_Rx,ENABLE);                                             //使能串口DMA接收功能
#endif

#if defined(UART_SEND_DMA_4_EN)                                                             //DMA方式发送数据
    RCC_AHBPeriphClockCmd(UART_SEND_DMA_CLK_4,ENABLE);                                      //开启DMA时钟
    
    DMA_DeInit(UART_SEND_DMA_CHANNEL_4);                                                    //复位DMA通道
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&UART4->DR;                             //外设基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)U4Buf.SenBuf;                               //内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                                      //方向：从内存读取发送到外设
    DMA_InitStructure.DMA_BufferSize = U4SenLen;                                            //缓存区长度
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                        //外设地址不变（外设的寄存器是固定的）
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                                 //内存地址自增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;                 //外设数据单位 8bit
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                         //内存数据单位 8bit
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;                                          //DMA模式：单次
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                                   //优先级：中
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                            //禁止内存到内存的传输
    DMA_Init(UART_SEND_DMA_CHANNEL_4,&DMA_InitStructure);                                   //配置DMA2的通道

    #if defined(UART_SEND_DMA_IT_4_EN)
        NVIC_InitStructure.NVIC_IRQChannel = UART_SEND_DMA_IT_4;                            //配置中断源
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART_SEND_DMA_IT_PRI_PRE_4;  //抢断优先级
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART_SEND_DMA_IT_PRI_SUB_4;         //子优先级
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                     //使能中断
        NVIC_Init(&NVIC_InitStructure);                                                     //初始化配置 NVIC
        
        DMA_ITConfig(UART_SEND_DMA_CHANNEL_4,DMA_IT_TC,ENABLE);                             //使能DMA传输完成中断
    #endif

    DMA_Cmd(UART_SEND_DMA_CHANNEL_4,DISABLE);                                               //失能DMA
    USART_DMACmd(UART4,USART_DMAReq_Tx,ENABLE);                                             //使能串口DMA发送功能
#endif

    USART_Cmd(UART4,ENABLE);                                                                //使能串口
}
/**************************************************************************
函数功能：串口5初始化配置
入口参数：bound：波特率
返回  值：无
**************************************************************************/
void UART_Config_5(u32 bound)
{ 
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(UART_GPIO_CLK_TX_5,ENABLE);                                      //打开引脚时钟
    RCC_APB2PeriphClockCmd(UART_GPIO_CLK_RX_5,ENABLE); 
    RCC_APB1PeriphClockCmd(UART_CLK_5,ENABLE);                                              //打开串口时钟

    GPIO_InitStructure.GPIO_Pin = UART_TX_GPIO_PIN_5;                                       //推挽复用模式
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(UART_TX_PORT_5,&GPIO_InitStructure); 

    GPIO_InitStructure.GPIO_Pin = UART_RX_GPIO_PIN_5;                                       //浮空输入模式
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(UART_RX_PORT_5,&GPIO_InitStructure);

    USART_DeInit(UART5);                                                                    //复位串口
    USART_InitStructure.USART_BaudRate = bound;                                             //配置波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                             //配置帧数据字长
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                                  //配置停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                                     //配置校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;         //配置硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                         //配置收发一起工作
    USART_Init(UART5,&USART_InitStructure);                                                 //完成串口的初始化配置

#if defined(UART_IT_5_EN)                                                                   //串口中断
    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;                                        //配置中断源
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART_IT_PRI_PRE_5;               //抢断优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART_IT_PRI_SUB_5;                      //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                         //使能中断
    NVIC_Init(&NVIC_InitStructure);                                                         //初始化配置 NVIC

    USART_ITConfig(UART5,USART_IT_RXNE,ENABLE);                                             //使能串口接收中断
    USART_ITConfig(UART5,USART_IT_IDLE,ENABLE);                                             //使能串口空闲中断
#endif

    USART_Cmd(UART5, ENABLE);                                                               //使能串口
}

/**************************************************************************
函数功能：开启一次DMA传输
入口参数：DMA_CHx：DMA通道、buff：通道缓存
返回  值：void
注意事项：1、DMA1与DMA2通用  2、接收与发送通用  3、DMA通道一旦使能就进行传输
//DMA_GetCurrDataCounter(DMA_CHx);  //得到当前还剩余多少个数据
**************************************************************************/
void DMA_Transfer(DMA_Channel_TypeDef* DMA_CHx,u16 buff)
{
    DMA_Cmd(DMA_CHx,DISABLE);                   //关闭DMA_CHx所指示的通道      
    DMA_SetCurrDataCounter(DMA_CHx,buff);       //重新配置DMA_CHx通道的缓存大小
    DMA_Cmd(DMA_CHx,ENABLE);                    //使能DMA_CHx所指示的通道 
}

/**************************************************************************
函数功能：选择一个串口接收一个字符
入口参数：pUSARTx：串口名、byte：字符地址
返回  值：收到的字符
注意事项：1、通过传入的指针保存接收到的字符 2、轮询方式接收
**************************************************************************/
bool Usart_ReceByte_P(USART_TypeDef *pUSARTx,u8 *byte)
{
    if(USART_GetFlagStatus(pUSARTx,USART_FLAG_RXNE) != RESET)
    {
        *byte = (u8)pUSARTx->DR;
        return true;
    }
    else
    {
        return false;
    }
}

/**************************************************************************
函数功能：选择一个串口接收一个字符
入口参数：pUSARTx：串口名
返回  值：收到的字符
注意事项：1、直接返回接收到的字符 2、轮询方式接收
**************************************************************************/
u8 Usart_ReceByte(USART_TypeDef *pUSARTx)
{
    u8 Receive = NULL;
    if(USART_GetFlagStatus(pUSARTx,USART_FLAG_RXNE) != RESET)
    {
        Receive = (u8)pUSARTx->DR;
    }
    return Receive;
}

/**************************************************************************
函数功能：选择一个串口发送一个字符（阻塞）
入口参数：pUSARTx：串口号、byte：字符内容、SendCount：发送次数、gap：间隔时间
返回  值：无
注意事项：
1.Usart_SendByte(UART4,0x0d,1,NULL);   //"/r"换行
2.Usart_SendByte(UART4,0x0a,1,NULL);   //"/n"回车
**************************************************************************/
void Usart_SendByte(USART_TypeDef *pUSARTx,char byte,u8 SendCount,u16 gap)
{
    if(SendCount == 1)
    {
        USART_SendData(pUSARTx,byte);                                       //发送一个字节数据到 USART 
        while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE) == RESET);        //等待发送数据寄存器为空
    }
    else
    {
        for(char count = 0;count < SendCount;count++)
        {
            USART_SendData(pUSARTx,byte);                                   //发送一个字节数据到 USART 
            while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE) == RESET);    //等待发送数据寄存器为空
            Delay_ms(gap);                                                  // 发送间隔时间
        }
    }
}
/**************************************************************************
函数功能：选择一个串口发送一个字符串（阻塞）
入口参数：pUSARTx：串口号、str：字符串内容、SendCount：发送次数、gap：间隔时间
返回  值：无
注意事项：
1.可以直接指定发送的字符串内容        Usart_SendString(USART2,"\r\nMessage：");
2.可以发送指定的字符串数组变量        Usart_SendString(USART2,Usart2_RecBuf);
3.可以利用sprintf函数拼接数组         sprintf(Usart2_RecBuf,"Noise:%4.1fdB\r\n",(float)ADCvalue/10);
                                      Usart_SendString(USART2,Usart2_RecBuf);
**************************************************************************/
void Usart_SendString(USART_TypeDef *pUSARTx,char *str,u8 SendCount,u16 gap)
{
    u32 data = 0;
    if(SendCount == 1)
    {
        do
        {
            Usart_SendByte(pUSARTx, *(str + data),1,NULL);
            data++;
        }while (*(str + data) != '\0');
        while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC) == RESET);         // 等待传输完成标志被置位
    }
    else
    {
        for(char count = 0;count < SendCount;count++)
        {
            data = 0;
            do
            {
                Usart_SendByte(pUSARTx, *(str + data),1,NULL);
                data++;
            }while (*(str + data) != '\0');
            while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC) == RESET);     // 等待传输完成标志被置位
            Delay_ms(gap);                                                  // 发送间隔时间
        }
    }
}

/**************************************************************************
函数功能：选择一个串口发送一个字符串（定长）
入口参数：pUSARTx：串口号、str：字符串内容、len：字符串长度、SendCount：发送次数、gap：间隔时间
返回  值：无
注意事项：无
**************************************************************************/
void Usart_SendStrFix(USART_TypeDef *pUSARTx,u8 *buf, u8 len,u8 SendCount,u16 gap)
{
    if(SendCount == 1)
    {
        while(len--)
        {
            while((pUSARTx->SR & 0x40) == 0);
            USART_SendData(pUSARTx,*buf++);
        }
        while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC) == RESET);         // 等待传输完成标志被置位
    }
    else
    {
        for(char count = 0;count < SendCount;count++)
        {
            while(len--)
            {
                while((pUSARTx->SR & 0x40) == 0);
                USART_SendData(pUSARTx,*buf++);
            }
            while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC) == RESET);     // 等待传输完成标志被置位
            Delay_ms(gap);                                                  // 发送间隔时间
        }
    }
}

/**************************************************************************
函数功能：选择一个串口发送一个字符（中断）
入口参数：USARTx：串口号、str：字符串内容 
返回  值：无
注意事项：1、中断发送和阻塞发送不能混用 2、两个中断发送之间需要有间隔否则数据会覆盖
**************************************************************************/
void Usart_SendStrIT(USART_TypeDef* pUSARTx,char *str,u16 count)
{
    if(pUSARTx == USART1)
    {
        if(count < U1SenLen - 1)
        {
            U1Para.TxCnt = count;
            U1Para.StrAddr = str;
        }
    }
    else if(pUSARTx == USART2)
    {
        if(count < U2SenLen - 1)
        {
            U2Para.TxCnt = count;
            U2Para.StrAddr = str;
        }
    }
    else if(pUSARTx == USART3)
    {
        if(count < U3SenLen - 1)
        {
            U3Para.TxCnt = count;
            U3Para.StrAddr = str;
        }
    }
    else if(pUSARTx == UART4)
    {
        if(count < U4SenLen - 1)
        {
            U4Para.TxCnt = count;
            U4Para.StrAddr = str;
        }
    }
    else if(pUSARTx == UART5)
    {
        if(count < U5SenLen - 1)
        {
            U5Para.TxCnt = count;
            U5Para.StrAddr = str;
        }
    }
    //当USART的数据寄存器为空，可以写入新的数据时，会触发此中断。
    USART_ITConfig(pUSARTx,USART_IT_TXE,ENABLE);
}

/**************************************************************************
函数功能：使用microLib重定向printf
入口参数：无
返回  值：无
注意事项：
    1.当定义了其他串口时可使用串口指针定义printf输出的串口，可以设置为打印输入与输出为不同的串口
    2.支持printf函数，而不需要选择use MicroLIB
    3.不使用半主机模式，至少需要重定义_ttywrch _sys_exit _sys_command_string函数，以同时兼容AC6和AC5模式
    4.重定义fputc函数，printf函数最终会通过调用fputc输出字符串到串口

//    static bool flage = false;
//    if(!flage)
//    {
//        flage = true;
//        USART_ClearFlag(USART_Printf,USART_IT_TC);      //防止printf首次发送时第一个字符丢失
//    }
**************************************************************************/
#pragma import(__use_no_semihosting)    //标准库需要的支持函数
struct __FILE 
{ 
    int handle; 
}; 
FILE __stdout;

int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}
char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}

void _sys_exit(int x)
{ 
    x = x; 
}

int fputc(int byte, FILE *f)
{
    while((USART_Printf->SR & 0X40) == 0);      //等待上一个字符发送完成
    USART_Printf->DR = (u8)byte;                //将要发送的字符 byte 写入到DR寄存器
    return byte;
}

/**************************************************************************
函数功能：串口1中断服务函数
入口参数：无
返回  值：无
注意事项：无
**************************************************************************/
#if defined(USART_IT_1_EN)
void USART1_IRQHandler(void)
{
//    if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)            //接收中断
//    {
//        if(U1Para.RxCnt < U1RecLen)
//        {
//            U1Buf.RecBuf[U1Para.RxCnt] = USART_ReceiveData(USART1); //接收数据
//            U1Para.RxCnt++;
//        }
//        else                                                        //超出长度重新接收
//        {
//            U1Para.RxCnt = 0;                                       //重置接收数量
//            U1Para.State = false;                                   //串口接收标志位
//        }
//    }

    if(USART_GetITStatus(USART1,USART_IT_IDLE) != RESET)                //空闲中断
    {
        USART1->DR;                                                     //清除空闲中断标志位
        if(DMA_GetCurrDataCounter(USART_RECE_DMA_CHANNEL_1) != 0)       //溢出检测
        {
            U1Para.State = true;                                        //串口接收标志位
        }
        else
        {
            printf("Usart1 Overflow!");
        }
        U1Para.RxCnt = 0;                                               //重置接收数量
        memset((char*)U1Buf.RecBuf,0,U1RecLen*sizeof(char));            //重置接收数组
        DMA_Transfer(USART_RECE_DMA_CHANNEL_1,U1RecLen);                //再次使能接收
    }
}
#endif
/**************************************************************************
函数功能：串口1DMA发送完成中断
入口参数：无
返回  值：无
注意事项：无
**************************************************************************/
#if defined(USART_SEND_DMA_IT_1_EN)
void USART_SEND_DMA_IT_FUNC_1(void)
{
    if(DMA_GetITStatus(USART_SEND_DMA_IT_FLAG_1) != RESET)          //检查传输完成标志
    {
        DMA_ClearITPendingBit(USART_SEND_DMA_IT_FLAG_1);            //清除传输完成标志（中断）
        memset((char*)U1Buf.SenBuf,0,U1SenLen*sizeof(char));        //重置发送数组
        DMA_Cmd(USART_SEND_DMA_CHANNEL_1,DISABLE);                  //失能DMA通道
    }
}
#endif
/**************************************************************************
函数功能：串口1DMA接收完成中断
入口参数：void
返回  值：void
注意事项：无
**************************************************************************/
#if defined(USART_RECE_DMA_IT_1_EN)
void USART_SEND_DMA_IT_FUNC_1(void)
{ 
    if(DMA_GetITStatus(USART_SEND_DMA_IT_FLAG_1) != RESET)          //检查传输完成标志
    {
        DMA_ClearITPendingBit(USART_SEND_DMA_IT_FLAG_1);            //清除传输完成标志（中断）
    }
}
#endif



/**************************************************************************
函数功能：串口2中断服务函数
入口参数：无
返回  值：无
注意事项：1、下位机通信  2、帧头：0x5E，帧尾：0x5F
调试方案：
//      //收到下位机的数据
//      LCD_ClearRect(135,85,240,32);
//      LCD_SetAsciiFont(&ASCII_Font32);   
//      LCD_DisplayString(5,85,"ReceCar:");
//      LCD_DisplayChar(140,85,ReceCarSta);

//    if(Task.DebugFlag){    
//     LCD_SetAsciiFont(&ASCII_Font32);    
//     LCD_DisplayString(5,85,"ReceCar:");
//     LCD_ClearRect(135,85,240,32);
//     LCD_DisplayChar(140,85,ReceCarSta);
//    }
**************************************************************************/
#if defined(USART_IT_2_EN)
void USART2_IRQHandler(void)             
{
//    if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET)            //接收中断
//    {
//        if(U2Para.RxCnt < U2RecLen)
//        {
//            U2Buf.RecBuf[U2Para.RxCnt] = USART_ReceiveData(USART2); //接收数据
//            U2Para.RxCnt++;
//        }
//        else                                                        //超出长度重新接收
//        {
//            U2Para.RxCnt = 0;                                       //重置接收数量
//            U2Para.State = false;                                   //串口接收标志位
//        }
//    }
    
    if(USART_GetITStatus(USART2,USART_IT_IDLE) != RESET)            //空闲中断
    {
        USART2->DR;                                                     //清除空闲中断标志位
        if(DMA_GetCurrDataCounter(USART_RECE_DMA_CHANNEL_2) != 0)       //溢出检测
        {
            U2Para.State = true;                                        //串口接收标志位
            if(SysValue.InitComplete)                                   //如果系统初始化完成
            {
                rt_mq_send(Usart2_list,                                 //写入队列的句柄
                        &U2Buf.RecBuf,                                  //写入的数据
                        sizeof(U2Buf.RecBuf));                          //数据的长度
                rt_event_send(Usart_event,USART2_EVENT);                //发送串口2事件
            }
        }
        else
        {
            printf("Usart2 Overflow!");
        }
        U2Para.RxCnt = 0;                                               //重置接收数量
        memset((char*)U2Buf.RecBuf,0,U2RecLen*sizeof(char));            //重置接收数组
        DMA_Transfer(USART_RECE_DMA_CHANNEL_2,U2SenLen);                //再次使能接收
    }
}
#endif
/**************************************************************************
函数功能：串口2DMA发送完成中断
入口参数：无
返回  值：无
注意事项：无
**************************************************************************/
#if defined(USART_SEND_DMA_IT_2_EN)
void USART_SEND_DMA_IT_FUNC_2(void)
{
    if(DMA_GetITStatus(USART_SEND_DMA_IT_FLAG_2) != RESET)          //检查传输完成标志
    {
        DMA_ClearITPendingBit(USART_SEND_DMA_IT_FLAG_2);            //清除传输完成标志（中断）
        memset(U2Buf.SenBuf,0,U2SenLen*sizeof(char));               //重置发送数组
        DMA_Cmd(USART_SEND_DMA_CHANNEL_2,DISABLE);                  //失能DMA通道
    }
}
#endif
/**************************************************************************
函数功能：串口2DMA接收完成中断
入口参数：void
返回  值：void
注意事项：无
**************************************************************************/
#if defined(USART_RECE_DMA_IT_2_EN)
void USART_SEND_DMA_IT_FUNC_2(void)
{ 
    if(DMA_GetITStatus(USART_SEND_DMA_IT_FLAG_2) != RESET)          //检查传输完成标志
    {
        DMA_ClearITPendingBit(USART_SEND_DMA_IT_FLAG_2);            //清除传输完成标志（中断）
    }
}
#endif



/**************************************************************************
函数功能：串口3中断服务函数
入口参数：无
返回  值：无
注意事项：1、扫码模块或WiFi模块通信  2、扫码模块帧尾：0x0a、0x0d
调试方案：
//      //收到扫码模块的数据
//      LCD_ClearRect(150,50,240,32);
//      LCD_SetAsciiFont(&ASCII_Font32);   
//      LCD_DisplayString(5,50,"ReceK210:");
//      LCD_DisplayString(155,50,ReceK210Data);
**************************************************************************/
#if defined(USART_IT_3_EN)
void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)                //接收中断
    {
        if(U3Para.RxCnt < U3RecLen)                                     //溢出检测
        {
            U3Buf.RecBuf[U3Para.RxCnt] = USART_ReceiveData(USART3);     //接收数据
            U3Para.RxCnt++;
        }
        else                                                            //超出长度重新接收
        {
            U3Para.RxCnt = 0;                                           //重置接收数量
            U3Para.State = false;                                       //串口接收标志位
        }
    }

    if(USART_GetITStatus(USART3,USART_IT_IDLE) != RESET)                //空闲中断
    {
        USART3->DR;                                                     //清除空闲中断标志位
        U3Para.State = true;                                            //串口接收标志位
        U3Para.RxCnt = 0;                                               //重置接收数量
        if(SysValue.InitComplete)                                       //如果系统初始化完成
        {
            rt_mq_send(Usart3_list,                                     //写入队列的句柄
                       &U3Buf.RecBuf,                                   //写入的数据
                       sizeof(U3Buf.RecBuf));                           //数据的长度
            rt_event_send(Usart_event,USART3_EVENT);                    //发送串口3事件
            memcpy((char*)U3Buf.ShowBuf,(char*)U3Buf.RecBuf,U3RecLen*sizeof(char));     //复制接收数组
            memset((char*)U3Buf.RecBuf,0,U3RecLen*sizeof(char));                        //清空接收数组
        }
    }
}
#endif
/**************************************************************************
函数功能：串口3DMA发送完成中断
入口参数：无
返回  值：无
注意事项：无
**************************************************************************/
#if defined(USART_SEND_DMA_IT_3_EN)
void USART_SEND_DMA_IT_FUNC_3(void)
{
    if(DMA_GetITStatus(USART_SEND_DMA_IT_FLAG_3) != RESET)          //检查传输完成标志
    {
        DMA_ClearITPendingBit(USART_SEND_DMA_IT_FLAG_3);            //清除传输完成标志（中断）
        memset(U3Buf.SenBuf,0,U3SenLen*sizeof(char));               //重置发送数组
        DMA_Cmd(USART_SEND_DMA_CHANNEL_3,DISABLE);                  //失能DMA通道
    }
}
#endif
/**************************************************************************
函数功能：串口3DMA接收完成中断
入口参数：void
返回  值：void
注意事项：无
**************************************************************************/
#if defined(USART_RECE_DMA_IT_3_EN)
void USART_SEND_DMA_IT_FUNC_3(void)
{ 
    if(DMA_GetITStatus(USART_SEND_DMA_IT_FLAG_3) != RESET)          //检查传输完成标志
    {
        DMA_ClearITPendingBit(USART_SEND_DMA_IT_FLAG_3);            //清除传输完成标志（中断）
    }
}
#endif



/**************************************************************************
函数功能：串口4中断服务函数
入口参数：无
返回  值：无
注意事项：蓝牙通信
**************************************************************************/
#if defined(UART_IT_4_EN)
void UART4_IRQHandler(void)
{
//    if(USART_GetITStatus(UART4,USART_IT_RXNE) != RESET)               //接收中断
//    {
//        if(U4Para.RxCnt < U4RecLen)
//        {
//            U4Buf.RecBuf[U4Para.RxCnt] = USART_ReceiveData(UART4);    //接收数据
//            U4Para.RxCnt++;
//        }
//        else                                                          //超出长度重新接收
//        {
//            U4Para.RxCnt = 0;                                         //重置接收数量
//            U4Para.State = false;                                     //串口接收标志位
//        }
//    }
    
    if(USART_GetITStatus(UART4,USART_IT_IDLE) != RESET)                                 //空闲中断
    {   
        UART4->DR;                                                                      //清除空闲中断标志位
        if(DMA_GetCurrDataCounter(UART_RECE_DMA_CHANNEL_4) != 0)                        //溢出检测
        {   
            U4Para.State = true;                                                        //串口接收标志位
            if(SysValue.InitComplete)                                                   //如果系统初始化完成
            {   
                rt_mq_send(Uart4_list,                                                  //写入队列的句柄
                           &U4Buf.RecBuf,                                               //写入的数据
                           sizeof(U4Buf.RecBuf));                                       //数据的长度
                rt_event_send(Usart_event,USART4_EVENT);                                //发送串口4事件
            }
            memcpy((char*)U4Buf.ShowBuf,(char*)U4Buf.RecBuf,U4RecLen*sizeof(char));     //复制接收数组
        }
        else
        {
            printf("Uart4 Overflow!");
        }
        memset((char*)U4Buf.RecBuf,0,U4RecLen*sizeof(char));                            //重置接收数组
        DMA_Transfer(UART_RECE_DMA_CHANNEL_4,U4RecLen);                                 //再次使能接收 
    }
}
#endif
/**************************************************************************
函数功能：串口4DMA发送完成中断
入口参数：无
返回  值：无
注意事项：无
**************************************************************************/
#if defined(UART_SEND_DMA_IT_4_EN)
void USART_SEND_DMA_IT_FUNC_4(void)
{
    if(DMA_GetITStatus(USART_SEND_DMA_IT_FLAG_4) != RESET)          //检查传输完成标志
    {
        DMA_ClearITPendingBit(USART_SEND_DMA_IT_FLAG_4);            //清除传输完成标志（中断）
        memset((char*)U4Buf.SenBuf,0,U4SenLen*sizeof(char));        //重置发送数组
        DMA_Cmd(UART_SEND_DMA_CHANNEL_4,DISABLE);                   //失能DMA通道
    }
}
#endif
/**************************************************************************
函数功能：串口4DMA接收完成中断
入口参数：void
返回  值：void
注意事项：无
**************************************************************************/
#if defined(UART_RECE_DMA_IT_4_EN)
void UART_SEND_DMA_IT_FUNC_4(void)
{ 
    if(DMA_GetITStatus(UART_SEND_DMA_IT_FLAG_4) != RESET)           //检查传输完成标志
    {
        DMA_ClearITPendingBit(UART_SEND_DMA_IT_FLAG_4);             //清除传输完成标志（中断）
    }
}
#endif



/**************************************************************************
函数功能：串口5中断服务函数
入口参数：无
返回  值：无
注意事项：无
**************************************************************************/
#if defined(UART_IT_5_EN)
void UART5_IRQHandler(void)
{
    if(USART_GetITStatus(UART5,USART_IT_RXNE) != RESET)             //接收中断
    {
        if(U5Para.RxCnt < U5RecLen)
        {
            U5Buf.RecBuf[U5Para.RxCnt] = USART_ReceiveData(UART5);  //接收数据
            U5Para.RxCnt++;
        }
        else                                                        //超出长度重新接收
        {
            U5Para.RxCnt = 0;                                       //重置接收数量
            U5Para.State = false;                                   //串口接收标志位
        }
    }
    if(USART_GetITStatus(UART5,USART_IT_IDLE) != RESET)             //空闲中断
    {
        UART5->DR;                                                  //清除空闲中断标志位
        U5Para.RxCnt = 0;                                           //重置接收数量
        U5Para.State = true;                                        //串口接收标志位
    }
}
#endif
