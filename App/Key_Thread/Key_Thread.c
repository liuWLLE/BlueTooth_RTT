#include "Key_thread.h"

// Bsp
#include "Key.h" 

/**************************************************************************
函数功能：按键控制线程函数
入口参数：无
返回  值：无
注意事项：无
**************************************************************************/
void KeyCtrl_thread_entry(void* parameter)
{
    AddKeyCallBack(KeyControlTask);

    //按键值传输（邮箱）
    KeyValue_mail = rt_mb_create("KeyMail",                             //邮箱名字
                                 5,                                     //邮箱大小
                                 RT_IPC_FLAG_FIFO);                     //信号量模式 FIFO
    while(true)
    {
        #if defined(DEBUG_THREAD_CONSUME_TIME)
            rt_tick_t StartTick = rt_tick_get();
        #endif

        #if defined(DEBUG_THREAD_INTERVAL_TIME)
            static rt_tick_t NowTick,OldTick;
            NowTick = rt_tick_get(); 
            rt_kprintf("LedKeyCtrl_Interval:%d\n",NowTick-OldTick);
            OldTick = NowTick;
        #endif
        
        KeyDeviceRunning();
        
        #if defined(DEBUG_THREAD_CONSUME_TIME)
            rt_kprintf("LedKeyCtrl_Consume:%d\n",rt_tick_get()-StartTick);
        #endif
        rt_thread_delay(10);
    }
}


/**************************************************************************
函数功能：按键控制任务切换
入口参数：KeyValue：按键值
返回  值：无
注意事项：回调函数
**************************************************************************/
void KeyControlTask(u8 KeyValue)
{
    switch(KeyValue)
    {
        case KeyShort:                                                          //控制系统开始运行
            {
                rt_mb_send(KeyValue_mail,(rt_uint32_t)KeyValue);                //发送邮箱消息
                if(SysValue.TaskStart)                                          //系统已经开始运行
                {
                    rt_kprintf("System Started\n");
                }
                else                                                            //系统未开始运行
                {
                    if(MainCtrl_thread != RT_NULL)
                    {
                        if(MainCtrl_thread->stat == RT_THREAD_INIT)             //初始态
                        {
                            rt_kprintf("MainCtrl thread success\n");
                            rt_thread_startup(MainCtrl_thread);                 //开始主控制线程
                        }
                        else if(MainCtrl_thread->stat == RT_THREAD_SUSPEND)     //挂起态
                        {
                            rt_kprintf("MainCtrl thread resume\n");
                            rt_thread_resume(MainCtrl_thread);                  //恢复主控制线程
                        }
                    }
                    if(SysValue.DebugStart)                                     //调试模式
                    {
                        rt_kprintf("LcdDebug thread already\n");
                    }
                    else                                                        //正常模式
                    {
                        if(LcdNormal_thread != RT_NULL)
                        {
                            if(LcdDebug_thread->stat == RT_THREAD_INIT)         //初始态
                            {
                                rt_kprintf("LcdNormal thread success\n");
                                rt_thread_startup(LcdNormal_thread);            //开始LCD正常刷新线程
                            }
                            else
                            {
                                rt_kprintf("LcdNormal thread already\n");
                            }
                        }
                    }
                    SysValue.TaskStart = true;                                  //开始状态调度
                }
            }
            break;
        case KeyLong:                                                           //控制系统进入调试模式
            {
                rt_mb_send(KeyValue_mail,(rt_uint32_t)KeyValue);                //发送邮箱消息
                if(SysValue.TaskStart)                                          //系统已经开始运行
                {
                    if(SysValue.DebugStart)                                     //调试模式
                    {
                        rt_kprintf("LcdDebug thread already\n");
                    }
                    else
                    {
                        rt_thread_suspend(LcdNormal_thread);                    //挂起LCD正常刷新线程
                        if(LcdDebug_thread != RT_NULL)
                        {
                            rt_kprintf("LcdDebug thread success\n");
                            rt_thread_startup(LcdDebug_thread);                 //开始LCD调试刷新线程
                        }
                        SysValue.DebugStart = true;                             //设置为调试模式
                    }
                }
                else                                                            //系统还未开始运行
                {
                    if(LcdDebug_thread != RT_NULL)
                    {
                        rt_kprintf("LcdDebug thread success\n");
                        rt_thread_startup(LcdDebug_thread);                     //开始LCD调试刷新线程
                    }
                    SysValue.DebugStart = true;                                 //设置为调试模式
                }
            }
            break;
        case KeyPersist:                                                        //控制系统停止运行（不影响LCD刷新）
            {
                rt_mb_send(KeyValue_mail,(rt_uint32_t)KeyValue);                //发送邮箱消息
                if(SysValue.TaskStart)                                          //系统已经开始运行
                {
                    rt_kprintf("System suspend\n");
                    SysValue.TaskStart = false;
                    rt_thread_suspend(MainCtrl_thread);
                }
            }
            break;
        case KeyRelease:break;
        default:break;
    }
}
