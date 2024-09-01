#ifndef __RTTHREAD_CFG_H__
#define __RTTHREAD_CFG_H__

// Basic Configuration 
// RT-Thread �Ļ�������
// Maximal level of thread priority <8-256>
// ��ʾ֧�ֶ��ٸ����ȼ���ȡֵ��ΧΪ 8~256
// Default: 32
// Ĭ��Ϊ 32
#define RT_THREAD_PRIORITY_MAX  32  

// OS tick per second
// ��ʾ����ϵͳÿ�����ж��ٸ�tick��tick���ǲ���ϵͳ��ʱ������
// Default: 1000   (1ms)
// Ĭ��Ϊ 1000��������ϵͳ��ʱ������tick���� 1ms
#define RT_TICK_PER_SECOND  1000    

// Alignment size for CPU architecture data access
// ��ʾ CPU �����������Ҫ���ٸ��ֽڶ���
// Default: 4
// Ĭ��Ϊ 4 ���ֽ�
#define RT_ALIGN_SIZE   4

// the max length of object name<2-16>
// ��ʾ�ں˶������ֵ���󳤶ȣ�ȡֵ��ΧΪ 2~16
// Default: 8
// Ĭ��Ϊ 8
#define RT_NAME_MAX 8

// Using RT-Thread components initialization
// ʹ�� RT-Thread �����ʼ����Ĭ��ʹ��
#define RT_USING_COMPONENTS_INIT
//#define RT_DEBUG_INIT   1

// ʹ���û� main ������Ĭ�ϴ�
#define RT_USING_USER_MAIN

// the stack size of main thread<1-4086>
// main �߳�ջ��С��ȡֵ��ΧΪ 1~4086
// Default: 512
// ��λΪ�ֽڣ�Ĭ��Ϊ512
#define RT_MAIN_THREAD_STACK_SIZE   2048



// Debug Configuration
// ��������
// enable kernel debug configuration
// �����ں˵�������
// Default: Disable kernel debug configuration
// Ĭ�������ں˵�������
#define RT_DEBUG

//����������߳�����ʱ�䣩
//#define DEBUG_THREAD_CONSUME_TIME

//����������̼߳��ʱ�䣩
//#define DEBUG_THREAD_INTERVAL_TIME                      

// enable components initialization debug configuration<0-1>
// ���������ʼ����������
// Default: 0
// Ĭ�ϲ�����
#define RT_DEBUG_INIT 1

// thread stack over flow detect
// �̶߳�ջ������
// Diable Thread stack over flow detect
// Ĭ�ϲ�ʹ���̶߳�ջ������
#define RT_USING_OVERFLOW_CHECK



// Hook Configuration
// ���Ӻ������ã�Ŀǰȫ���ر�
// using hook
//#define RT_USING_HOOK

// using idle hook
//#define RT_USING_IDLE_HOOK



// Software timers Configuration
// �����ʱ������
// Enables user timers
// �Ƿ�ʹ�������ʱ��
#define RT_USING_TIMER_SOFT     1
#if RT_USING_TIMER_SOFT == 0
    #undef RT_USING_TIMER_SOFT
#endif

// The priority level of timer thread <0-31>
// ��ʱ���������̵߳����ȼ�
// Default: 4
// Ĭ��Ϊ4
#define RT_TIMER_THREAD_PRIO    4

// The stack size of timer thread <0-8192>
// ��ʱ�������߳�ջ��С
// Default: 512
// Ĭ��Ϊ512
#define RT_TIMER_THREAD_STACK_SIZE  512



// IPC(Inter-process communication) Configuration
// �ڲ�ͨ�����ã������ź��������������¼����������Ϣ����
// Using Semaphore
// ʹ���ź���������ʹ�ã�
#define RT_USING_SEMAPHORE

// Using Mutex
// ʹ�û����ź���
#define RT_USING_MUTEX

// Using Event
// ʹ���¼���
#define RT_USING_EVENT

// Using MailBox
// ʹ����Ϣ����
#define RT_USING_MAILBOX

// Using Message Queue
// ʹ�ö���
#define RT_USING_MESSAGEQUEUE



// Memory Management Configuration
// �ڴ��������
// Dynamic Heap Management
// ��ʾ�Ƿ�ѣ�Ŀǰ�رգ���ʹ�ö�
// ��Ҫʹ�ö�̬�ڴ洴���߳�ʱ��Ҫʹ��
#define RT_USING_HEAP

// using small memory
// �ڱ�ʾ�Ƿ�ʹ��С�ڴ棬Ŀǰʹ��
#define RT_USING_SMALL_MEM

// using tiny size of memory
// ��ʾ�Ƿ�ʹ�ü�С�ڴ棬Ŀǰ�رգ���ʹ��
//#define RT_USING_TINY_SIZE



// Console Configuration
// ����̨����
// ����̨����rt_kprintf()��������������豸��ͨ��ʹ�ô���
// Using console
#define RT_USING_CONSOLE

// the buffer size of console <1-1024>
// ����̨�ַ���������С
// Default: 128  (128Byte)
// Ĭ��128�ֽ�
#define RT_CONSOLEBUF_SIZE  256

// ѡ�����̨rt_kprintf()���������������
// Ĭ��ʹ�ô���1
#define DEBUG_USARTx        UART4



// FinSH Configuration
// FINSH ����
// include finsh config
// Select this choice if you using FinSH 
//#include "finsh_config.h"




// Device Configuration
// �豸����
// using device framework
// ʹ���豸���
//#define RT_USING_DEVICE

#endif
