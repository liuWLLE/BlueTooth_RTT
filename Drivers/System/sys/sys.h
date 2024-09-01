#ifndef __SYS_H
#define __SYS_H

// Core
#include "stm32f10x.h"

// Std
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

// Rtos
#include <rthw.h>
#include <rtthread.h>

// Sys
#include "Delay.h"

/**
 * SYS_SUPPORT_OS用于定义系统文件夹是否支持OS
 * 0,不支持OS
 * 1,支持OS
 */
#define SYS_SUPPORT_OS      1

//串口事件集
#define USART1_EVENT        (0x01 << 1)     //设置串口1接收事件掩码的位 1
#define USART2_EVENT        (0x01 << 2)     //设置串口2接收事件掩码的位 2
#define USART3_EVENT        (0x01 << 3)     //设置串口3接收事件掩码的位 3
#define USART4_EVENT        (0x01 << 4)     //设置串口4接收事件掩码的位 4
#define USART5_EVENT        (0x01 << 5)     //设置串口5接收事件掩码的位 5

//LCD事件集
#define LCD_START_EVENT     (0x01 << 0)     //设置显示“开始运行”事件掩码的位 0
#define LCD_CODES_EVENT     (0x01 << 1)     //设置显示任务码事件掩码的位 1
#define LCD_HELLO_EVENT     (0x01 << 2)     //设置显示“欢迎”事件掩码的位 2
#define LCD_ERROR_EVENT     (0x01 << 3)     //设置显示“错误”事件掩码的位 3

//发送至下位机命令
#define GoRawAreaS          "^B_"       //去原料区-------------------1
#define GoRoughRED          "^C_"       //去粗加工区红圆环-----------2
#define GoRoughGREEN        "^D_"       //去粗加工区绿圆环-----------3
#define GoRoughBLUE         "^E_"       //去粗加工区蓝圆环-----------4
#define GoStoreRED          "^L_"       //去暂存区红圆环-------------5
#define GoStoreGREEN        "^M_"       //去暂存区绿圆环-------------6
#define GoStoreBLUE         "^N_"       //去暂存区蓝圆环-------------7
#define StoreGoRough        "^J_"       //暂存区到粗加工区-----------8
#define RoughGoRaw          "^K_"       //粗加工到原料区-------------9
#define RestartS            "^F_"       //返回起点-------------------10
#define Calibration         "^P_"       //允许底盘进行校准-----------11
#define ReceCarSent         "^O_"       //收到底盘发送的消息

//接收下位机命令
#define ArriveLocation      0x51        //底盘已到达指定位置（Q）------1
#define CarCalibration      0x49        //运行底盘校准（I）------------3
#define MaterialRED         0x72        //检测到红色色块（r）----------4
#define MaterialGREEN       0x67        //检测到绿色色块（g）----------5
#define MaterialBLUE        0x62        //检测到蓝色色块（b）----------6


//位带操作,实现51类似的GPIO控制功能，IO口操作宏定义
#define BITBAND(addr,bitnum)    ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)          *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr,bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

//IO口地址映射
#define GPIOA_ODR_Addr      (GPIOA_BASE+12)                 //0x4001080C 
#define GPIOB_ODR_Addr      (GPIOB_BASE+12)                 //0x40010C0C 
#define GPIOC_ODR_Addr      (GPIOC_BASE+12)                 //0x4001100C 
#define GPIOD_ODR_Addr      (GPIOD_BASE+12)                 //0x4001140C 
#define GPIOE_ODR_Addr      (GPIOE_BASE+12)                 //0x4001180C 
#define GPIOF_ODR_Addr      (GPIOF_BASE+12)                 //0x40011A0C    
#define GPIOG_ODR_Addr      (GPIOG_BASE+12)                 //0x40011E0C    
                    
#define GPIOA_IDR_Addr      (GPIOA_BASE+8)                  //0x40010808 
#define GPIOB_IDR_Addr      (GPIOB_BASE+8)                  //0x40010C08 
#define GPIOC_IDR_Addr      (GPIOC_BASE+8)                  //0x40011008 
#define GPIOD_IDR_Addr      (GPIOD_BASE+8)                  //0x40011408 
#define GPIOE_IDR_Addr      (GPIOE_BASE+8)                  //0x40011808 
#define GPIOF_IDR_Addr      (GPIOF_BASE+8)                  //0x40011A08 
#define GPIOG_IDR_Addr      (GPIOG_BASE+8)                  //0x40011E08 
 
//IO口操作,只对单一的IO口!确保n的值小于16!
#define PAout(n)            BIT_ADDR(GPIOA_ODR_Addr,n)      //输出 
#define PAin(n)             BIT_ADDR(GPIOA_IDR_Addr,n)      //输入 
                    
#define PBout(n)            BIT_ADDR(GPIOB_ODR_Addr,n)      //输出 
#define PBin(n)             BIT_ADDR(GPIOB_IDR_Addr,n)      //输入 
                    
