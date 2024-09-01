#include "Display_Thread.h"

// Hardware
#include "Usart.h"

// Bsp
#include "LcdInit.h"
#include "LcdImage.h"


/**************************************************************************
函数功能：LCD屏幕正常刷新线程函数
入口参数：无
返回  值：无
注意事项：无
**************************************************************************/
void LcdNormal_thread_entry(void* parameter)
{
    rt_uint32_t RecvedEvent;
    
    while(true)
    {
        //等待接收事件标志
        rt_event_recv(LcdShow_event,                                            //事件对象句柄
                      LCD_START_EVENT|LCD_CODES_EVENT|                          //接收线程感兴趣的事件（LCD显示）
                      LCD_HELLO_EVENT|LCD_ERROR_EVENT,                          //判断事件与线程所需是否匹配
                      RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,                   //接收完成时清除对应事件的标志位
                      RT_WAITING_FOREVER,                                       //指定超时事件,一直等
                      &RecvedEvent);                                            //指向接收到的事件
        
        #if defined(DEBUG_THREAD_CONSUME_TIME)
            rt_tick_t StartTick = rt_tick_get();
        #endif

        #if defined(DEBUG_THREAD_INTERVAL_TIME)
            static rt_tick_t NowTick,OldTick;
            NowTick = rt_tick_get(); 
            rt_kprintf("LcdNormal_Interval:%d\n",NowTick-OldTick);
            OldTick = NowTick;
        #endif
        
        switch(RecvedEvent)
        {
            case LCD_START_EVENT:ScreenDisplayKeyState("Start Run!");break;     //开始运行
            case LCD_CODES_EVENT:ScreenDisplayTaskCode(SysTask.CodeS);break;    //屏幕显示任务码
            case LCD_HELLO_EVENT:ScreenDisplayKeyState("Hello World!");break;   //欢迎
            case LCD_ERROR_EVENT:ScreenDisplayKeyState("System Error!");break;  //报错
            default:break;
        }

        #if defined(DEBUG_THREAD_CONSUME_TIME)
            rt_kprintf("LcdNormal_Consume:%d\n",rt_tick_get()-StartTick);
        #endif
    }
}

/**************************************************************************
函数功能：LCD屏幕调试刷新线程函数
入口参数：无
返回  值：无
注意事项：无
**************************************************************************/
void LcdDebug_thread_entry(void* parameter)
{
    u8 KeyValue;
    char *OrderStr;
    bool TakeCoseFlag = true;

    LCD_Clear();
    LCD_SetAsciiFont(&ASCII_Font24);
    LCD_DisplayString(78,5,"*Debug*");          //显示当前为Debug模式
    LCD_DisplayString(5,35, "SendCar:");        //给下位机发送的指令
    LCD_DisplayString(5,65, "ReceCar:");        //收到下位机的数据
    LCD_DisplayString(5,95, "QRcodes:");        //收到二维码的数据
    LCD_DisplayString(5,125,"OldKeys:");        //按键旧值
    LCD_DisplayString(5,155,"NowTask:");        //当前状态
    LCD_DisplayString(5,185,"NowCode:");        //本次任务码
    LCD_DisplayString(5,215,"BTvalue:");        //蓝牙指令
    
    LCD_DisplayString(110,35,"NULL");           //给下位机发送指令的初始值
    LCD_DisplayString(110,65,"NULL");           //下位机数据初始值
    LCD_DisplayString(110,95,"NULL");           //二维码数据初始值
    LCD_DisplayString(110,125,KeyString[0]);    //按键初始值
    LCD_DisplayString(110,155,TaskString[0]);   //状态初始值
    LCD_DisplayString(110,185,"NULL");          //任务码初始值
    LCD_DisplayString(110,215,"NULL");          //蓝牙指令初始值
    
    while(true)
    {
        #if defined(DEBUG_THREAD_CONSUME_TIME)
            rt_tick_t StartTick = rt_tick_get();
        #endif

        #if defined(DEBUG_THREAD_INTERVAL_TIME)
            static rt_tick_t NowTick,OldTick;
            NowTick = rt_tick_get(); 
            rt_kprintf("LcdDebug_Interval:%d\n",NowTick-OldTick);
            OldTick = NowTick;
        #endif

        //给下位机发送的指令
        rt_err_t OrderMail = rt_mb_recv(SendOrder_mail,             //邮箱对象句柄
                                        (rt_uint32_t*)&OrderStr,    //接收邮箱消息
                                        RT_WAITING_NO);             //指定超时事件,不等待
        if(RT_EOK == OrderMail)
        {
            LCD_ClearRect(110,35,50,25);
            LCD_DisplayString(110,35,OrderStr);
        }
        //收到下位机的数据
        if(U2Para.State)
        {
            U2Para.State = false;                                   //串口接收标志位
            LCD_ClearRect(110,65,50,25);
            LCD_DisplayChar(110,65,SysValue.ReceCarSta);
        }
        //收到二维码的数据
        if(U3Para.State)
        {
            U3Para.State = false;                                   //串口接收标志位
            LCD_ClearRect(110,95,132,25);
            LCD_DisplayString(110,95,(char*)U3Buf.ShowBuf);
        }
        //按键值
        rt_err_t KeyMail = rt_mb_recv(KeyValue_mail,                //邮箱对象句柄
                                      (rt_uint32_t*)&KeyValue,      //接收邮箱消息
                                      RT_WAITING_NO);               //指定超时事件,不等待
        if(RT_EOK == KeyMail)
        {
            LCD_ClearRect(110,125,90,25);
            LCD_DisplayString(110,125,KeyString[KeyValue]);
        }
        //当前状态
        if(SysTask.NowTask != SysTask.OldShow)
        {
            LCD_ClearRect(110,155,110,25);
            LCD_DisplayString(110,155,TaskString[SysTask.NowTask]); 
        }
        //本次任务码
        if(SysTask.CodeS != NULL && TakeCoseFlag)
        {
            TakeCoseFlag = false;
            LCD_ClearRect(110,185,110,25);
            LCD_DisplayNumber(110,185,SysTask.CodeS,6);
        }
        //收到蓝牙指令
        if(U4Para.State)
        {
            U4Para.State = false;                                   //串口接收标志位
            LCD_ClearRect(110,215,132,25);
            LCD_DisplayString(110,215,(char*)U4Buf.ShowBuf);
        }
        
        #if defined(DEBUG_THREAD_CONSUME_TIME)
            rt_kprintf("LcdDebug_Consume:%d\n",rt_tick_get()-StartTick);
        #endif
        rt_thread_delay(50);
    }
}


