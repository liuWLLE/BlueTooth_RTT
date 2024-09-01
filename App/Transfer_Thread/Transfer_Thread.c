#include "Transfer_thread.h"

// Hardware
#include "Usart.h"

// Middle
#include "CRC.h"

// Bsp
#include "Motor.h"
#include "Iap.h"
#include "Steering.h"

// App
#include "Arm_Thread.h"
#include "Display_Thread.h"


/**************************************************************************
函数功能：串口接收参数线程函数
入口参数：无 
返回  值：无
注意事项：串口接收到消息后通过事件唤醒线程
**************************************************************************/
void UsartRece_thread_entry(void* parameter)
{
    rt_uint32_t RecvedEvent;
    
    //串口接收（事件）
    Usart_event = rt_event_create("Usart",                          //事件标志组名字
                                  RT_IPC_FLAG_PRIO);                //事件为优先级模式

    //串口2传输（队列）     
    Usart2_list = rt_mq_create("Usart2",                            //消息队列名字
                              U2RecLen,                             //消息的最大长度
                              5,                                    //消息队列的最大容量
                              RT_IPC_FLAG_FIFO);                    //队列模式 FIFO
    //串口3传输（队列）     
    Usart3_list = rt_mq_create("U3list",                            //消息队列名字
                              U3RecLen,                             //消息的最大长度
                              5,                                    //消息队列的最大容量
                              RT_IPC_FLAG_FIFO);                    //队列模式 FIFO
    //串口4传输（队列）     
    Uart4_list = rt_mq_create("Uart4",                              //消息队列名字
                              U4RecLen,                             //消息的最大长度
                              5,                                    //消息队列的最大容量
                              RT_IPC_FLAG_FIFO);                    //队列模式 FIFO
                              
    //发送指令（邮箱）
    SendOrder_mail = rt_mb_create("SendMail",                       //邮箱名字
                                  5,                                //邮箱大小
                                  RT_IPC_FLAG_FIFO);                //信号量模式 FIFO
                                 
    while(true)
    {
        //等待接收事件标志
        rt_event_recv(Usart_event,                                  //事件对象句柄
                      USART2_EVENT|USART3_EVENT|                    //接收线程感兴趣的事件（串口2/3/4）
                      USART4_EVENT,                                 //判断事件集合中信息与线程所需信息是否全部匹配
                      RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,       //接收完成时清除对应事件的标志位
                      RT_WAITING_FOREVER,                           //指定超时事件,一直等
                      &RecvedEvent);                                //指向接收到的事件
        
        #if defined(DEBUG_THREAD_CONSUME_TIME)
            rt_tick_t StartTick = rt_tick_get();
        #endif

        #if defined(DEBUG_THREAD_INTERVAL_TIME)
            static rt_tick_t NowTick,OldTick;
            NowTick = rt_tick_get(); 
            rt_kprintf("UsartRece_Interval:%d\n",NowTick-OldTick);
            OldTick = NowTick;
        #endif
        
        switch(RecvedEvent)
        {
            case USART2_EVENT:ReceCarState_Wire();break;
            case USART3_EVENT:ReceTaskCode_Mult();break;
            case USART4_EVENT:MotorSteeringCtrl_BT();break;
            default:break;
        }
        
        #if defined(DEBUG_THREAD_CONSUME_TIME)
            rt_kprintf("UsartRece_Consume:%d\n",rt_tick_get()-StartTick);
        #endif
    }
}

/**************************************************************************
函数功能：接收下位机状态
入口参数：无
返回  值：无
注意事项：1、串口2应用任务
**************************************************************************/
void ReceCarState_Wire(void)
{
    char ReceBuf[U2RecLen];
    
    rt_mq_recv(Usart2_list,                                         //读取队列的句柄
               &ReceBuf,                                            //读取的数据保存位置
               sizeof(ReceBuf),                                     //读取的数据的长度
               RT_WAITING_NO);                                      //等待时间：不等待
            
    if(strlen(ReceBuf) == 3 && SysValue.TaskStart)                  //数据长度正确且任务开始运行
    {
        if(ReceBuf[0] == 0x5E && ReceBuf[2] == 0x5F)                //检查帧头帧尾
        {       
            SysValue.ReceCarSta = ReceBuf[1];                       //获取下位机的状态
        }
    }
}

