#include "Esp8266.h"

//WIFI STA模式无线参数.
static char* WiFiSSID = "光电科技协会";             //路由器SSID 
static char* WiFiKey = "123guangdian123";           //连接密码  
static char* NetMode = "TCP";                       //客户端模式
static char* NetIP = "192.168.1.117";               //目标IP地址
static char* NetPort = "8000";                      //连接端口号

struct                  
{
    u8 DataLength;                                  //数据长度
    bool *ReceState;                                //接收标志位
    char *ReceBuffer;                               //接受数组
    USART_TypeDef *SnedUSARTx;                      //发送串口
    void (*SendBufFunc)(USART_TypeDef *,char *);    //数据发送函数
    void (*ShowStateFunc)(u16,u16,char *);          //信息提示函数
}*Esp_P;                                            //ESP8266方法

/**************************************************************************
函数功能：向ESP8266发送AT指令
入口参数：Cmd：AT指令、Ack：期望应答、Attempts：尝试次数
返回  值：是否成功发送指令
注意事项：如果不需要应答，则发送AT达到尝试次数后自动退出，若需要应答，则接收到期望应答后自动退出
**************************************************************************/
void ESP8266_Init(void (*ShowFunc)(),void (*SendFunc)(),USART_TypeDef *USARTx,char *Buff,bool *State,u8 Length)
{
    Esp_P->ShowStateFunc = ShowFunc;
    Esp_P->SendBufFunc = SendFunc;
    Esp_P->SnedUSARTx = USARTx;
    Esp_P->ReceBuffer = Buff;
    Esp_P->ReceState = State;
    Esp_P->DataLength = Length;
}

/**************************************************************************
函数功能：向ESP8266发送AT指令
入口参数：Cmd：AT指令、Ack：期望应答、Attempts：尝试次数
返回  值：是否成功发送指令
注意事项：如果不需要应答，则发送AT达到尝试次数后自动退出，若需要应答，则接收到期望应答后自动退出
调试方案：
//          if(Task.DebugFlag){
//              LCD_SetAsciiFont(&ASCII_Font32);
//              LCD_ClearRect(87,50,240,32);
//              LCD_DisplayString(5,50,"QR:");
//              LCD_DisplayString(90,50,QRCode.ReceQRData);
//          }
//          if(Task.DebugFlag){
//              LCD_SetAsciiFont(&ASCII_Font32);
//              LCD_ClearRect(87,50,240,32);
//              LCD_DisplayString(5,50,"ESP:");
//              LCD_DisplayString(90,50,QRCode.ReceESP8266);
//          }
**************************************************************************/
static bool SendEsp8266Cmd(char *Cmd,char *Ack,u16 Attempts)
{
    char SenBuf[Esp_P->DataLength];                             //发送缓冲
    Esp_P->ReceState = false;                                   //初始化接收标志位
    sprintf(SenBuf,"%s\r\n",Cmd);                               //添加结束符
    if(Ack == NULL)                                             //如果不需要应答
    {
        while(--Attempts)                                       //预期的尝试次数
        {
            Esp_P->SendBufFunc(Esp_P->SnedUSARTx,SenBuf);       //发送指令
            Delay_ms(100);
        }
        return true;
    }
    else if(Ack != NULL)                                        //如果需要应答
    {
        while(--Attempts)                                       //预期的尝试次数
        {
            Esp_P->SendBufFunc(Esp_P->SnedUSARTx,SenBuf);       //发送指令
            for(char count = 0;count < 5;count++)               //预期的串口消息检索次数
            {
                Delay_ms(20);       
                if(Esp_P->ReceState)                            //接收到消息
                {
                    if(strstr(Esp_P->ReceBuffer,Ack) != 0)
                    {
                        memset(Esp_P->ReceBuffer,0,Esp_P->DataLength*sizeof(char));
                        Esp_P->ReceState = false;
                        return true;
                    }
                    memset(Esp_P->ReceBuffer,0,Esp_P->DataLength*sizeof(char));
                    Esp_P->ReceState = false;
                }
            }
            if(Attempts <= 1)
            {
                return false;
            }
            Delay_ms(500);
        }
    }
    return false;
}

