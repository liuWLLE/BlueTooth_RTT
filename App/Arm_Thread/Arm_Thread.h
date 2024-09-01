#ifndef __ARM_THREAD_H_
#define __ARM_THREAD_H_

#include "sys.h"

static char *TaskCodeStr[] = 
{
    "123+123","123+132","123+213","123+231","123+312","123+321",
    "132+123","132+132","132+213","132+231","132+312","132+321",
    
    "213+123","213+132","213+213","213+231","213+312","213+321",
    "231+123","231+132","231+213","231+231","231+312","231+321",
    
    "312+123","312+132","312+213","312+231","312+312","312+321",
    "321+123","321+132","321+213","321+231","321+312","321+321"
};

static unsigned int TaskCodeNum[] = 
{
    123123,123132,123213,123231,123312,123321,
    132123,132132,132213,132231,132312,132321,
    
    213123,213132,213213,213231,213312,213321,
    231123,231132,231213,231231,231312,231321,
    
    312123,312132,312213,312231,312312,312321,
    321123,321132,321213,321231,321312,321321
};

//机械臂任务定义
typedef enum
{
    RecoverDefault,     //恢复默认
    ColorAndRing,       //颜色识别与色环定位
}ArmTaskSet; 

typedef enum
{
    Normal,             //正常平放物料
    StackUp             //物料码垛
}OrderPlace;

void MainCtrl_thread_entry(void* parameter);                                //主控制线程

void Ground_PutAction(OrderPlace PlaceState,ColorSele color,bool Rotate);   //放置物料动作组
void Ground_TakeAction(ColorSele color);                                    //从地上拿取物料并放置在物料盘
void TakeRawMaterial(void);                                                 //原料区拿取物料动作组
void IdentifyColorTake(bool Update,char AloneCode);                         //识别并拿取对应颜色的物料以及发送信息给车

#endif