/**************************************************************************
函数功能：接收任务码
入口参数：无
返回  值：无
注意事项：1、串口3应用任务  2、扫描模块、WiFi模块多种方式
**************************************************************************/
void ReceTaskCode_Mult(void)
{
    char ReceBuf[U3RecLen];

    rt_mq_recv(Usart3_list,                                             //读取队列的句柄
               &ReceBuf,                                                //读取的数据保存位置
               sizeof(ReceBuf),                                         //读取的数据的长度
               RT_WAITING_NO);                                          //等待时间：不等待
    
    if(SysTask.CodeS == NULL && SysValue.TaskStart)                     //如果未获取任务码且开始运行
    {
        u8 CodeSpace = sizeof(TaskCodeStr)/sizeof(TaskCodeStr[0]);      //计算任务码总数量
        ReceBuf[6] = '\0';                                              //取前六位为任务码
        for(char count = 0;count < CodeSpace;count++)                   //遍历匹配任务码
        {
            //strstr(str1,str2);查找str2在str1中的位置，返回其位置
            if(strstr(ReceBuf,TaskCodeStr[count]) != 0)                 //如果查找到任务码
            {
                SendOrderToCar(ReceCarSent,2,40);
                SysTask.CodeS = TaskCodeNum[count];                     //获取完整任务码
                SysTask.Code1 = SysTask.CodeS / 1000;                   //解算任务码
                SysTask.Code2 = SysTask.CodeS % 1000;   
                break;
            }
        }
    }
}

/**************************************************************************
函数功能：蓝牙控制电机与舵机动作
入口参数：无
返回  值：无
注意事项：1、串口4应用任务  2、电机进行任何运动之前尽量先复位
**************************************************************************/
void MotorSteeringCtrl_BT(void)
{
    char ReceBlueState;                                 //收到的蓝牙的数字(-127~128)
    char ReceBuf[U4RecLen];
    
    rt_mq_recv(Uart4_list,                              //读取队列的句柄
               &ReceBuf,                                //读取的数据保存位置
               sizeof(ReceBuf),                         //读取的数据的长度
               RT_WAITING_NO);                          //等待时间：不等待
    
    u8 ReceBlueLength = (u8)strlen(ReceBuf);            //收到的数据长度
    if(ReceBlueLength != 0)
    {
        if(ReceBlueLength < 3)                          //如果数据长度小于3
        {
            u32 HeightNow = ZeroHeight;                 //初始化电机零点高度
            if(ReceBlueLength == 1)                     //数字仅有个位
            {
                ReceBlueState = ReceBuf[0] - 48;
            }
            else if(ReceBlueLength == 2)                //数字有个位和十位
            {
                ReceBlueState = (ReceBuf[0]-48)*10 + (ReceBuf[1]-48);
            }
            rt_mutex_take(Action_mux,RT_WAITING_FOREVER);                   //获取互斥量（一直等）
            switch(ReceBlueState)                                           //选择对应指令
            {
                case 0:MotorMotion_Rotate(Reset_R,false);break;             // 0 旋转电机复位
                case 1:MotorMotion_Rotate(Vision,false);break;              // 1 旋转电机旋转
                case 2:HeightNow = MotorMotion_Lift(Reset_L,false);break;   // 2 升降电机复位
                case 3:HeightNow = MotorMotion_Lift(Grab,false);break;      // 3 升降电机降地面抓取 
                case 4:HeightNow = MotorMotion_Lift(Stack,false);break;     // 4 升降电机降码垛
                case 5:HeightNow = MotorMotion_Lift(RawArea,false);break;   // 5 升降电机降转盘
                case 6:HeightNow = MotorMotion_Lift(Stuff,false);break;     // 6 升降电机降托盘
                case 7:ArmClipTakeState(Release,false);break;               // 7 抓夹松开
                case 8:ArmClipTakeState(Grasp,false);break;                 // 8 抓夹抓紧
                case 9:break; // 9
                case 10:HeightNow = MotorMotion_Lift(Rise,false);break;     // 10 上升50mm 
                case 11:HeightNow = MotorMotion_Lift(Fall,false);break;     // 11 下降50mm 
                case 12:ArmClipTakeState(Avoid,false);break;                // 12 抓夹避撞
                case 13:MotorMotion_Lift(Places,false);break;               // 13 升降电机降地面放置
                case 14:HeightNow = MotorMotion_Lift(Aim,false);break;      // 14 升降电机降校准
                case 15:HeightNow = MotorMotion_Lift(Lowest,false);break;   // 15 升降电机降最低
                case 16:break;
                default:break;
            }
            rt_mutex_release(Action_mux);                                   //释放互斥量
            sprintf((char*)U4Buf.SenBuf,"Height:%d,%d",HeightNow,ReceBlueState);
        }
        else if(ReceBlueLength == 3)                                        //数字最大到128
        {
            ReceBlueState = (ReceBuf[0]-48)*100 + (ReceBuf[1]-48)*10 + (ReceBuf[2]-48);
            if(ReceBlueState == 110)                                        //指定数字110为程序升级指令
            {
                FlashSetFlag_IAP(FLASH_DATA_ADDR,true);                     //写入程序标志位并重启
            }
            else
            {
                sprintf((char*)U4Buf.SenBuf,"%s",ReceBuf);
            }
        }
        else
        {
            sprintf((char*)U4Buf.SenBuf,"%s",ReceBuf);
        }
        sprintf((char*)U4Buf.SenBuf,"%s,%d,",(char*)U4Buf.SenBuf,strlen((char*)U4Buf.SenBuf));  //加上数据位长度
        CRC16_ModifyData((char*)U4Buf.SenBuf,strlen((char*)U4Buf.SenBuf));                      //加上CRC校验值
        DMA_Transfer(UART_SEND_DMA_CHANNEL_4,sizeof(U4Buf.SenBuf));                             //DMA回传数组
    }
}

