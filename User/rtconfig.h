#ifndef __RTTHREAD_CFG_H__
#define __RTTHREAD_CFG_H__

// Basic Configuration 
// RT-Thread 的基本配置
// Maximal level of thread priority <8-256>
// 表示支持多少个优先级，取值范围为 8~256
// Default: 32
// 默认为 32
#define RT_THREAD_PRIORITY_MAX  32  

// OS tick per second
// 表示操作系统每秒钟有多少个tick，tick即是操作系统的时钟周期
// Default: 1000   (1ms)
// 默认为 1000，即操作系统的时钟周期tick等于 1ms
#define RT_TICK_PER_SECOND  1000    

// Alignment size for CPU architecture data access
// 表示 CPU 处理的数据需要多少个字节对齐
// Default: 4
// 默认为 4 个字节
#define RT_ALIGN_SIZE   4

// the max length of object name<2-16>
// 表示内核对象名字的最大长度，取值范围为 2~16
// Default: 8
// 默认为 8
#define RT_NAME_MAX 8

// Using RT-Thread components initialization
// 使用 RT-Thread 组件初始化，默认使能
#define RT_USING_COMPONENTS_INIT
//#define RT_DEBUG_INIT   1

// 使用用户 main 函数，默认打开
#define RT_USING_USER_MAIN

// the stack size of main thread<1-4086>
// main 线程栈大小，取值范围为 1~4086
// Default: 512
// 单位为字节，默认为512
#define RT_MAIN_THREAD_STACK_SIZE   2048



// Debug Configuration
// 调试配置
// enable kernel debug configuration
// 启用内核调试配置
// Default: Disable kernel debug configuration
// 默认启用内核调试配置
#define RT_DEBUG

//调试输出（线程消耗时间）
//#define DEBUG_THREAD_CONSUME_TIME

//调试输出（线程间隔时间）
//#define DEBUG_THREAD_INTERVAL_TIME                      

// enable components initialization debug configuration<0-1>
// 启用组件初始化调试配置
// Default: 0
// 默认不启用
#define RT_DEBUG_INIT 1

// thread stack over flow detect
// 线程堆栈溢出检测
// Diable Thread stack over flow detect
// 默认不使能线程堆栈溢出检测
#define RT_USING_OVERFLOW_CHECK



// Hook Configuration
// 钩子函数配置，目前全部关闭
// using hook
//#define RT_USING_HOOK

// using idle hook
//#define RT_USING_IDLE_HOOK



// Software timers Configuration
// 软件定时器配置
// Enables user timers
// 是否使能软件定时器
#define RT_USING_TIMER_SOFT     1
#if RT_USING_TIMER_SOFT == 0
    #undef RT_USING_TIMER_SOFT
#endif

// The priority level of timer thread <0-31>
// 定时器器管理线程的优先级
// Default: 4
// 默认为4
#define RT_TIMER_THREAD_PRIO    4

// The stack size of timer thread <0-8192>
// 定时器管理线程栈大小
// Default: 512
// 默认为512
#define RT_TIMER_THREAD_STACK_SIZE  512



// IPC(Inter-process communication) Configuration
// 内部通信配置，包括信号量、互斥量、事件、邮箱和消息队列
// Using Semaphore
// 使用信号量（必须使用）
#define RT_USING_SEMAPHORE

// Using Mutex
// 使用互斥信号量
#define RT_USING_MUTEX

// Using Event
// 使用事件集
#define RT_USING_EVENT

// Using MailBox
// 使用消息邮箱
#define RT_USING_MAILBOX

// Using Message Queue
// 使用队列
#define RT_USING_MESSAGEQUEUE



// Memory Management Configuration
// 内存管理配置
// Dynamic Heap Management
// 表示是否堆，目前关闭，不使用堆
// 需要使用动态内存创建线程时需要使用
#define RT_USING_HEAP

// using small memory
// 于表示是否使用小内存，目前使能
#define RT_USING_SMALL_MEM

// using tiny size of memory
// 表示是否使用极小内存，目前关闭，不使用
//#define RT_USING_TINY_SIZE



// Console Configuration
// 控制台配置
// 控制台即是rt_kprintf()函数调试输出的设备，通常使用串口
// Using console
#define RT_USING_CONSOLE

// the buffer size of console <1-1024>
// 控制台字符缓冲区大小
// Default: 128  (128Byte)
// 默认128字节
#define RT_CONSOLEBUF_SIZE  256

// 选择控制台rt_kprintf()函数调试输出串口
// 默认使用串口1
#define DEBUG_USARTx        UART4



// FinSH Configuration
// FINSH 配置
// include finsh config
// Select this choice if you using FinSH 
//#include "finsh_config.h"




// Device Configuration
// 设备配置
// using device framework
// 使用设备框架
//#define RT_USING_DEVICE

#endif
