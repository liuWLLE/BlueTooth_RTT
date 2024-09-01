#include "Arm_Thread.h"

// Bsp
#include "Steering.h"
#include "Motor.h"
#include "Led.h"

// App
#include "Transfer_thread.h"


/**************************************************************************
函数功能：主控制线程函数
入口参数：无 
返回  值：无
注意事项：无
**************************************************************************/
void MainCtrl_thread_entry(void* parameter)
{
    //LCD显示（事件）
    LcdShow_event = rt_event_create("LcdShow_event",                    //事件标志组名字
                                    RT_IPC_FLAG_PRIO);                  //事件为优先级模式
    while(true)
    {
        #if defined(DEBUG_THREAD_CONSUME_TIME)
            rt_tick_t StartTick = rt_tick_get();
        #endif

        #if defined(DEBUG_THREAD_INTERVAL_TIME)
            static rt_tick_t NowTick,OldTick;
            NowTick = rt_tick_get(); 
            rt_kprintf("MainCtrl_Interval:%d\n",NowTick-OldTick);
            OldTick = NowTick;
        #endif
        
        switch(SysTask.NowTask)
        {
            case TaskStart:
                {
                    LaserLed_OFF;                                       //关闭激光
                    rt_event_send(LcdShow_event,LCD_START_EVENT);       //发送LCD显示事件
                    SendOrderToCar(GoRawAreaS,2,40);                    //发送指令

                    SysTask.OldShow = SysTask.NowTask;
                    SysTask.NowTask = ScanQR;
                }break;
            case ScanQR:
                {
                    if(SysTask.CodeS != NULL)
                    {
                        rt_event_send(LcdShow_event,LCD_CODES_EVENT);   //发送LCD显示事件
                        rt_mutex_take(Action_mux,RT_WAITING_FOREVER);   //获取互斥量（一直等）

                        ArmClipTakeState(Avoid,false);                  //抓夹松开
                        MotorMotion_Rotate(Vision,false);               //旋转电机转向
                        MotorMotion_Lift(Raw,false);                    //升降电机降原料区识别 

                        rt_mutex_release(Action_mux);                   //释放互斥量

                        SysTask.OldShow = SysTask.NowTask;
                        SysTask.NowTask = TakeRaw1;
                    }
                }break;
            case TakeRaw1:
                {
                    if(SysValue.ReceCarSta == ArriveLocation)
                    {
                        SysValue.ReceCarSta = NULL;
                        rt_mutex_take(Action_mux,RT_WAITING_FOREVER);                   //获取互斥量（一直等）
                        
                        IdentifyColorTake(true,SysTask.Code1/100);                      //识别原料区物料
                        IdentifyColorTake(false,SysTask.Code1/10%10);       
                        IdentifyColorTake(false,SysTask.Code1%10);      
            
                        rt_thread_delay(1000);                                          //延时后机械臂旋出
                        rt_thread_delay(1000);             
                        rt_thread_delay(1000);             
                        MotorMotion_Rotate(Vision,false);                               //旋转电机旋转（准备校准）
                        ArmClipTakeState(Avoid,false);                                  //抓夹稍微夹紧避免碰撞
                        MotorMotion_Lift(Aim,false);                                    //降校准
                        MaterialPosition((ColorSele)(SysTask.Code1/100),false);         //车上盘提前旋转至所需
                        
                        rt_mutex_release(Action_mux);                                   //释放互斥量
                        
                        SysTask.OldShow = SysTask.OldTask = SysTask.NowTask;
                        SysTask.NowTask = Calibrat;    
                    }
                }break;
            case GoRough:
                {
                    if(SysValue.ReceCarSta == ArriveLocation)
                    {
                        SysValue.ReceCarSta = NULL;
                        rt_mutex_take(Action_mux,RT_WAITING_FOREVER);                   //获取互斥量（一直等）
                        
                        MotorMotion_Lift(Reset_L,false);                                //升降电机复位
                        MotorMotion_Rotate(Reset_R,false);                              //旋转电机复位
                        
                        rt_mutex_release(Action_mux);                                   //释放互斥量
                        
                        SysTask.OldShow = SysTask.NowTask;
                        SysTask.NowTask = TPRough1;
                    }
                }break;
            case TPRough1:
                {
                    rt_mutex_take(Action_mux,RT_WAITING_FOREVER);                       //获取互斥量（一直等）
                    
                    Ground_PutAction(Normal,(ColorSele)(SysTask.Code1/10%10),true);     //粗加工区放置物料
                    Ground_PutAction(Normal,(ColorSele)(SysTask.Code1%10),true);
                    Ground_PutAction(Normal,(ColorSele)(SysTask.Code1/100),false);
    
                    Ground_TakeAction((ColorSele)(SysTask.Code1/10%10));                //粗加工区拿起物料
                    Ground_TakeAction((ColorSele)(SysTask.Code1%10));
                    Ground_TakeAction((ColorSele)(SysTask.Code1/100));                  //自带旋转电机旋转    
    
                    ArmClipTakeState(Avoid,false);                                      //抓夹稍微夹紧避免碰撞
                    MotorMotion_Lift(Aim,false);                                        //降校准 

                    rt_mutex_release(Action_mux);                                       //释放互斥量
                    
                    SysTask.OldShow = SysTask.OldTask = SysTask.NowTask;
                    SysTask.NowTask = Calibrat;
                }break;
            case GoStaging:
                {
                    if(SysValue.ReceCarSta == ArriveLocation)
                    {
                        SysValue.ReceCarSta = NULL;
                        SendRingToCar(Staging,Wait,SysTask.Code1/100,true);
                        
                        SysTask.OldShow = SysTask.NowTask;
                        SysTask.NowTask = PutStag1;
                    }
                }break;
            case PutStag1:
                {
                    rt_mutex_take(Action_mux,RT_WAITING_FOREVER);                       //获取互斥量（一直等）

                    Ground_PutAction(Normal,(ColorSele)(SysTask.Code1/10%10),true);     //暂存区放置物料
                    Ground_PutAction(Normal,(ColorSele)(SysTask.Code1%10),true);
                    Ground_PutAction(Normal,(ColorSele)(SysTask.Code2/100),false);      //结束后去拿物料
                    MotorMotion_Lift(Aim,false);                                        //降校准 

                    rt_mutex_release(Action_mux);                                       //释放互斥量
                    
                    SysTask.OldShow = SysTask.OldTask = SysTask.NowTask;
                    SysTask.NowTask = Calibrat;
                }break;
            case PassRough:
                {
                    if(SysValue.ReceCarSta == ArriveLocation)
                    {
                        SysValue.ReceCarSta = NULL;
                        rt_mutex_take(Action_mux,RT_WAITING_FOREVER);               //获取互斥量（一直等）

                        SendOrderToCar(RoughGoRaw,5,40);                            //暂存区返回原料区    
                        MotorMotion_Lift(Raw,false);                                //升降电机降原料区

                        rt_mutex_release(Action_mux);                               //释放互斥量

                        SysTask.OldShow = SysTask.NowTask;
                        SysTask.NowTask = TakeRaw2;
                    }
                }break;
            case TakeRaw2:
                {
                    if(SysValue.ReceCarSta == ArriveLocation)
                    {
                        SysValue.ReceCarSta = NULL;
                        rt_mutex_take(Action_mux,RT_WAITING_FOREVER);               //获取互斥量（一直等）

                        IdentifyColorTake(true,SysTask.Code2/100);                  //识别原料区物料
                        IdentifyColorTake(false,SysTask.Code2/10%10);
                        IdentifyColorTake(false,SysTask.Code2%10);

                        rt_thread_delay(1000);                                      //延时后机械臂旋出
                        rt_thread_delay(1000);
                        rt_thread_delay(1000);
                        MotorMotion_Rotate(Vision,false);                           //旋转电机旋转（准备校准）
                        ArmClipTakeState(Avoid,false);                              //抓夹稍微夹紧避免碰撞
                        MotorMotion_Lift(Aim,false);                                //降校准
                        MaterialPosition((ColorSele)(SysTask.Code2/100),false);     //车上盘提前旋转至所需

                        rt_mutex_release(Action_mux);                               //释放互斥量

                        SysTask.OldShow = SysTask.OldTask = SysTask.NowTask;
                        SysTask.NowTask = Calibrat;
                    }
                }break;
            case GoRough2:
                {
                    if(SysValue.ReceCarSta == ArriveLocation)
                    {
                        SysValue.ReceCarSta = NULL;
                        rt_mutex_take(Action_mux,RT_WAITING_FOREVER);               //获取互斥量（一直等）
                        
                        MotorMotion_Lift(Reset_L,false);                            // 升降电机复位
                        MotorMotion_Rotate(Reset_R,false);                          // 旋转电机复位
                        
                        rt_mutex_release(Action_mux);                               //释放互斥量
                        
                        SysTask.OldShow = SysTask.NowTask;
                        SysTask.NowTask = TPRough2;
                    }
                }break;
            case TPRough2:
                {
                    rt_mutex_take(Action_mux,RT_WAITING_FOREVER);                       //获取互斥量（一直等）

                    Ground_PutAction(Normal,(ColorSele)(SysTask.Code2/10%10),true);     //粗加工区再次放物料
                    Ground_PutAction(Normal,(ColorSele)(SysTask.Code2%10),true);
                    Ground_PutAction(Normal,(ColorSele)(SysTask.Code2/100),false);
    
                    Ground_TakeAction((ColorSele)(SysTask.Code2/10%10));                //粗加工区再次拿物料
                    Ground_TakeAction((ColorSele)(SysTask.Code2%10));    
                    Ground_TakeAction((ColorSele)(SysTask.Code2/100));
    
                    ArmClipTakeState(Avoid,false);                                      //抓夹稍微夹紧避免碰撞
                    MotorMotion_Lift(Pile,false);                                       //降码垛校准 
                    
                    rt_mutex_release(Action_mux);                                       //释放互斥量
                    
                    SysTask.OldShow = SysTask.OldTask = SysTask.NowTask;
                    SysTask.NowTask = Calibrat;
                }break;
            case GoStag2:
                {
                    if(SysValue.ReceCarSta == ArriveLocation)
                    {
                        SysValue.ReceCarSta = NULL;
                        SendRingToCar(Staging,Wait,SysTask.Code2/100,true);
                        
                        SysTask.OldShow = SysTask.NowTask;
                        SysTask.NowTask = PutStag2;
                    }
                }break;
            case PutStag2:
                {
                    rt_mutex_take(Action_mux,RT_WAITING_FOREVER);                       //获取互斥量（一直等）
                    
                    Ground_PutAction(StackUp,(ColorSele)(SysTask.Code2/10%10),true);    //暂存区码垛    
                    Ground_PutAction(StackUp,(ColorSele)(SysTask.Code2%10),true);
                    Ground_PutAction(StackUp,NoColor,true);
                    
                    rt_mutex_release(Action_mux);                                       //释放互斥量
                    
                    SysTask.OldShow = SysTask.NowTask;
                    SysTask.NowTask = Restart;
                }break;
            case Restart:
                {
                    if(SysValue.ReceCarSta == ArriveLocation)
                    {
                        SysValue.ReceCarSta = NULL;
                        rt_event_send(LcdShow_event,LCD_HELLO_EVENT);                   //发送LCD显示事件
                        
                        SysTask.OldShow = SysTask.NowTask;
                        SysTask.NowTask = TaskStart;
                        SysValue.TaskStart = false;
                    }
                }break;
            case Calibrat:                                                              //色环校准
                {
                    if(SysValue.ReceCarSta == CarCalibration)
                    {
                        SysValue.ReceCarSta = NULL;
                        SendOrderToCar(Calibration,2,40);
                        switch(SysTask.OldTask)
                        {
                            case TakeRaw1:SysTask.NowTask = GoRough;break;
                            case TPRough1:SysTask.NowTask = GoStaging;break;
                            case PutStag1:SysTask.NowTask = PassRough;break;
                            case TakeRaw2:SysTask.NowTask = GoRough2;break;
                            case TPRough2:SysTask.NowTask = GoStag2;break;
                            default:rt_event_send(LcdShow_event,LCD_ERROR_EVENT);break;
                        }
                    }
                }break;
            default:rt_event_send(LcdShow_event,LCD_ERROR_EVENT);break;
        }
        
        #if defined(DEBUG_THREAD_CONSUME_TIME)
            rt_kprintf("MainCtrl_Consume:%d\n",rt_tick_get()-StartTick);
        #endif
        rt_thread_delay(30);
    }
}


