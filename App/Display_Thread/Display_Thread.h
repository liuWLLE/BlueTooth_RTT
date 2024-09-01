#ifndef __DISPLAY_H_
#define __DISPLAY_H_

#include "sys.h"

void LcdNormal_thread_entry(void* parameter);       //LCD屏幕正常刷新线程函数
void LcdDebug_thread_entry(void* parameter);        //LCD屏幕调试刷新线程函数

void ScreenDisplayTaskCode(uint32_t TaskCode);      //屏幕显示任务码
void ScreenDisplayPicture(const uint8_t *pImage);   //屏幕显示图片
void ScreenDisplayKeyState(char *KeyText);          //屏幕显示关键状态

#endif

