#include "delay.h"

static u8 fac_us;       //us延时系数（系统频率72Mhz下us延时系数为72）
static u16 fac_ms;      //ms延时系数（在os下代表可以延时的最小毫秒单位）

/**************************************************************************
函数功能：初始化延迟函数
入口参数：sysclk：系统时钟频率
返回  值：无
注意事项：无
**************************************************************************/
void Delay_init(u8 SysClk)
{
    fac_us = SysClk;                                    
    fac_ms = 1000 / RT_TICK_PER_SECOND;                 //代表OS可以延时的最小毫秒单位 1ms
    
#if (!OS_RTTHREAD_USED)                                 //使用RT-Thread则不需要再初始化滴答定时器
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);    //配置滴答定时器时钟源为系统主时钟

    u32 reload;                                         //滴答定时器重载值
    reload = SysClk;                                    //初始为每秒的时钟计数次数 单位Mhz 72
    reload *= 1000000 / RT_TICK_PER_SECOND;             //根据系统节拍数设定每个节拍的微秒数
                                                        //1000 000即每秒微秒数 举例72*1000 0000/1000=72000
                                                        //则一个节拍的微秒数为72000
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;          //开启SysTick中断
    SysTick->LOAD = reload;                             //每1/RT_TICK_PER_SECOND秒中断一次
                                                        //系统的时钟为72Mhz，而一个节拍的微秒数为72000
                                                        //因此中断频率为72000 000/72000=1000 Hz
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;           //使能SysTick
#endif
}

/**************************************************************************
函数功能：延时nus
入口参数：nus：要延时的us数
返回  值：无
注意事项：通过不断检查 SysTick 计数器的值来实现微秒级别的精确延时
**************************************************************************/
void Delay_us(u32 nus)
{
    u32 ticks = nus*fac_us;                     //存储需要的节拍数
    u32 tnow,tcnt = 0;                          //当前计数器值、累计的节拍数
    u32 reload = SysTick->LOAD;                 //系统滴答定时器（SysTick）的装载值
    u32 told = SysTick->VAL;                    //获取当前的计数器值作为初始值
    while(true)
    {
        tnow = SysTick->VAL;                    //获取当前的计数器值
        if(tnow != told)                        //判断当前计数器值与上一个计数器值 told 是否相同
        {
            if(tnow < told)                     //计数器发生溢出需要特殊累计
            {
                tcnt += told - tnow;            //这里注意一下SYSTICK是一个递减的计数器就可以了
            }
            else                                //没有溢出则正常累计
            {
                tcnt += reload - tnow + told;   //累加计数器值的差值更新到 tcnt 中
            }
            told = tnow;                        //更新 told 为当前的计数器值
            if(tcnt >= ticks)                   //时间超过/等于要延迟的时间
            {
                break;                          //退出
            }
        }  
    }
}  

/**************************************************************************
函数功能：延时nms
入口参数：nms：要延时的ms数
返回  值：无
注意事项：只能在线程（任务）中使用
**************************************************************************/
void DelayOS_ms(u16 nms)
{
    if(nms >= fac_ms)                   //延时的时间大于OS的最小时间周期 
    {
        rt_thread_delay(nms / fac_ms);  //RTT延时
    }
    nms %= fac_ms;                      //OS无法提供这么小的延时则采用阻塞方式补齐余下时间   
    Delay_us((u32)(nms * 1000));        //阻塞方式补齐延时时间
}

/**************************************************************************
函数功能：阻塞延时nms
入口参数：nms：要延时的ms数
返回  值：无
注意事项：不会引起任务调度
**************************************************************************/
void Delay_ms(u16 nms)
{
    for(u16 i = 0;i < nms;i++)
    {
        Delay_us(1000);
    }
}