/**************************************************************************
函数功能：ESP8266模块自检
入口参数：State：模块状态
返回  值：true：配置成功、false：配置失败
**************************************************************************/
static bool ESP8266_SelfTest(void)
{
    bool FeedBack = SendEsp8266Cmd("AT","OK",10);   //检测模块是否在线
    if(FeedBack)
    {
        Esp_P->ShowStateFunc(5,182,"ESP8266 Detected!");
        return true;
    }
    else
    {
        Esp_P->ShowStateFunc(5,182,"ESP8266 Not Detected!");
        Delay_ms(1000);
        Delay_ms(1000);
        return false;
    }
}

/**************************************************************************
函数功能：ESP8266模块进入透传发送模式
入口参数：State：模块状态
返回  值：无
注意事项：进入透传模式后再发送信息时模块无反馈，因此不需要给反馈
**************************************************************************/
static void ESP8266_UnvarnishSend(bool State) 
{
    if(State)
    {
        bool FeedBack = SendEsp8266Cmd("AT+CIPMODE=1","OK",10);
        if(FeedBack)
        {
            SendEsp8266Cmd("AT+CIPSEND",">",5);
        }
        else;
    }
    else;
}

/**************************************************************************
函数功能：ESP8266模块连接外部服务端
入口参数：Mode：通信协议、IP：服务端IP地址、Port：服务端口号、State：模块状态
返回  值：true：连接成功、false：连接失败
注意事项：sprintf函数将字符串写入字符数组时，新的字符串会覆盖掉原有内容，直到遇到 '\0' 为止
**************************************************************************/
static bool ESP8266_LinkServe(char *Mode,char *IP,char *Port,bool State)  
{
    char SenStr[100] = {0},SenBuf[100];
    if(State)
    {
        sprintf(SenStr,"\"%s\",\"%s\",%s",Mode,IP,Port); 
        sprintf(SenBuf,"AT+CIPSTART=%s",SenStr);
        bool FeedBack = SendEsp8266Cmd(SenBuf,"OK",25);
        if(FeedBack)
        {
            Esp_P->ShowStateFunc(5,245,"Server Connected!");
            Delay_ms(1000);
            return true;
        }
        else
        {
            Esp_P->ShowStateFunc(5,245,"Server Unable Connected!");
            Delay_ms(1000);
            Delay_ms(1000);
            return false;
        }
    }
    else return false;
}

/**************************************************************************
函数功能：ESP8266模块连接外部WiFi
入口参数：SSID：WiFi名称字符串、PassWord：WiFi密码字符串、State：模块状态
返回  值：true：连接成功、false：连接失败
**************************************************************************/
static bool ESP8266_JoinWifi(char *SSID,char *PassWord,bool State)
{
    char SenBuf[100];
    if(State)
    {
        sprintf(SenBuf,"AT+CWJAP=\"%s\",\"%s\"",SSID,PassWord); //连接的WIFI名称和密码
        bool FeedBack = SendEsp8266Cmd(SenBuf,"OK",25);
        if(FeedBack)
        {
            Esp_P->ShowStateFunc(5,224,"WiFi Connected!");
            Delay_ms(1000);
            return true;
        }
        else
        {
            Esp_P->ShowStateFunc(5,224,"WiFi Unable Connected!");
            Delay_ms(1000);
            Delay_ms(1000);
            return false;
        }
    }
    else return false;
}

