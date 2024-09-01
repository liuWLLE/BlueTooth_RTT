#include "board.h" 

// Hardware
#include "Usart.h"
#include "Flash.h"
#include "WDG.h"
 
// Bsp
#include "Iap.h"
#include "Key.h" 
#include "Led.h"
#include "Steering.h"
#include "Motor.h"
#include "Esp8266.h"
#include "LcdInit.h"

// App
#include "Display_Thread.h"

//开机重置系统任务码控制
TaskState SysTask =
{
    .NowTask = TaskStart,       //任务逻辑
    .OldTask = TaskStart,
    .OldShow = TaskStart,
    .CodeS = NULL,              //完整任务码
    .Code1 = NULL,              //第一次任务码
    .Code2 = NULL,              //第二次任务码
};
//开机重置系统运行参数值
Operat_Control SysValue =
{
    .QR_Mode = ScanModule,      //任务码接收方式 ScanModule/ESP8266/DT_06
    .ReceCarSta = NULL,         //收到下位机的状态
    .TaskStart = false,         //任务运行标志位
    .DebugStart = false,        //程序调试标志位
    .DirectGrab = false,        //物料判断抓取标志位
    .InitComplete = false       //初始化未完成
};

// 使用动态内存时需要开启该宏
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    // 从内部 SRAM 里面分配一部分静态内存来作为 rtt 的堆空间，这里配置为 4KB
    #define RT_HEAP_SIZE    8192
static uint32_t rt_heap[RT_HEAP_SIZE];
/**************************************************************************
函数功能：获取堆的起始地址
入口参数：无 
返回  值：堆的起始地址
**************************************************************************/
RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}
/**************************************************************************
函数功能：获取堆的结束地址
入口参数：无 
返回  值：堆的结束地址
**************************************************************************/
RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif

/**************************************************************************
函数功能：开发板硬件初始化函数
入口参数：无 
返回  值：无
注意事项：1、初始化阶段线程未开始调度，串口仍采用全局变量方式，线程开始后采用队列方法
          2、屏幕动画高度位置：15、35、56、77、98、119、140、161、182、203、224、245、266
**************************************************************************/
void rt_hw_board_init(void)
{
    // 设置中断偏移
    sys_nvic_set_vector_table(FLASH_BASE_SET,FLASH_SHIFT_ADDR);
    
    // 系统滴答定时器配置
    SysTick_Config(SystemCoreClock/RT_TICK_PER_SECOND);

    // 单片机外设初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_init(72);

    // LCD初始化
    LCD_Init();
    
    // 加载设备信息
    Delay_ms(200);
    LCD_Clear();
    LCD_SetColor(LCD_YELLOW);
    LCD_SetAsciiFont(&ASCII_Font16);
    LCD_DisplayString(5,15,"Copyright (C)  @2023");
    LCD_SetColor(LCD_BLUE);
    LCD_DisplayString(5,35,"STM32F103RCT6  72 Mhz");
    LCD_DisplayString(5,56,"FLASH:256 KB  SRAM:48 KB");
    LCD_SetColor(LCD_YELLOW);
    LCD_DisplayString(5,77,"Initialize System:");

    // 初始化Key Led
    Delay_ms(200);
    KeyInit();
    LaserLed_Init();
    BoardLed_Init();

    // 按键设置lash中IAP标志位
    switch(KeyScanSingle(false))
    {
        case KeyShort:
            {
                LCD_SetColor(LCD_RED);
                FLASH_Write(FLASH_DATA_ADDR,&*(u16*)1,1);
                LCD_DisplayString(5,98,"Set IAP Flag Success!");
                Delay_ms(1000);
                LCD_DisplayString(5,119,"System Restart!");
                Delay_ms(500);
                sys_soft_reset();
            }break;
        case KeyLong:
            {
                LCD_SetColor(LCD_RED);
                FLASH_Write(FLASH_DATA_ADDR,&*(u16*)1,1);
                LCD_DisplayString(5,98,"Set IAP Flag Success!");
                Delay_ms(1000);
                LCD_DisplayString(5,119,"System Restart!");
                Delay_ms(500);
                sys_soft_reset();
            }break;
        case KeyRelease:break;
        default:break; 
    }
    LCD_SetColor(LCD_WHITE);
    LCD_DisplayString(5,98,"Key Led Init Success..."); 

    // 步进电机初始化
    Delay_ms(200);
    Motor_Init();
    LCD_DisplayString(5,119,"Motor Init Success...");

    // 舵机初始化
    Delay_ms(200);
    Steering_Init();
    LCD_DisplayString(5,140,"Steering Init Success...");

    // 串口初始化
    Delay_ms(200);
    USART_Config_2(19200);
    UART_Config_4(115200);
    if(SysValue.QR_Mode == ScanModule)
    {
        USART_Config_3(9600);
        LCD_DisplayString(5,161,"Serial Init Success...");
        SysValue.InitComplete = true;

    }
    else if(SysValue.QR_Mode == ESP8266)
    {
        USART_Config_3(115200);
        ESP8266_Init(LCD_DisplayString,
                     Usart_SendString,
                     USART3,
                     (char*)U3Buf.RecBuf,
                     (bool*)&U3Para.State,
                     U3RecLen);
        LCD_DisplayString(5,161,"Serial Init Success...");
        SysValue.InitComplete = ConfigurationEsp8266();
    }
    else if(SysValue.QR_Mode == DT_06)
    {
        USART_Config_3(115200);
        LCD_DisplayString(5,161,"Serial Init Success...");
        
    }
    
    // 看门狗初始化（40,000/4*2^(4)/625 = 1 HZ）
    Delay_ms(200);
    IWDG_Init(4,625);
    LCD_DisplayString(5,182,"IWDG Init Success...");
    
    // 初始化完成
    Delay_ms(200);
    if(!SysValue.DebugStart)
    {
        if(SysValue.InitComplete)
        {
            ScreenDisplayKeyState("Hello World!");
        }
        else
        {
            ScreenDisplayKeyState("System Error!");
        }
    }


/*
   RT-Thread 为开发板组件提供的一个初始化函数，该函数在components.c里面实现，
   由 rtconfig.h 里面的宏RT_USING_COMPONENTS_INIT决定是否调用，默认是开启。
*/
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
/*
   RT-Thread 提供的一个内存初始化函数，只有在使用 RT-Thread 提供的动态内存分配函数时才需要使用到
  该函数在 mem.c 里面实现，由 rtconfig.h 里面的 RT_USING_HEAP 和 RT_USING_USER_MAIN 这两个决定是否调用
*/
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
}

/**************************************************************************
函数功能：SysTick 中断服务函数
入口参数：无 
返回  值：无
注意事项：SysTick 中断服务函数在固件库文件 stm32f10x_it.c 中也定义了，而现在
          在 board.c 中又定义一次，那么编译的时候会出现重复定义的错误，解决
          方法是可以把 stm32f10x_it.c 中的注释或者删除即可
**************************************************************************/
void SysTick_Handler(void)
{
    rt_interrupt_enter();       // 进入中断
    rt_tick_increase();         // 更新时基
    rt_interrupt_leave();       // 离开中断
}

// 串口初始化以及控制台输出实现
#ifdef RT_USING_CONSOLE
/**************************************************************************
函数功能：重映射串口 DEBUG_USARTx 到 rt_kprintf()
入口参数：str：要输出到串口的字符串
返回  值：无
注意事项：DEBUG_USARTx 是在 bsp_usart.h 中定义的宏，默认使用串口 1
**************************************************************************/
void rt_hw_console_output(const char *str)
{
    rt_enter_critical();    // 进入临界段
    while(*str!='\0')       // 直到字符串结束
    {
        if(*str=='\n')      // 换行
        {
            USART_SendData(DEBUG_USARTx,'\r');
            while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_TXE) == RESET);
        }
        USART_SendData(DEBUG_USARTx, *str++);
        while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_TXE) == RESET);
    }
    rt_exit_critical();     // 退出临界段
}

#endif


