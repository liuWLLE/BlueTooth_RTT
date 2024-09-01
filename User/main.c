#include "sys.h"

// App
#include "Arm_Thread.h"
#include "Transfer_thread.h"
#include "Key_thread.h"
#include "Display_Thread.h"
#include "Led_WDG.h"


/*----------------------------------------------
所用资源一览表：
舵机驱动--Tim1、灯环驱动--Tim2、控制中断---Tim6、电机驱动---Tim8
下位机通信-----Usart2（115200）、二维码通信----Uart3（9600/115200）、蓝牙通信----Uart4（115200）
STM32占用GPIO一览表：
LCD屏幕： PB3、PB4、PB5、PB6、PB7
补光灯IO：PA1、PB8、PB9
电机控制：PA5、PC7、PB13 / PA4、PC6、PB12
舵机脉冲：抓夹--PA8、物料转盘--PA11
Usart2：    PA2--TX    PA3--RX     （下位机）
Usart3：    PB10--TX   PB11--RX    （二维码）
Uart4：     PC10-TX    PC11-RX    （蓝牙）
注意事项：发送到串口4的数据需以\r\n（回车）为结尾
-----------------------------------------------
下位机通信表：帧头：0x5E（^）       帧尾：0x5F（_）
-------发送-------------发送-------------发送---
//0x42：去原料区（B）--------------------------- GoRawArea
//0x43：去粗加工区红圆环（C）------------------- GoRoughRED
//0x44：去粗加工区绿圆环（D）------------------- GoRoughGREEN
//0x45：去粗加工区蓝圆环（E）------------------- GoRoughBLUE
//0x4c：去暂存区红圆环（L）--------------------- GoStoreRED
//0x4d：去暂存区绿圆环（M）--------------------- GoStoreGREEN
//0x4e：去暂存区蓝圆环（N）--------------------- GoStoreBLUE
//0x4a：暂存区到粗加工区红圆环（J）------------- StoreGoRough
//0x4b：粗加工到原料区（K）--------------------- RoughGoRaw
//0x46：返回起点（F）--------------------------- RestartS
//0x50：允许底盘进行校准（P）------------------- Calibration
//0x4F：收到底盘发送的消息(O)------------------- ReceCarSent（弃用）
--上位机从一个区到下一个区，直接去任务码位值1对应的圆环到达后，
--双方通信进行色环末端校正,随后上位机根据二维码任务顺序，发送去各圆环处的指令，
--每次到达后，下位机都发  "^@_"  ，上位机接受到后，再发下一个圆环处。
--------接收------------接收-------------接收---
//0x40：底盘已到达指定位置（@）------------------- ArriveLocation
//0x47：下位机确认收到指令（G）------------------- ReceiveMessage（弃用）
//0x48：举臂扫描二维码（H）----------------------- RaiseArmQRCode
//0x49：运行底盘校准（I）------------------------- CarCalibration
//0x72：检测到红色色块（r）----------------------- MaterialRED
//0x67：检测到绿色色块（g）----------------------- MaterialGREEN
//0x62：检测到绿色色块（b）----------------------- MaterialBLUE
注意：取物料完毕，接着还是用原来的指令依次去粗加工区，暂存区
-----------------------------------------------*/

rt_mq_t Usart2_list = RT_NULL;                          //定义串口2队列控制块
rt_mq_t Usart3_list = RT_NULL;                          //定义串口3队列控制块
rt_mq_t Uart4_list = RT_NULL;                           //定义串口4队列控制块

rt_mutex_t Action_mux = RT_NULL;                        //定义互斥量控制块

rt_event_t Usart_event = RT_NULL;                       //定义串口事件控制块
rt_event_t LcdShow_event = RT_NULL;                     //定义LCD显示事件控制块

rt_timer_t Led_WDG_timer = RT_NULL;                     //定义LED与WDG定时器

rt_mailbox_t SendOrder_mail = RT_NULL;                  //定义发送指令邮箱控制块
rt_mailbox_t KeyValue_mail = RT_NULL;                   //定义按键值邮箱控制块