/**************************************************************************
函数功能：设置ESP8266的基础功能
入口参数：Cmd：基础功能指令、State：模块状态
返回  值：true：选择成功、false：选择失败
**************************************************************************/
static bool ESP8266_BasicSet(BasicCmdDef Cmd,bool State) 
{
    bool FeedBack;
    if(State)
    {
        switch(Cmd)
        {
            case ReStart:
                    {
                        FeedBack = SendEsp8266Cmd("AT+RST","OK",10);            //重启模块延时
                        if(FeedBack)
                        {
                            Delay_ms(1000);
                            Delay_ms(1000);
                            return true;
                        }
                        else
                        { 
                            Esp_P->ShowStateFunc(5,224,"ESP8266 Unable Restart!");
                            Delay_ms(1000);
                            Delay_ms(1000);
                            return false;
                        }
                    } 
            case Back_ON:
                    {
                        FeedBack = SendEsp8266Cmd("ATE1","OK",10);
                        if(FeedBack)
                        {
                            return true;
                        }
                        else
                        { 
                            Esp_P->ShowStateFunc(5,203,"ESP8266 Unable BackShow!");
                            Delay_ms(1000);
                            Delay_ms(1000);
                            return false;
                        }
                    } 
            case Back_OFF:
                    {
                        FeedBack = SendEsp8266Cmd("ATE0","OK",10);
                        if(FeedBack)
                        {
                            return true;
                        }
                        else
                        { 
                            Esp_P->ShowStateFunc(5,203,"ESP8266 Unable DisBackShow!");
                            Delay_ms(1000);
                            Delay_ms(1000);
                            return false;
                        }
                    }
            case ReStore:
                    {
                        FeedBack = SendEsp8266Cmd("AT+RESTORE","OK",10);        //恢复出厂延时
                        if(FeedBack)
                        {
                            Delay_ms(1000);
                            Delay_ms(1000);
                            return true;
                        }
                        else
                        { 
                            Esp_P->ShowStateFunc(5,203,"ESP8266 Unable Restore!");
                            Delay_ms(1000);
                            Delay_ms(1000);
                            return false;
                        }
                    } 
            default:return false;
        }
    }
    else return false;
}

/**************************************************************************
函数功能：选择ESP8266 WiFi的工作模式
入口参数：Mode：工作模式、State：模块状态
返回  值：true：选择成功、false：选择失败
**************************************************************************/
static bool ESP8266_NetMode(NetTypeDef Mode,bool State)
{
    bool FeedBack;
    if(State)
    {
        switch(Mode)
        {
            case STA:   FeedBack = SendEsp8266Cmd("AT+CWMODE=1","OK",10);break; 
            case AP:    FeedBack = SendEsp8266Cmd("AT+CWMODE=2","OK",10);break; 
            case STA_AP:FeedBack = SendEsp8266Cmd("AT+CWMODE=3","OK",10);break;
            case Manual:FeedBack = true;break;
            default:    FeedBack = false;break; 
        }
        if(FeedBack)
        {
            Esp_P->ShowStateFunc(5,203,"Mode Set Success!");
            return true;
        }
        else
        {
            Esp_P->ShowStateFunc(5,203,"Mode Set Fail!");
            Delay_ms(1000);
            Delay_ms(1000);
            return false;
        }
    }
    else
    {
        return false;
    }
}

/**************************************************************************
函数功能：配置ESP8266通过WiFi接收任务码
入口参数：无
返回  值：无
注意事项：
--开机并开启串口配置好ESP8266后，关闭串口，等到预期的流程后开启串口中断接收任务码
--模块重启会发送大量数据导致串口接收异常，因此涉及到模块重启的操作在使用之前需要进行手动配置
    //InitFlag = ESP8266_BasicSet(ReStore,InitFlag);            //恢复出厂设置避免自动连接
    //InitFlag = ESP8266_BasicSet(Back_OFF,InitFlag);           //关闭回显
    //InitFlag = ESP8266_BasicSet(ReStart,InitFlag);            //重启生效
**************************************************************************/
bool ConfigurationEsp8266(void)
{
    bool InitFlag = ESP8266_SelfTest();                             //检测模块是否在线
    InitFlag = ESP8266_NetMode(Manual,InitFlag);                    //设置STA模式(手动)
    InitFlag = ESP8266_JoinWifi(WiFiSSID,WiFiKey,InitFlag);         //连接WiFi  
    InitFlag = ESP8266_LinkServe(NetMode,NetIP,NetPort,InitFlag);   //连接服务端
    //ESP8266_UnvarnishSend(InitFlag);                              //进入透传模式
    return InitFlag;
}