/**************************************************************************
函数功能：从物料盘拿取并放置物料动作组
入口参数：AloneCode：任务码位值、color：任务码对应的颜色、Rotate：是否让旋转电机复位
返回  值：无
**************************************************************************/
void Ground_PutAction(OrderPlace PlaceState,ColorSele color,bool Rotate)
{
    static char PutCount;
    PutCount++; 
    MotorMotion_Lift(Stuff,false);          //升降电机下降至车上托盘
    ArmClipTakeState(Grasp,true);           //抓夹夹紧
    MotorMotion_Lift(Reset_L,false);        //升降复位
    MotorMotion_Rotate(Vision,true);        //旋转电机转向
    if(PlaceState == Normal)
    {
        MotorMotion_Lift(Places,false);     //升降触底
    }
    else if(PlaceState == StackUp)
    {
        MotorMotion_Lift(Stack,false);      //升降码垛
    }
    Delay_ms(400);
    ArmClipTakeState(Avoid,true);           //抓夹松开
    MaterialPosition(color,false);          //物料托盘提前旋转至下一个颜色所需位置
    if(PutCount < 3)
    {
        switch(SysTask.NowTask)                                                 //第一、二次抓取物料放置
        {
            case TPRough1:SendRingToCar(Rough,Wait,(int)color,false);  break;   //粗加工区内移动至下一个色环
            case PutStag1:SendRingToCar(Staging,Wait,(int)color,false);break;   //暂存区内移动至下一个色环
            case TPRough2:SendRingToCar(Rough,Wait,(int)color,false);  break;   //粗加工区内移动至下一个色环
            case PutStag2:
                    {
                          MotorMotion_Lift(Pile,false);                         //升降码垛校准
                          SendRingToCar(Staging,Wait,(int)color,false);         //暂存区内移动至下一个色环
                    }break;
        }
        MotorMotion_Lift(Reset_L,false);                                        //升降电机复位
    }
    else
    {
        switch(SysTask.NowTask)                                                 //第三次抓取物料放置
        {
            case TPRough1:SendRingToCar(Rough,Wait,(int)color,false);break;     //粗加工区内移动至下一个色环
            case PutStag1:SendOrderToCar(StoreGoRough,5,40);break;              //前往粗加工区
            case TPRough2:SendRingToCar(Rough,Wait,(int)color,false);break;     //粗加工区内移动至下一个色环
            case PutStag2:                                                      //码垛完成
                    {
                          SendOrderToCar(RestartS,5,40);                        //返回起点
                          MotorMotion_Lift(Reset_L,false);                      //升降电机复位
                    }break;
        }
        PutCount = 0;
    }
    if(Rotate)
    {
        MotorMotion_Rotate(Reset_R,false);                                      //旋转电机是否复位
    }
}

