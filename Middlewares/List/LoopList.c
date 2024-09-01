#include "LoopList.h"

/**************************************************************************
函数功能：创建环形队列
入口参数：rb:环形队列对象、Buff:队列数据存储空间、BuffLen：队列大小
返回  值：无
**************************************************************************/
void rbCreate(rb_t* rb,void *Buff,uint32_t BuffLen)
{
    if(NULL == rb)              //对象不为空
    {
        //printf("ERROR: input rb is NULL\n");
        return;
    }
    rb->rbCapacity = BuffLen;
    rb->rbBuff = Buff;
    rb->rbHead = rb->rbBuff;    //头指向数组首地址
    rb->rbTail = rb->rbBuff;    //尾指向数组首地址
}

/**************************************************************************
函数功能：删除环形队列
入口参数：rb:环形队列对象
返回  值：无
**************************************************************************/
void rbDelete(rb_t* rb)
{
    if(NULL == rb)          //对象不为空
    {
        //printf("ERROR: input rb is NULL\n");
        return;
    }
    rb->rbBuff = NULL;      //地址赋值为空
    rb->rbHead = NULL;      //头地址为空
    rb->rbTail = NULL;      //尾地址尾空
    rb->rbCapacity = 0;     //长度为空
}

/**************************************************************************
函数功能：获取链表的长度
入口参数：rb:环形队列对象
返回  值：无
**************************************************************************/
int32_t rbCapacity(rb_t *rb)
{
    if(NULL == rb)          //对象不为空
    {
        //printf("ERROR: input rb is NULL\n");
        return -1;
    }
    return rb->rbCapacity;
}

/**************************************************************************
函数功能：返回能读的空间
入口参数：rb:环形队列对象
返回  值：无
**************************************************************************/
int32_t rbCanRead(rb_t *rb)
{
    if(NULL == rb)                  //对象不为空
    {
        //printf("ERROR: input rb is NULL\n");
        return -1;
    }
    if (rb->rbHead == rb->rbTail)   //头与尾相遇
    {
        return 0;
    }
    if (rb->rbHead < rb->rbTail)    //尾大于头
    {
        return rb->rbTail - rb->rbHead;
    }
    return rbCapacity(rb) - (rb->rbHead - rb->rbTail);  //头大于尾
}

/**************************************************************************
函数功能：返回能写入的空间
入口参数：rb:环形队列对象
返回  值：无
**************************************************************************/
int32_t rbCanWrite(rb_t *rb)
{
    if(NULL == rb)          //对象不为空
    {
        //printf("ERROR: input rb is NULL\n");
        return -1;
    }
    return rbCapacity(rb) - rbCanRead(rb);  //总的减去已经写入的空间
}

/**************************************************************************
函数功能：读取队列内容
入口参数：rb:环形队列对象、data:要读取的数据、count:读的个数
返回  值：返回读取的个数
**************************************************************************/
int32_t rbRead(rb_t *rb, void *data, uint32_t count)
{
    int copySz = 0;
    if(NULL == rb)          //对象不为空
    {
        //printf("ERROR: input rb is NULL\n");
        return -1;
    }
    if(NULL == data)        //数据不为空
    {
        //printf("ERROR: input data is NULL\n");
        return -1;
    }
    if(rb->rbHead < rb->rbTail)             //尾大于头
    {
        copySz = min(count, rbCanRead(rb)); //查看能读的个数
        memcpy(data,rb->rbHead,copySz);     //读出数据到data
        rb->rbHead += copySz;               //头指针加上读取的个数
        return copySz;                      //返回读取的个数
    }
    else                                    //头大于等于了尾
    {
        if (count < rbCapacity(rb)-(rb->rbHead-rb->rbBuff))         //读的个数小于头上面的数据量
        {
            copySz = count;                 //读出的个数
            memcpy(data,rb->rbHead,copySz);
            rb->rbHead += copySz;
            return copySz;
        }
        else                                //读的个数大于头上面的数据量
        {
            copySz = rbCapacity(rb)-(rb->rbHead-rb->rbBuff);        //先读出来头上面的数据
            memcpy(data,rb->rbHead,copySz);
            rb->rbHead = rb->rbBuff;                                //头指针指向数组的首地址
            copySz += rbRead(rb,(char*)data+copySz,count-copySz);   //接着读剩余要读的个数
            return copySz;
        }
    }
}