/**************************************************************************
函数功能：向下位机发送圆环指令
入口参数：str：指令内容  SendCount：发送次数  gap：发送间隔
返回  值：无
**************************************************************************/
void SendOrderToCar(char *str,u8 SendCount,u16 gap)
{
    rt_mb_send(SendOrder_mail,(rt_uint32_t)str);            //发送邮箱消息
    Usart_SendString(SendCarUsart,str,SendCount,gap);       //串口发送函数
}

/**************************************************************************
函数功能：向下位机发送圆环指令
入口参数：Loca：选择粗加工区或暂存区。State：是否需要等待。AloneCode：任务码位值
返回  值：无
**************************************************************************/
void SendRingToCar(OrderLoca Loca,OrderRun Run,u8 Code,bool Init)
{  
    if(Loca == Rough)                                       //去粗加工区
    {
        switch(Code)                                        //根据任务码位值选择发送命令
        {
            case 1:SendOrderToCar(GoRoughRED,8,40);break; 
            case 2:SendOrderToCar(GoRoughGREEN,8,40);break;
            case 3:SendOrderToCar(GoRoughBLUE,8,40);break;
        }
    }
    else if(Loca == Staging)                                //去暂存区
    {
        switch(Code)                                        //根据任务码位值选择发送命令
        {
            case 1:SendOrderToCar(GoStoreRED,8,40);break;
            case 2:SendOrderToCar(GoStoreGREEN,8,40);break;
            case 3:SendOrderToCar(GoStoreBLUE,8,40);break;
        }
    }
    if(Init)
    {
        MotorMotion_Lift(Reset_L,false);                    //升降电机复位
        MotorMotion_Rotate(Reset_R,false);                  //旋转电机旋转
    }       
    if(Run == Wait)                                         //如果需要等待
    {
        while(true)
        {
            Delay_ms(10);                                   //延时等待寄存器值变化
            if(SysValue.ReceCarSta == ArriveLocation)       //底盘到达位置后才跳出
            {
                SysValue.ReceCarSta = NULL;
                break;
            }
        }
    }                                                       //如果不需要等待就直接跳出
}