/**************************************************************************
函数功能：从地上拿取物料并放置在物料盘动作组
入口参数：AloneCode：任务码位值
返回  值：无
**************************************************************************/
void Ground_TakeAction(ColorSele color)
{
    static char TakeCount;
    TakeCount++;
    ArmClipTakeState(Avoid,false);          // 抓夹稍微夹紧避免碰撞
    MotorMotion_Lift(Grab,false);           // 升降电机触底
    ArmClipTakeState(Grasp,true);           // 抓夹夹紧
    MotorMotion_Lift(Reset_L,false);        // 升降电机复位
    MotorMotion_Rotate(Reset_R,true);       // 旋转电机复位
    MotorMotion_Lift(Stuff,true);           // 升降电机下降至车上托盘
    ArmClipTakeState(Release,true);         // 抓夹松开(松开得足够大，升降电机不需要复位)
    if(SysTask.NowTask == TPRough1)
    {
        if(TakeCount < 3)
        {
            SendRingToCar(Rough,Jump,(int)color,false);
        }
        else
        {
            SendRingToCar(Staging,Jump,(int)color,false);   //移动至暂存区
            TakeCount = 0;
        }
    }
    else if(SysTask.NowTask == TPRough2)
    {
        if(TakeCount < 3)
        {
            SendRingToCar(Rough,Jump,(int)color,false);
        }
        else
        {
            SendRingToCar(Staging,Jump,(int)color,false);   //移动至暂存区
            TakeCount = 0;
        }
    }
    MaterialPosition(color,false);              // 物料托盘提前旋转至所需要
    MotorMotion_Rotate(Vision,false);           // 旋转电机旋转
}