#define PCout(n)            BIT_ADDR(GPIOC_ODR_Addr,n)      //输出 
#define PCin(n)             BIT_ADDR(GPIOC_IDR_Addr,n)      //输入 
                    
#define PDout(n)            BIT_ADDR(GPIOD_ODR_Addr,n)      //输出 
#define PDin(n)             BIT_ADDR(GPIOD_IDR_Addr,n)      //输入 
                    
#define PEout(n)            BIT_ADDR(GPIOE_ODR_Addr,n)      //输出 
#define PEin(n)             BIT_ADDR(GPIOE_IDR_Addr,n)      //输入
                    
#define PFout(n)            BIT_ADDR(GPIOF_ODR_Addr,n)      //输出 
#define PFin(n)             BIT_ADDR(GPIOF_IDR_Addr,n)      //输入
                    
#define PGout(n)            BIT_ADDR(GPIOG_ODR_Addr,n)      //输出 
#define PGin(n)             BIT_ADDR(GPIOG_IDR_Addr,n)      //输入


//颜色任务码定义
typedef enum
{
    Red_1 = 1,              //红1
    Green_2,                //绿2
    Blue_3,                 //蓝3
    NoColor                 //无
}ColorSele;

//任务运行流程定义
typedef enum
{ 
    TaskStart,              //任务开始
    ScanQR,                 //扫描二维码
    TakeRaw1,               //到达原料区后第一次拿取原料
    GoRough,TPRough1,       //前往粗加工区、第一次放置与拿取原料
    GoStaging,PutStag1,     //前往暂存区、第一次放置原料
    PassRough,TakeRaw2,     //经过粗加工区、返回原料区后第二次拿取原料 
    GoRough2,TPRough2,      //第二次前往粗加工区、第二次放置与拿取原料
    GoStag2,PutStag2,       //第二次前往暂存区、第二次放置原料
    Restart,Calibrat        //返回起点、底盘校准
}TaskSet; 
static char *TaskString[] = 
{
    "InitWait",
    "ScanQR",
    "TakeRaw1",
    "GoRough","TPRough1",
    "GoStaging","PutStag1",
    "PassRough","TakeRaw2",
    "GoRough2","TPRough2",
    "GoStag2","PutStag2",
    "Restart","Calibrat"
};

//二维码接收模式
typedef enum
{
    ScanModule,
    ESP8266,
    DT_06 
}QR_Rece; 

//任务码相关定义
typedef struct
{
    volatile TaskSet NowTask;           //当前任务
    volatile TaskSet OldTask;           //上一次任务
    volatile TaskSet OldShow;           //上一次任务（调试）
    volatile unsigned int CodeS;        //完整任务码
    volatile unsigned int Code1;        //第一次任务码
    volatile unsigned int Code2;        //第二次任务码
}TaskState;
extern TaskState SysTask; 

//任务控制定义
typedef struct
{
    volatile QR_Rece QR_Mode;           //二维码接收方式
    volatile char ReceCarSta;           //收到下位机的状态
    volatile bool TaskStart;            //任务是否开始
    volatile bool DebugStart;           //是否进入调试
    volatile bool DirectGrab;           //是否直接抓取(不看颜色)
    volatile bool InitComplete;         //初始化是否完成
}Operat_Control;
extern Operat_Control SysValue; 

//按键值字符串
static char *KeyString[] = 
{
    "Release",
    "Short",
    "Long",
    "Persist",
};

extern rt_mq_t Usart2_list;                 //定义串口2队列控制块
extern rt_mq_t Usart3_list;                 //定义串口3队列控制块
extern rt_mq_t Uart4_list;                  //定义串口4队列控制块

extern rt_mutex_t Action_mux;               //定义互斥量控制块

extern rt_event_t Usart_event;              //定义串口事件控制块
extern rt_event_t LcdShow_event;            //定义LCD显示事件控制块

extern rt_mailbox_t SendOrder_mail;         //定义发送指令邮箱控制块
extern rt_mailbox_t KeyValue_mail;          //定义按键值邮箱控制块

extern rt_thread_t MainCtrl_thread;         //主控制线程控制块指针
extern rt_thread_t UsartRece_thread;        //串口接收参数线程控制块指针
extern rt_thread_t LedKeyCtrl_thread;       //LED、按键控制线程控制块指针
extern rt_thread_t LcdNormal_thread;        //LCD屏幕正常刷新线程控制块指针
extern rt_thread_t LcdDebug_thread;         //LCD屏幕调试刷新线程控制块指针


u16 CheckStackUsage(rt_thread_t thread);                                //计算线程已使用的栈大小
void sys_nvic_set_vector_table(uint32_t baseaddr,uint32_t offset);      //设置中断偏移量
void sys_standby(void);                                                 //进入待机模式
void sys_soft_reset(void);                                              //系统软复位
void sys_wfi_set(void);                                                 //执行WFI指令
void sys_intx_disable(void);                                            //关闭所有中断
void sys_intx_enable(void);                                             //开启所有中断
void sys_msr_msp(uint32_t addr);                                        //设置栈顶地址

#endif
