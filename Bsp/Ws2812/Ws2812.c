#include "Ws2812.h" 

//单个灯珠的编码对应的比较值数组
uint16_t WS2812B_DATA_BUFF[WS2812B_DATA_SIZE*WS2812B_LED_NUM]; 

/**************************************************************************
函数功能：WS2812灯环初始化（定时器+DMA）
入口参数：无
返回  值：无
注意事项：1、定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us（Ft=定时器工作频率，单位:Mhz）
          2、DMA发送一旦使能就开始 2、采用发送完成中断来重置标志位
**************************************************************************/
void WS2812B_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};
    DMA_InitTypeDef DMA_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(WS2812B_GPIO_CLK,ENABLE);                                //使能引脚时钟
    RCC_APB1PeriphClockCmd(WS2812B_TIMER_CLK,ENABLE);                               //使能定时器时钟
    RCC_AHBPeriphClockCmd(WS2812B_DMA_CLK,ENABLE);                                  //使能DMA时钟
                    
    GPIO_InitStructure.GPIO_Pin = WS2812B_GPIO_PIN;                                 //初始化引脚
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                               //引脚速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                                 //复用推挽输出
    GPIO_Init(WS2812B_PORT,&GPIO_InitStructure);                                    //根据指定的参数初始化引脚

    TIM_DeInit(WS2812B_TIMER);                                                      //复位定时器
    TIM_TimeBaseStructure.TIM_Prescaler = WS2812B_TIMER_ARR;                        //定时器分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                     //向上计数模式
    TIM_TimeBaseStructure.TIM_Period = WS2812B_TIMER_PSC;                           //自动重装载值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;                         //时钟分割
    TIM_TimeBaseInit(WS2812B_TIMER,&TIM_TimeBaseStructure);                         //根据指定的参数初始化定时器
                
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                               //选择定时器模式：TIM脉冲宽度调制模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                   //比较输出使能
    TIM_OCInitStructure.TIM_Pulse = 0;                                              //待装入捕获比较寄存器的脉冲值(此程序不用加初值)
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;                       //输出极性：TIM输出比较极性高
    TIM_OC2Init(WS2812B_TIMER,&TIM_OCInitStructure);                                //根据指定的参数初始化定时器外设
    TIM_OC2PreloadConfig(WS2812B_TIMER,TIM_OCPreload_Enable);                       //使能定时器在通道上的预装载寄存器

    DMA_DeInit(WS2812B_DMA_CHANNEL);                                                //复位DMA通道
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(WS2812B_TIMER_CHANNEL);       //DMA外设基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)WS2812B_DATA_BUFF;                  //DMA内存源地址 
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                              //数据传输方向：从内存读取发生到外设
    DMA_InitStructure.DMA_BufferSize = WS2812B_DATA_SIZE;                           //DMA通道的缓存大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                //外设地址不变（外设的寄存器是固定的）
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                         //内存地址自增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;     //数据单位 16bit
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;             //数据单位 16bit
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                                   //DMA模式：单次
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                           //优先级：中
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                    //是否设置从内存到内存的传输
    DMA_Init(WS2812B_DMA_CHANNEL, &DMA_InitStructure);                              //配置DMA通道
    
    NVIC_InitStructure.NVIC_IRQChannel = WS2812B_DMA_INTERRUPT;                     //配置中断源
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = WS2812B_DMA_IT_PRI_PRE;  //抢断优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = WS2812B_DMA_IT_PRI_SUB;         //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                                 //使能中断
    NVIC_Init(&NVIC_InitStructure);                                                 //初始化NVIC配置
    DMA_ITConfig(WS2812B_DMA_CHANNEL,DMA_IT_TC,ENABLE);                             //使能DMA传输完成中断
    
    TIM_Cmd(WS2812B_TIMER, DISABLE);                                                //失能定时器，防止第一个脉冲异常
    TIM_DMACmd(WS2812B_TIMER, WS2812B_TIMER_DMA, ENABLE);                           //使能定时器的DMA功能
    DMA_Cmd(WS2812B_DMA_CHANNEL, DISABLE);                                          //失能DMA通道
}

/**************************************************************************
函数功能：WS2812灯带DMA发送数据
入口参数：num：点亮灯珠的个数
返回  值：void
注意事项：无
**************************************************************************/
static void WS2812B_DMA_Send(uint8_t num)
{
    DMA_SetCurrDataCounter(WS2812B_DMA_CHANNEL,num*WS2812B_DATA_SIZE);  //设置DMA通道的缓存大小
    DMA_Cmd(WS2812B_DMA_CHANNEL,ENABLE);                                //使能DMA通道
    TIM_Cmd(WS2812B_TIMER,ENABLE);                                      //开启定时器
    while(DMA_GetFlagStatus(WS2812B_DMA_IT_FLAG) != SET);               //等待DMA传输完成
    DMA_Cmd(WS2812B_DMA_CHANNEL,DISABLE);                               //失能DMA通道
    DMA_ClearFlag(WS2812B_DMA_IT_FLAG);                                 //清除DMA传输标志位
    TIM_Cmd(WS2812B_TIMER, DISABLE);                                    //关闭定时器
}