/**************************************************************************
函数功能：原料区拿取物料动作组
入口参数：NeedColor：所需物料的颜色 
返回  值：无
**************************************************************************/
void TakeRawMaterial(void) 
{
    static char ActionCount;
    MotorMotion_Lift(RawArea,false);        // 升降电机降至原料盘
    ArmClipTakeState(Grasp,true);           // 抓夹夹紧
    ActionCount++;
    MotorMotion_Lift(Reset_L,false);        // 升降电机复位
    MotorMotion_Rotate(Reset_R,true);       // 旋转电机复位
    MotorMotion_Lift(Stuff,false);          // 升降电机下降至托盘
    ArmClipTakeState(Release,true);         // 抓夹松开
    if(ActionCount >= 3)
    {
        if(SysTask.NowTask == TakeRaw1)
        {
            SendRingToCar(Rough,Jump,SysTask.Code1/100,false);  //第一次去粗加工区
        }
        else if(SysTask.NowTask == TakeRaw2)
        {
            SendRingToCar(Rough,Jump,SysTask.Code2/100,false);  //第二次去粗加工区
        }
    }
    if(ActionCount < 3)
    {
        MotorMotion_Rotate(Vision,false);   // 旋转电机旋转
        MotorMotion_Lift(RawArea,true);     // 升降电机降至原料盘
    }
    else ActionCount = 0;                   // 避免拿完后还举起来等待
}

