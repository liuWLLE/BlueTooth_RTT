#include "List.h"

/*
配置初始化
//  LIST_initQueue(); //初始化队列
串口接收：
//  u8 receive;
//  if(USART_GetITStatus(DEBUG_USART3,USART_IT_RXNE)!=RESET){   //如果接收到了消息
/       receive = USART_ReceiveData(USART3);                    //取出接收寄存器数据
//      LIST_enQueue(receive);                                  //入队
//  }
*/

__IO LIST_t rxLIST = {0};

/**************************************************************************
函数功能：初始化队列
入口参数：无
返回  值：无
**************************************************************************/
void LIST_initQueue(void)
{
    rxLIST.ptrRead  = 0;
    rxLIST.ptrWrite = 0;
}

/**************************************************************************
函数功能：入队
入口参数：无
返回  值：无
**************************************************************************/
void LIST_enQueue(uint8_t data)
{
    rxLIST.buffer[rxLIST.ptrWrite] = data;
    
    ++rxLIST.ptrWrite;
    
    ++rxLIST.Length;
    if(rxLIST.ptrWrite >= LIST_SIZE)
    {
        rxLIST.ptrWrite = 0;
    }
}

/**************************************************************************
函数功能：出队
入口参数：无
返回  值：无
**************************************************************************/
uint8_t LIST_deQueue(void)
{
    uint8_t element = 0;
    
    element = rxLIST.buffer[rxLIST.ptrRead];
    
    ++rxLIST.ptrRead;
    --rxLIST.Length;
    if(rxLIST.ptrRead >= LIST_SIZE)
    {
        rxLIST.ptrRead = 0;
    }

    return element;
}

/**************************************************************************
函数功能：判断空队列
入口参数：无
返回  值：无
**************************************************************************/
bool LIST_isEmpty(void)
{
    if(rxLIST.ptrRead == rxLIST.ptrWrite)
    {
        return true;
    }
    
    return false;
}

/**************************************************************************
函数功能：计算队列长度
入口参数：无
返回  值：无
**************************************************************************/
uint8_t LIST_queueLength(void)
{
    if(rxLIST.ptrRead <= rxLIST.ptrWrite)
    {
        return (rxLIST.ptrWrite - rxLIST.ptrRead);
    }
    else
    {
        return (LIST_SIZE - rxLIST.ptrRead + rxLIST.ptrWrite);
    }
}