/**************************************************************************
函数功能：WS2812灯带DMA发送数据（中断）
入口参数：num：点亮灯珠的个数
返回  值：void
注意事项：1、DMA发送一旦使能就开始 2、采用发送完成中断来重置标志位
**************************************************************************/
static void WS2812B_DMA_SendIT(uint8_t num)
{
    DMA_SetCurrDataCounter(WS2812B_DMA_CHANNEL,num*WS2812B_DATA_SIZE);  //设置DMA通道的缓存大小
    DMA_Cmd(WS2812B_DMA_CHANNEL,ENABLE);                                //使能DMA通道
    TIM_Cmd(WS2812B_TIMER,ENABLE);                                      //开启定时器
}

/**************************************************************************
函数功能：复位灯带
入口参数：void
返回  值：void
注意事项：延时时间最小值大于50us即可
**************************************************************************/
void WS2812B_Reset(void)
{
    TIM_Cmd(WS2812B_TIMER,DISABLE);
    WS2812B_LOW;
    Delay_ms(1);
}

/**************************************************************************
函数功能：写数据编码
入口参数：num：灯珠数量、GRB_Data：RGB颜色数据
返回  值：void
**************************************************************************/
static void WS2812B_Write_24Bits(uint8_t num,uint32_t GRB_Data)
{
    uint8_t i,j;
    for(j = 0; j < num; j++)
    {
        for(i = 0; i < WS2812B_DATA_SIZE; i++)
        {
            /*因为数据发送的顺序是GRB，高位先发，所以从高位开始判断，判断后比较值先放入缓存数组*/
            WS2812B_DATA_BUFF[i+j*WS2812B_DATA_SIZE] = ((GRB_Data << i) & 0x800000) ? WS2812B_T1H : WS2812B_T0H;
        }
    }
}

/**************************************************************************
函数功能：灯环整体设置颜色
入口参数：red、green、blue 三原色
返回  值：void
注意事项：green << 16 | red << 8 | blue 将三原色单独数据合并为24位数据
**************************************************************************/
void WS2812B_SetColor(Color_WS2812B Color)
{
    uint8_t red,green,blue;
    switch(Color){
        case Red:   red = 255,green = 0,  blue = 0;break;
        case Green: red = 0,  green = 255,blue = 0;break;
        case Blue:  red = 0,  green = 0,  blue = 255;break;
        case White: red = 255,green = 255,blue = 150;break;
        case None:  red = 0,  green = 0,  blue = 0;break;
    }
    WS2812B_Write_24Bits(WS2812B_LED_NUM,green << 16 | red << 8 | blue);
    WS2812B_DMA_SendIT(WS2812B_LED_NUM);
    Delay_ms(2);
    WS2812B_DMA_SendIT(WS2812B_LED_NUM);
    Delay_ms(2);
    WS2812B_DMA_SendIT(WS2812B_LED_NUM);  //传输三次避免出错
}

/**************************************************************************
函数功能：灯环第n位设置颜色
入口参数：void
返回  值：void
注意事项：先将Num位数组填充所需颜色，再把前Num - 1为数组清空
**************************************************************************/
void WS2812B_SetColor_N(uint8_t Num,uint8_t red,uint8_t green,uint8_t blue)
{
    WS2812B_Write_24Bits(Num,green << 16 | red << 8 | blue);
    WS2812B_Write_24Bits(Num - 1,0x000000);
    WS2812B_DMA_SendIT(Num);
}

/**************************************************************************
函数功能：WS2812灯带DMA中断
入口参数：void
返回  值：void
注意事项：1、采用发送完成中断来重置标志位 2、失能DMA通道避免下次发送之前不会被触发
**************************************************************************/
void WS2812B_DMA_IT_FUNC(void)
{
    if(DMA_GetITStatus(WS2812B_DMA_IT_FLAG) != RESET)   //检查传输完成标志
    {
        DMA_ClearITPendingBit(WS2812B_DMA_IT_FLAG);     //清除传输完成标志（中断）
        DMA_Cmd(WS2812B_DMA_CHANNEL,DISABLE);           //失能DMA通道
        TIM_Cmd(WS2812B_TIMER,DISABLE);                 //关闭定时器
    }
}