/**************************************************************************
函数功能：识别并拿取对应颜色的物料
入口参数：AloneCode：任务码位值
返回  值：无
注意事项：为了跳过刚举臂识别颜色时的第一个物料，则让识别到的颜色需与上一次颜色值不同才能夹取，
如此以来不管第一个物料是什么颜色，都不会被夹取，此外还需要将刚举臂识别到的第一种颜色设置代表
为上一次颜色值变量的初值
调试方案：
//      //颜色抓取
//      LCD_ClearRect(167,120,240,32);
//      LCD_SetAsciiFont(&ASCII_Font32);
//      LCD_DisplayString(5,120,"ColorTake:");
//      LCD_DisplayChar(170,120,ColorCode);
**************************************************************************/
void IdentifyColorTake(bool Update,char AloneCode)
{ 
    bool UpdateOld = false;
    ColorSele ColorCode = NoColor;                      //当前时刻看到的颜色
    static ColorSele OldValue = NoColor;                //颜色旧值
    if(!SysValue.DirectGrab)
    {
        MaterialPosition((ColorSele)AloneCode,false);   //物料托盘提前旋转    
    }
    SysValue.ReceCarSta = NULL;                         //清空避免旧值误判
    if(Update)
    {
        UpdateOld = true;                               //第一次抓取更新颜色旧值
    }
    while(true)
    {
        switch(SysValue.ReceCarSta)
        {
            case MaterialRED:   ColorCode = Red_1;  break;
            case MaterialGREEN: ColorCode = Green_2;break;
            case MaterialBLUE:  ColorCode = Blue_3; break;
            default:            ColorCode = NoColor;break;
        }
        if(ColorCode != NoColor)                        //检查数字码是否正确
        {       
            if(UpdateOld)                               //如果是第一次抓取
            {       
                UpdateOld = false;                      //在循环中仅更新一次
                OldValue = ColorCode;                   //将第一次看到的颜色设置上一次颜色变量初始值
            }       
            else        
            {       
                if(SysValue.DirectGrab)                 //若不看颜色直接拿
                {       
                    if(ColorCode != OldValue)           //颜色与旧值不相同
                    {       
                        OldValue = ColorCode;           //更新颜色旧值
                        MaterialPosition((ColorSele)ColorCode,false);
                        Delay_ms(800);                  //等物料旋转至合适位置
                        TakeRawMaterial();              //直到拿起物料后跳出
                        break;      
                    }       
                }       
                else                                    //若检查颜色后才抓取
                {       
                    if(Update)                          //抓取第一个物料
                    {
                        if(ColorCode!=OldValue && ColorCode==AloneCode)
                        {
                            Delay_ms(500);              //颜色与旧值不相同且颜色码与任务码其一位值对应
                            Delay_ms(200);              //等物料旋转至合适位置
                            TakeRawMaterial();          //直到拿起物料后跳出
                            break;      
                        }       
                        OldValue = ColorCode;           //更新颜色，避免第一颜色是所需要颜色而卡死
                    }       
                    else                                //抓取第二、三个物料
                    {
                        if(ColorCode == AloneCode)      //如果是所需的颜色就直接抓
                        {
                            Delay_ms(700);              //等物料旋转至合适位置
                            TakeRawMaterial();          //直到拿起物料后跳出
                            break;
                        }
                    }
                }
            }
        }
        Delay_ms(10); 
    }
}

