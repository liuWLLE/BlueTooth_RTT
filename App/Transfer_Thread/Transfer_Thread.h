#ifndef __TRANSFER_THREAD_H_
#define __TRANSFER_THREAD_H_

#include "sys.h"

#define SendCarUsart    USART2

typedef enum
{
    Jump,           //发送指令后跳出
    Wait,           //发送指令后等待
}OrderRun;

typedef enum
{
    Rough,          //选择粗加工区
    Staging,        //选择暂存区
}OrderLoca;

void UsartRece_thread_entry(void* parameter);                           //串口接收参数线程函数

void ReceCarState_Wire(void);                                           //接收下位机状态（杜邦线）
void ReceTaskCode_Mult(void);                                           //接收任务码（扫描模块、WiFi模块）
void MotorSteeringCtrl_BT(void);                                        //蓝牙控制电机运动（蓝牙）

void SendOrderToCar(char *str,u8 SendCount,u16 gap);                    //下下位机发送指令
void SendRingToCar(OrderLoca Loca,OrderRun Run,u8 Code,bool Init);      //向下位机发送圆环指令

#endif