rt_thread_t MainCtrl_thread = RT_NULL;                  //主控制线程控制块指针
rt_thread_t UsartRece_thread = RT_NULL;                 //串口接收参数线程控制块指针
rt_thread_t LedKeyCtrl_thread = RT_NULL;                //按键控制线程控制块指针
rt_thread_t LcdNormal_thread = RT_NULL;                 //LCD屏幕正常刷新线程控制块指针
rt_thread_t LcdDebug_thread = RT_NULL;                  //LCD屏幕调试刷新线程控制块指针

/**************************************************************************
函数功能：系统main函数
入口参数：无
返回  值：无
**************************************************************************/
int main(void)
{
    //LED与WDG软件定时器
    Led_WDG_timer = rt_timer_create("Led_WDG_callback",     //软件定时器的名称
                                    Led_WDG_callback,       //软件定时器的超时函数
                                    RT_NULL,                //定时器超时函数的入口参数
                                    200,                    //软件定时器的超时时间(周期超时时间)
                                    RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
                                                            //软件定时器模式 周期模式
    if (Led_WDG_timer != RT_NULL)
    {
        rt_timer_start(Led_WDG_timer);
    }

    Action_mux = rt_mutex_create("Action_mux",
                                 RT_IPC_FLAG_PRIO);         //创建一个互斥量
    
    //主控制参数（线程）
    MainCtrl_thread =
        rt_thread_create("MainCtrl",                        //线程名字
                         MainCtrl_thread_entry,             //线程入口函数
                         RT_NULL,                           //线程入口参数
                         512,                               //线程栈大小（字节）
                         0,                                 //线程的优先级（数值越小优先级越高）
                         120);                              //线程时间片（线程一次调度能运行的最大时长）

    //串口接收参数（线程）
    UsartRece_thread =
        rt_thread_create("UsartRece",                       //线程名字
                         UsartRece_thread_entry,            //线程入口函数
                         RT_NULL,                           //线程入口参数
                         512,                               //线程栈大小（字节）
                         1,                                 //线程的优先级（数值越小优先级越高）
                         100);                              //线程时间片（线程一次调度能运行的最大时长）
    if(UsartRece_thread != RT_NULL && SysValue.InitComplete)
    {
        rt_kprintf("UsartRece thread success\n");
        rt_thread_startup(UsartRece_thread);
    }

    //按键控制参数（线程）
    LedKeyCtrl_thread =
        rt_thread_create("KeyCtrl",                         //线程名字
                         KeyCtrl_thread_entry,              //线程入口函数
                         RT_NULL,                           //线程入口参数
                         256,                               //线程栈大小（字节）
                         2,                                 //线程的优先级（数值越小优先级越高）
                         20);                               //线程时间片（线程一次调度能运行的最大时长）
    if(LedKeyCtrl_thread != RT_NULL && SysValue.InitComplete)
    {
        rt_kprintf("LedKeyCtrl thread success\n");
        rt_thread_startup(LedKeyCtrl_thread);               //启动线程，开启调度
    }

    //LCD屏幕正常刷新（线程）
    LcdNormal_thread =
        rt_thread_create("LcdRefresh",                      //线程名字
                         LcdNormal_thread_entry,            //线程入口函数
                         RT_NULL,                           //线程参数（无）
                         256,                               //线程栈大小（字节）
                         3,                                 //线程的优先级（数值越小优先级越高）
                         100);                              //线程时间片（线程一次调度能运行的最大时长）

    //LCD屏幕调试刷新（线程）
    LcdDebug_thread =
        rt_thread_create("LcdDebug",                        //线程名字
                         LcdDebug_thread_entry,             //线程入口函数
                         RT_NULL,                           //线程参数（无）
                         256,                               //线程栈大小（字节）
                         3,                                 //线程的优先级（数值越小优先级越高）
                         100);                              //线程时间片（线程一次调度能运行的最大时长）
    if(LcdDebug_thread != RT_NULL && SysValue.DebugStart && SysValue.InitComplete)
    {
        rt_kprintf("LcdDebug thread success\n");
        rt_thread_startup(LcdDebug_thread);                 //开始LCD调试刷新线程
    }
}

