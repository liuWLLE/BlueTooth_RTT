#ifndef __KEY_THREAD_H_
#define __KEY_THREAD_H_

#include "sys.h"

void KeyCtrl_thread_entry(void* parameter);     //按键控制线程函数

void KeyControlTask(u8 KeyValue);               //按键控制

#endif

