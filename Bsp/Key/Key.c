#include "Key.h"

//按键状态
typedef struct
{
    u8 KeyValueNew;                 //按键当前值
    u8 KeyValueOld;                 //按键上次值
    void (*KeyCallBackFunc)(u8);    //按键回调函数
}KeyControl;
volatile KeyControl KeyAlone;       //定义记录按键状态的结构体变量

/**************************************************************************
函数功能：IO口电平检测
入口参数：无
返回  值：当前IO口的电平情况
注意事项：无
**************************************************************************/
static inline u8 KeyGpioState(void)
{
    return GPIO_ReadInputDataBit(KEY_PORT,KEY_GPIO_PIN);
}

/**************************************************************************
函数功能：按键IO口初始化
入口参数：无
返回  值：无
注意事项：当引脚设置为任意输入时，配置引脚速度无意义
**************************************************************************/
void KeyInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    
    RCC_APB2PeriphClockCmd(KEY_GPIO_CLK,ENABLE);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
    GPIO_InitStructure.GPIO_Pin = KEY_GPIO_PIN;
    
    GPIO_Init(KEY_PORT, &GPIO_InitStructure);
}

/*************************************************************************************************
函数功能：检测按键值
入口参数：mode：true：检测松开、false：不检测松开
返 回 值：KeyShort：短按、KeyLong：长按、KeySlow：松开
注意事项：仅在开机自检阶段使用一次
*************************************************************************************************/
u8 KeyScanSingle(bool mode)
{
    if(mode)
    {
        u32 KeyTime;
        if(KeyGpioState() == Bit_RESET)         //检测按键是否被按下
        {
            Delay_ms(KEY_SHAKE_TIME);           //延时消抖
            if(KeyGpioState() == Bit_RESET)     //再次检测是否为低电平
            {
                while(KeyGpioState() == Bit_RESET)
                {
                    Delay_ms(KEY_SHAKE_TIME);
                    KeyTime++;
                }
                if(KeyTime>3 && KeyTime<50)     //短按
                {
                    KeyTime = 0;
                    return KeyShort;
                }
                else if(KeyTime > 120)          //长按
                {
                    KeyTime = 0;
                    return KeyLong;
                }
            }
            else
            {
                return KeyRelease;              //松开
            }
        }
    }
    else
    {
        if(KeyGpioState() == Bit_RESET)         //检测按键是否被按下
        {
            Delay_ms(KEY_SHAKE_TIME);           //延时消抖
            if(KeyGpioState() == Bit_RESET)     //再次检测是否为低电平
            {
                return KeyShort;
            }
            else
            {
                return KeyRelease;              //松开
            }
        }
    }
    return KeyRelease;                          //松开
}

/*************************************************************************************************
函数功能：检测按键值
入口参数：无
返 回 值：KeyShort：短按、KeyLong：长按、KeySlow：松开
注意事项：1、以一定频率调用  2、频率不要超过 1000 Hz  3、通过按键被按下的时间判断键值
*************************************************************************************************/
u8 KeyScanFreq(void)
{
    static unsigned int PressTime;
    static unsigned int LooseCount;

    if(KeyGpioState() == KEY_PRESS)                         //检测到按下
    {
        PressTime += KEY_TIME_RATIO;                        //计算按下的时间
        LooseCount = 0;                                     //松开消抖
        if(PressTime >= KEY_PERSIST_TIME)
        {
            return KeyPersist;                              //返回持续长按
        }
    }
    else if(KeyGpioState() == KEY_LOOSE)                    //检测到松开
    {
        LooseCount++;                                       //记录松开的次数
        if(LooseCount > KEY_LOOSE_COUNT)                    //松开大于一定次数认为完全松开
        {
            LooseCount = 0;                                 //重置松开次数
            if(PressTime >= KEY_SHAKE_TIME)                 //按下超过消抖时间认为确实按下
            {
                if(PressTime <= KEY_SHORT_TIME)             //短按时间内认为是短按
                {
                    PressTime = 0;                          //按下时间清零
                    return KeyShort;                        //返回短按松开
                }
                else if(PressTime <= KEY_PERSIST_TIME)      //长按时间内认为长按松开
                {
                    PressTime = 0;                          //按下时间清零
                    return KeyLong;                         //返回长按松开
                }
                PressTime = 0;                              //按下时间清零
            }
            return KeyRelease;                              //返回松开
        }
    }
    return KeyRelease;                                      //条件都不成立认为松开
}

/***********************************************************
函数功能：按键设备运行函数
入口参数：无
返回  值：无
注意事项：检测按键值变化
***********************************************************/
void KeyDeviceRunning(void)
{
    KeyAlone.KeyValueNew = KeyScanFreq();                   //经由按键检测函数获得被按下的按键值
    if(KeyAlone.KeyValueNew != KeyAlone.KeyValueOld)        //若键值发生变化
    {
        //printf("Key:%d\r\n",KeyAlone.KeyValueNew);          //调试输出
        if(KeyAlone.KeyCallBackFunc != NULL)                //判断函数是否被注册
        {
            KeyAlone.KeyCallBackFunc(KeyAlone.KeyValueNew); //调用函数
        }
        KeyAlone.KeyValueOld = KeyAlone.KeyValueNew;        //更新键值
    }
}

/***********************************************************
函数功能：回调函数注册
入口参数：KeyAppFnc：需要被调用的函数
返回  值：无
注意事项：无
***********************************************************/
void AddKeyCallBack(void (*KeyAppFnc)(u8))
{
    KeyAlone.KeyValueNew = KeyRelease;          //定义一个当前按键值变量
    KeyAlone.KeyValueOld = KeyRelease;          //定义一个上次按键值变量
    KeyAlone.KeyCallBackFunc = KeyAppFnc;       //注册用户函数
}

