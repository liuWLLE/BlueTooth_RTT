#ifndef __ESP8266_H
#define __ESP8266_H

#include "sys.h"

/*
ESP8266接收任务码指令：
-- AT 测试指令
-- AT+RST 重启模块
-- ATE0 关闭回显
-- AT+RESTORE 恢复出厂设置功能
-- AT+CWMODE=1 设置为 STA 模式
-- AT+CWJAP="光电科技协会","123guangdian123" 连接路由器
-- AT+CIPSTART="TCP","192.168.1.102",8000 模块连接到 server
-- AT+CIPMODE=1 开启透传模式
-- AT+CIPSEND 开始透传
-- AT+CWDHCP=1,0 关闭STA的DHCP功能
-- AT+CIFSR 获取本地IP
*/

typedef enum
{
    ReStart,    //重启模块
    Back_ON,    //开启模块回显功能
    Back_OFF,   //关闭模块回显功能
    ReStore     //恢复出厂设置
}BasicCmdDef;

typedef enum
{
    STA,        //STA模式
    AP,         //AP模式
    STA_AP,     //STA&AP模式
    Manual      //手动配置模式
}NetTypeDef;

void ESP8266_Init(void (*ShowFunc)(),void (*SendFunc)(),USART_TypeDef *USARTx,char *Buff,bool *State,u8 Length);    //ESP8266初始化
bool ConfigurationEsp8266(void);    //配置ESP8266接收任务码

#endif