/**************************************************************************
函数功能：屏幕显示任务码
入口参数：TaskCode：任务码
返回  值：无
**************************************************************************/
void ScreenDisplayTaskCode(uint32_t TaskCode)
{
    LCD_Clear();
    switch(TaskCode)
    {
        case 123123:LCD_DrawImage(120,0,120,280,Image_123_240x280);LCD_DrawImage(0,0,120,280,Image_add123_240x280);break;
        case 123132:LCD_DrawImage(120,0,120,280,Image_123_240x280);LCD_DrawImage(0,0,120,280,Image_add132_240x280);break;
        case 123213:LCD_DrawImage(120,0,120,280,Image_123_240x280);LCD_DrawImage(0,0,120,280,Image_add213_240x280);break;
        case 123231:LCD_DrawImage(120,0,120,280,Image_123_240x280);LCD_DrawImage(0,0,120,280,Image_add231_240x280);break;
        case 123312:LCD_DrawImage(120,0,120,280,Image_123_240x280);LCD_DrawImage(0,0,120,280,Image_add312_240x280);break;
        case 123321:LCD_DrawImage(120,0,120,280,Image_123_240x280);LCD_DrawImage(0,0,120,280,Image_add321_240x280);break;
        case 132123:LCD_DrawImage(120,0,120,280,Image_132_240x280);LCD_DrawImage(0,0,120,280,Image_add123_240x280);break;
        case 132132:LCD_DrawImage(120,0,120,280,Image_132_240x280);LCD_DrawImage(0,0,120,280,Image_add132_240x280);break;
        case 132213:LCD_DrawImage(120,0,120,280,Image_132_240x280);LCD_DrawImage(0,0,120,280,Image_add213_240x280);break;
        case 132231:LCD_DrawImage(120,0,120,280,Image_132_240x280);LCD_DrawImage(0,0,120,280,Image_add231_240x280);break;
        case 132312:LCD_DrawImage(120,0,120,280,Image_132_240x280);LCD_DrawImage(0,0,120,280,Image_add312_240x280);break;
        case 132321:LCD_DrawImage(120,0,120,280,Image_132_240x280);LCD_DrawImage(0,0,120,280,Image_add321_240x280);break;
        case 213123:LCD_DrawImage(120,0,120,280,Image_213_240x280);LCD_DrawImage(0,0,120,280,Image_add123_240x280);break;
        case 213132:LCD_DrawImage(120,0,120,280,Image_213_240x280);LCD_DrawImage(0,0,120,280,Image_add132_240x280);break;
        case 213213:LCD_DrawImage(120,0,120,280,Image_213_240x280);LCD_DrawImage(0,0,120,280,Image_add213_240x280);break;
        case 213231:LCD_DrawImage(120,0,120,280,Image_213_240x280);LCD_DrawImage(0,0,120,280,Image_add231_240x280);break;
        case 213312:LCD_DrawImage(120,0,120,280,Image_213_240x280);LCD_DrawImage(0,0,120,280,Image_add312_240x280);break;
        case 213321:LCD_DrawImage(120,0,120,280,Image_213_240x280);LCD_DrawImage(0,0,120,280,Image_add321_240x280);break;
        case 231123:LCD_DrawImage(120,0,120,280,Image_231_240x280);LCD_DrawImage(0,0,120,280,Image_add123_240x280);break;
        case 231132:LCD_DrawImage(120,0,120,280,Image_231_240x280);LCD_DrawImage(0,0,120,280,Image_add132_240x280);break;
        case 231213:LCD_DrawImage(120,0,120,280,Image_231_240x280);LCD_DrawImage(0,0,120,280,Image_add213_240x280);break;
        case 231231:LCD_DrawImage(120,0,120,280,Image_231_240x280);LCD_DrawImage(0,0,120,280,Image_add231_240x280);break;
        case 231312:LCD_DrawImage(120,0,120,280,Image_231_240x280);LCD_DrawImage(0,0,120,280,Image_add312_240x280);break;
        case 231321:LCD_DrawImage(120,0,120,280,Image_231_240x280);LCD_DrawImage(0,0,120,280,Image_add321_240x280);break;
        case 312123:LCD_DrawImage(120,0,120,280,Image_312_240x280);LCD_DrawImage(0,0,120,280,Image_add123_240x280);break;
        case 312132:LCD_DrawImage(120,0,120,280,Image_312_240x280);LCD_DrawImage(0,0,120,280,Image_add132_240x280);break;
        case 312213:LCD_DrawImage(120,0,120,280,Image_312_240x280);LCD_DrawImage(0,0,120,280,Image_add213_240x280);break;
        case 312231:LCD_DrawImage(120,0,120,280,Image_312_240x280);LCD_DrawImage(0,0,120,280,Image_add231_240x280);break;
        case 312312:LCD_DrawImage(120,0,120,280,Image_312_240x280);LCD_DrawImage(0,0,120,280,Image_add312_240x280);break;
        case 312321:LCD_DrawImage(120,0,120,280,Image_312_240x280);LCD_DrawImage(0,0,120,280,Image_add321_240x280);break;
        case 321123:LCD_DrawImage(120,0,120,280,Image_321_240x280);LCD_DrawImage(0,0,120,280,Image_add123_240x280);break;
        case 321132:LCD_DrawImage(120,0,120,280,Image_321_240x280);LCD_DrawImage(0,0,120,280,Image_add132_240x280);break;
        case 321213:LCD_DrawImage(120,0,120,280,Image_321_240x280);LCD_DrawImage(0,0,120,280,Image_add213_240x280);break;
        case 321231:LCD_DrawImage(120,0,120,280,Image_321_240x280);LCD_DrawImage(0,0,120,280,Image_add231_240x280);break;
        case 321312:LCD_DrawImage(120,0,120,280,Image_321_240x280);LCD_DrawImage(0,0,120,280,Image_add312_240x280);break;
        case 321321:LCD_DrawImage(120,0,120,280,Image_321_240x280);LCD_DrawImage(0,0,120,280,Image_add321_240x280);break;
    }
}

/**************************************************************************
函数功能：屏幕显示图片
入口参数: pImage：图片
返回  值：无
**************************************************************************/
void ScreenDisplayPicture(const uint8_t *pImage)
{
    LCD_Clear();
    LCD_DrawImage(0,0,240,280,pImage);
}

/**************************************************************************
函数功能：屏幕显示系统关键状态
入口参数: KeyText：文字
返回  值：无
**************************************************************************/
void ScreenDisplayKeyState(char *KeyText)
{
    LCD_Clear();
    LCD_SetColor(LCD_WHITE);
    LCD_SetAsciiFont(&ASCII_Font32);
    LCD_DisplayString(10,120,KeyText);
}
