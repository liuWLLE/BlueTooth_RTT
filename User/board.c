#include "board.h" 

// Hardware
#include "Usart.h"
#include "Flash.h"
#include "WDG.h"
 
// Bsp
#include "Iap.h"
#include "Key.h" 
#include "Led.h"
#include "Steering.h"
#include "Motor.h"
#include "Esp8266.h"
#include "LcdInit.h"

// App
#include "Display_Thread.h"

//��������ϵͳ���������
TaskState SysTask =
{
    .NowTask = TaskStart,       //�����߼�
    .OldTask = TaskStart,
    .OldShow = TaskStart,
    .CodeS = NULL,              //����������
    .Code1 = NULL,              //��һ��������
    .Code2 = NULL,              //�ڶ���������
};
//��������ϵͳ���в���ֵ
Operat_Control SysValue =
{
    .QR_Mode = ScanModule,      //��������շ�ʽ ScanModule/ESP8266/DT_06
    .ReceCarSta = NULL,         //�յ���λ����״̬
    .TaskStart = false,         //�������б�־λ
    .DebugStart = false,        //������Ա�־λ
    .DirectGrab = false,        //�����ж�ץȡ��־λ
    .InitComplete = false       //��ʼ��δ���
};

// ʹ�ö�̬�ڴ�ʱ��Ҫ�����ú�
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    // ���ڲ� SRAM �������һ���־�̬�ڴ�����Ϊ rtt �Ķѿռ䣬��������Ϊ 4KB
    #define RT_HEAP_SIZE    8192
static uint32_t rt_heap[RT_HEAP_SIZE];
/**************************************************************************
�������ܣ���ȡ�ѵ���ʼ��ַ
��ڲ������� 
����  ֵ���ѵ���ʼ��ַ
**************************************************************************/
RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}
/**************************************************************************
�������ܣ���ȡ�ѵĽ�����ַ
��ڲ������� 
����  ֵ���ѵĽ�����ַ
**************************************************************************/
RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif

/**************************************************************************
�������ܣ�������Ӳ����ʼ������
��ڲ������� 
����  ֵ����
ע�����1����ʼ���׶��߳�δ��ʼ���ȣ������Բ���ȫ�ֱ�����ʽ���߳̿�ʼ����ö��з���
          2����Ļ�����߶�λ�ã�15��35��56��77��98��119��140��161��182��203��224��245��266
**************************************************************************/
void rt_hw_board_init(void)
{
    // �����ж�ƫ��
    sys_nvic_set_vector_table(FLASH_BASE_SET,FLASH_SHIFT_ADDR);
    
    // ϵͳ�δ�ʱ������
    SysTick_Config(SystemCoreClock/RT_TICK_PER_SECOND);

    // ��Ƭ�������ʼ��
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    Delay_init(72);

    // LCD��ʼ��
    LCD_Init();
    
    // �����豸��Ϣ
    Delay_ms(200);
    LCD_Clear();
    LCD_SetColor(LCD_YELLOW);
    LCD_SetAsciiFont(&ASCII_Font16);
    LCD_DisplayString(5,15,"Copyright (C)  @2023");
    LCD_SetColor(LCD_BLUE);
    LCD_DisplayString(5,35,"STM32F103RCT6  72 Mhz");
    LCD_DisplayString(5,56,"FLASH:256 KB  SRAM:48 KB");
    LCD_SetColor(LCD_YELLOW);
    LCD_DisplayString(5,77,"Initialize System:");

    // ��ʼ��Key Led
    Delay_ms(200);
    KeyInit();
    LaserLed_Init();
    BoardLed_Init();

    // ��������lash��IAP��־λ
    switch(KeyScanSingle(false))
    {
        case KeyShort:
            {
                LCD_SetColor(LCD_RED);
                FLASH_Write(FLASH_DATA_ADDR,&*(u16*)1,1);
                LCD_DisplayString(5,98,"Set IAP Flag Success!");
                Delay_ms(1000);
                LCD_DisplayString(5,119,"System Restart!");
                Delay_ms(500);
                sys_soft_reset();
            }break;
        case KeyLong:
            {
                LCD_SetColor(LCD_RED);
                FLASH_Write(FLASH_DATA_ADDR,&*(u16*)1,1);
                LCD_DisplayString(5,98,"Set IAP Flag Success!");
                Delay_ms(1000);
                LCD_DisplayString(5,119,"System Restart!");
                Delay_ms(500);
                sys_soft_reset();
            }break;
        case KeyRelease:break;
        default:break; 
    }
    LCD_SetColor(LCD_WHITE);
    LCD_DisplayString(5,98,"Key Led Init Success..."); 

    // ���������ʼ��
    Delay_ms(200);
    Motor_Init();
    LCD_DisplayString(5,119,"Motor Init Success...");

    // �����ʼ��
    Delay_ms(200);
    Steering_Init();
    LCD_DisplayString(5,140,"Steering Init Success...");

    // ���ڳ�ʼ��
    Delay_ms(200);
    USART_Config_2(19200);
    UART_Config_4(115200);
    if(SysValue.QR_Mode == ScanModule)
    {
        USART_Config_3(9600);
        LCD_DisplayString(5,161,"Serial Init Success...");
        SysValue.InitComplete = true;

    }
    else if(SysValue.QR_Mode == ESP8266)
    {
        USART_Config_3(115200);
        ESP8266_Init(LCD_DisplayString,
                     Usart_SendString,
                     USART3,
                     (char*)U3Buf.RecBuf,
                     (bool*)&U3Para.State,
                     U3RecLen);
        LCD_DisplayString(5,161,"Serial Init Success...");
        SysValue.InitComplete = ConfigurationEsp8266();
    }
    else if(SysValue.QR_Mode == DT_06)
    {
        USART_Config_3(115200);
        LCD_DisplayString(5,161,"Serial Init Success...");
        
    }
    
    // ���Ź���ʼ����40,000/4*2^(4)/625 = 1 HZ��
    Delay_ms(200);
    IWDG_Init(4,625);
    LCD_DisplayString(5,182,"IWDG Init Success...");
    
    // ��ʼ�����
    Delay_ms(200);
    if(!SysValue.DebugStart)
    {
        if(SysValue.InitComplete)
        {
            ScreenDisplayKeyState("Hello World!");
        }
        else
        {
            ScreenDisplayKeyState("System Error!");
        }
    }


/*
   RT-Thread Ϊ����������ṩ��һ����ʼ���������ú�����components.c����ʵ�֣�
   �� rtconfig.h ����ĺ�RT_USING_COMPONENTS_INIT�����Ƿ���ã�Ĭ���ǿ�����
*/
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
/*
   RT-Thread �ṩ��һ���ڴ��ʼ��������ֻ����ʹ�� RT-Thread �ṩ�Ķ�̬�ڴ���亯��ʱ����Ҫʹ�õ�
  �ú����� mem.c ����ʵ�֣��� rtconfig.h ����� RT_USING_HEAP �� RT_USING_USER_MAIN �����������Ƿ����
*/
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
}

/**************************************************************************
�������ܣ�SysTick �жϷ�����
��ڲ������� 
����  ֵ����
ע�����SysTick �жϷ������ڹ̼����ļ� stm32f10x_it.c ��Ҳ�����ˣ�������
          �� board.c ���ֶ���һ�Σ���ô�����ʱ�������ظ�����Ĵ��󣬽��
          �����ǿ��԰� stm32f10x_it.c �е�ע�ͻ���ɾ������
**************************************************************************/
void SysTick_Handler(void)
{
    rt_interrupt_enter();       // �����ж�
    rt_tick_increase();         // ����ʱ��
    rt_interrupt_leave();       // �뿪�ж�
}

// ���ڳ�ʼ���Լ�����̨���ʵ��
#ifdef RT_USING_CONSOLE
/**************************************************************************
�������ܣ���ӳ�䴮�� DEBUG_USARTx �� rt_kprintf()
��ڲ�����str��Ҫ��������ڵ��ַ���
����  ֵ����
ע�����DEBUG_USARTx ���� bsp_usart.h �ж���ĺ꣬Ĭ��ʹ�ô��� 1
**************************************************************************/
void rt_hw_console_output(const char *str)
{
    rt_enter_critical();    // �����ٽ��
    while(*str!='\0')       // ֱ���ַ�������
    {
        if(*str=='\n')      // ����
        {
            USART_SendData(DEBUG_USARTx,'\r');
            while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_TXE) == RESET);
        }
        USART_SendData(DEBUG_USARTx, *str++);
        while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_TXE) == RESET);
    }
    rt_exit_critical();     // �˳��ٽ��
}

#endif


