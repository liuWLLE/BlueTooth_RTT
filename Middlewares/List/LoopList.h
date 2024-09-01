#ifndef __LOOPLIST_H
#define __LOOPLIST_H

#include "sys.h"

#define min(a,b) (a)<(b)?(a):(b)       //获取最小值

/** 环形缓冲区数据结构 */
typedef struct {
    uint32_t  rbCapacity;   //空间大小
    char  *rbHead;          //头
    char  *rbTail;          //尾
    char  *rbBuff;          //数组的首地址
}rb_t;

typedef struct{
    rb_t Buff;              //管理:缓存数据数组
    rb_t ManageBuff;        //管理:每次缓存数据的数组
    
    signed int  Count;
    signed int  Cnt;
    unsigned char ReadFlage;
    unsigned char SendFlage;
    signed int  ReadLen;
    signed int  SendLen;
        
    int32_t value;      //内部使用,用户不可使用
    signed int  Len;    //内部使用,用户不可使用
}buff_manage_struct;

void rbCreate(rb_t *rb,void *Buff,uint32_t BuffLen);    //创建或者说初始化环形缓冲区
void rbDelete(rb_t* rb);
int32_t rbCapacity(rb_t *rb);                           //得到环形大小
int32_t rbCanRead(rb_t *rb);                            //能读出数据的个数
int32_t rbCanWrite(rb_t *rb);                           //还剩余的空间
int32_t rbRead(rb_t *rb, void *data, uint32_t count);   //读取数据
int32_t rbWrite(rb_t *rb, const void *data, uint32_t count);
int32_t PutData(rb_t *rb ,void *buf, uint32_t len);

//数据缓存处理
void BufferManageCreate(buff_manage_struct *bms,void *buff,uint32_t BuffLen,void *ManageBuff,uint32_t ManageBuffLen);
void BufferManageWrite(buff_manage_struct *bms,void *buff,uint32_t BuffLen,int *DataLen);
void BufferManageRead(buff_manage_struct *bms,void *buff,int *DataLen);

#endif
