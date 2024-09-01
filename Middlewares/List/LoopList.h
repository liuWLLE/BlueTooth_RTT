#ifndef __LOOPLIST_H
#define __LOOPLIST_H

#include "sys.h"

#define min(a,b) (a)<(b)?(a):(b)       //��ȡ��Сֵ

/** ���λ��������ݽṹ */
typedef struct {
    uint32_t  rbCapacity;   //�ռ��С
    char  *rbHead;          //ͷ
    char  *rbTail;          //β
    char  *rbBuff;          //������׵�ַ
}rb_t;

typedef struct{
    rb_t Buff;              //����:������������
    rb_t ManageBuff;        //����:ÿ�λ������ݵ�����
    
    signed int  Count;
    signed int  Cnt;
    unsigned char ReadFlage;
    unsigned char SendFlage;
    signed int  ReadLen;
    signed int  SendLen;
        
    int32_t value;      //�ڲ�ʹ��,�û�����ʹ��
    signed int  Len;    //�ڲ�ʹ��,�û�����ʹ��
}buff_manage_struct;

void rbCreate(rb_t *rb,void *Buff,uint32_t BuffLen);    //��������˵��ʼ�����λ�����
void rbDelete(rb_t* rb);
int32_t rbCapacity(rb_t *rb);                           //�õ����δ�С
int32_t rbCanRead(rb_t *rb);                            //�ܶ������ݵĸ���
int32_t rbCanWrite(rb_t *rb);                           //��ʣ��Ŀռ�
int32_t rbRead(rb_t *rb, void *data, uint32_t count);   //��ȡ����
int32_t rbWrite(rb_t *rb, const void *data, uint32_t count);
int32_t PutData(rb_t *rb ,void *buf, uint32_t len);

//���ݻ��洦��
void BufferManageCreate(buff_manage_struct *bms,void *buff,uint32_t BuffLen,void *ManageBuff,uint32_t ManageBuffLen);
void BufferManageWrite(buff_manage_struct *bms,void *buff,uint32_t BuffLen,int *DataLen);
void BufferManageRead(buff_manage_struct *bms,void *buff,int *DataLen);

#endif
