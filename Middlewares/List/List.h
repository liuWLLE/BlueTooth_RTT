#ifndef __LIST_H
#define __LIST_H

#include "sys.h"

#define LIST_SIZE   210     //队列大小

typedef struct {
    uint8_t buffer[LIST_SIZE];  //队列
    __IO uint8_t ptrWrite;      //队尾
    __IO uint8_t ptrRead;       //队头
    __IO uint8_t Length;        //队长
}LIST_t;

extern __IO LIST_t rxLIST;

void LIST_initQueue(void);
void LIST_enQueue(uint8_t data);
uint8_t LIST_deQueue(void);
bool LIST_isEmpty(void);
uint8_t LIST_queueLength(void);

#endif