/**************************************************************************
函数功能：写入队列内容
入口参数：rb:环形队列对象、data:写入的数据、count:写的个数
返回  值：剩余空间
**************************************************************************/
int32_t rbWrite(rb_t *rb, const void *data, uint32_t count)
{
    int tailAvailSz = 0;
    if(NULL == rb)              //对象不为空
    {
        //printf("ERROR: rb is empty \n");
        return -1;
    }
    if(NULL == data)            //写入的数据不为空
    {
        //printf("ERROR: data is empty \n");
        return -1;
    }
    if(count >= rbCanWrite(rb)) //如果剩余的空间不够
    {
        //printf("ERROR: no memory \n");
        return -1;
    }
    if(rb->rbHead <= rb->rbTail)                    //头小于等于尾
    {
        tailAvailSz = rbCapacity(rb)-(rb->rbTail-rb->rbBuff);   //查看尾上面剩余的空间
        if (count <= tailAvailSz)                   //个数小于等于尾上面剩余的空间
        {
            memcpy(rb->rbTail,data,count);          //拷贝数据到环形数组
            rb->rbTail += count;                    //尾指针加上数据个数
            if(rb->rbTail == rb->rbBuff+rbCapacity(rb))         //正好写到最后
            {
                rb->rbTail = rb->rbBuff;            //尾指向数组的首地址
            }
            return count;                           //返回写入的数据个数
        }
        else
        {
            memcpy(rb->rbTail,data,tailAvailSz);    //填入尾上面剩余的空间
            rb->rbTail = rb->rbBuff;                //尾指针指向数组首地址
                    //剩余空间                   剩余数据的首地址       剩余数据的个数
            return tailAvailSz + rbWrite(rb,(char*)data+tailAvailSz,count-tailAvailSz);//接着写剩余的数据
        }
    }
    else            //头大于尾
    {
      memcpy(rb->rbTail,data,count);
      rb->rbTail += count;
      return count;
    }
}

/**************************************************************************
函数功能：往环形队列里面写入数据
入口参数：rb:环形队列对象、data:写入的数据、len:数据长度
返回  值：写入成功与否 -1:错误
**************************************************************************/
int32_t PutData(rb_t *rb ,void *buf, uint32_t len)
{
    int32_t count = 0;
    if(NULL == buf)
    {
        //printf("ERROR: gizPutData buf is empty \n");
        return -1;
    }
    count = rbWrite(rb,buf,len);
    if(count != len)
    {
        //printf("ERROR: Failed to rbWrite \n");
        return -1;
    }
    return count;
}

/**************************************************************************
函数功能：创建数据缓存管理
入口参数：bms:缓存管理结构体变量、buff:缓存数据的数组、BuffLen:数组长度、
          ManageBuff:记录每次缓存多少字节的数组、ManageBuffLen:记录每次缓存多少字节的数组长度
返回  值：无
注意事项：__disable_irq();//禁用所有可屏蔽的中断
**************************************************************************/
void BufferManageCreate(buff_manage_struct *bms,void *buff,uint32_t BuffLen,void *ManageBuff,uint32_t ManageBuffLen)
{
    __disable_irq();
    rbCreate(&(bms->Buff),buff,BuffLen);                    //队列1负责存储接收的数据
    rbCreate(&(bms->ManageBuff),ManageBuff,ManageBuffLen);  //队列2负责存储接收数据的个数
    bms->Count = 0;
    bms->Cnt = 0;
    bms->ReadFlage = 0;
    bms->ReadLen = 0;
    bms->SendFlage = 0;
    bms->SendLen = 0;
    bms->value = 0;
    __enable_irq();
}

/**************************************************************************
函数功能：写入缓存数据
入口参数：bms:缓存管理结构体变量、buff:写入的数据、BuffLen:写入的数据个数、
          DataLen  返回: 0 Success;1:管理缓存满;2:数据缓存满
返回  值：无
注意事项：__disable_irq();//禁用所有可屏蔽的中断
**************************************************************************/
void BufferManageWrite(buff_manage_struct *bms,void *buff,uint32_t BuffLen,int *DataLen)
{
    __disable_irq();
    if(rbCanWrite(&(bms->Buff)) > BuffLen)          //可以写入数据
    {
        if(rbCanWrite(&(bms->ManageBuff)) > 4)      //可以记录数据个数
        {
            PutData(&(bms->Buff),buff,BuffLen);
            PutData(&(bms->ManageBuff),&BuffLen,4);
            *DataLen = 0;
        }
        else
        {
            *DataLen = -1;
        }
    }
    else
    {
        *DataLen = -2;
    }
    __enable_irq();
}

/**************************************************************************
函数功能：从缓存中读取数据
入口参数：bms:缓存管理结构体变量、buff:返回的数据地址、DataLen;读取的数据个数、DataLen:读取的数据个数
返回  值：取出的数据个数
注意事项：__disable_irq();//禁用所有可屏蔽的中断
**************************************************************************/
void BufferManageRead(buff_manage_struct *bms,void *buff,int *DataLen)
{
    __disable_irq();
    *DataLen = 0;
    if(rbCanRead(&(bms->ManageBuff)) >= 4)
    {
        rbRead(&(bms->ManageBuff),&(bms->Len),4);   //读出来存入的数据个数
        if(bms->Len > 0)
        {
            *DataLen = rbRead(&(bms->Buff),buff, bms->Len); 
        }
    }
    __enable_irq();
}

