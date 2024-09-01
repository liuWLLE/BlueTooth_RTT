#include "LoopList.h"

/**************************************************************************
�������ܣ��������ζ���
��ڲ�����rb:���ζ��ж���Buff:�������ݴ洢�ռ䡢BuffLen�����д�С
����  ֵ����
**************************************************************************/
void rbCreate(rb_t* rb,void *Buff,uint32_t BuffLen)
{
    if(NULL == rb)              //����Ϊ��
    {
        //printf("ERROR: input rb is NULL\n");
        return;
    }
    rb->rbCapacity = BuffLen;
    rb->rbBuff = Buff;
    rb->rbHead = rb->rbBuff;    //ͷָ�������׵�ַ
    rb->rbTail = rb->rbBuff;    //βָ�������׵�ַ
}

/**************************************************************************
�������ܣ�ɾ�����ζ���
��ڲ�����rb:���ζ��ж���
����  ֵ����
**************************************************************************/
void rbDelete(rb_t* rb)
{
    if(NULL == rb)          //����Ϊ��
    {
        //printf("ERROR: input rb is NULL\n");
        return;
    }
    rb->rbBuff = NULL;      //��ַ��ֵΪ��
    rb->rbHead = NULL;      //ͷ��ַΪ��
    rb->rbTail = NULL;      //β��ַβ��
    rb->rbCapacity = 0;     //����Ϊ��
}

/**************************************************************************
�������ܣ���ȡ����ĳ���
��ڲ�����rb:���ζ��ж���
����  ֵ����
**************************************************************************/
int32_t rbCapacity(rb_t *rb)
{
    if(NULL == rb)          //����Ϊ��
    {
        //printf("ERROR: input rb is NULL\n");
        return -1;
    }
    return rb->rbCapacity;
}

/**************************************************************************
�������ܣ������ܶ��Ŀռ�
��ڲ�����rb:���ζ��ж���
����  ֵ����
**************************************************************************/
int32_t rbCanRead(rb_t *rb)
{
    if(NULL == rb)                  //����Ϊ��
    {
        //printf("ERROR: input rb is NULL\n");
        return -1;
    }
    if (rb->rbHead == rb->rbTail)   //ͷ��β����
    {
        return 0;
    }
    if (rb->rbHead < rb->rbTail)    //β����ͷ
    {
        return rb->rbTail - rb->rbHead;
    }
    return rbCapacity(rb) - (rb->rbHead - rb->rbTail);  //ͷ����β
}

/**************************************************************************
�������ܣ�������д��Ŀռ�
��ڲ�����rb:���ζ��ж���
����  ֵ����
**************************************************************************/
int32_t rbCanWrite(rb_t *rb)
{
    if(NULL == rb)          //����Ϊ��
    {
        //printf("ERROR: input rb is NULL\n");
        return -1;
    }
    return rbCapacity(rb) - rbCanRead(rb);  //�ܵļ�ȥ�Ѿ�д��Ŀռ�
}

/**************************************************************************
�������ܣ���ȡ��������
��ڲ�����rb:���ζ��ж���data:Ҫ��ȡ�����ݡ�count:���ĸ���
����  ֵ�����ض�ȡ�ĸ���
**************************************************************************/
int32_t rbRead(rb_t *rb, void *data, uint32_t count)
{
    int copySz = 0;
    if(NULL == rb)          //����Ϊ��
    {
        //printf("ERROR: input rb is NULL\n");
        return -1;
    }
    if(NULL == data)        //���ݲ�Ϊ��
    {
        //printf("ERROR: input data is NULL\n");
        return -1;
    }
    if(rb->rbHead < rb->rbTail)             //β����ͷ
    {
        copySz = min(count, rbCanRead(rb)); //�鿴�ܶ��ĸ���
        memcpy(data,rb->rbHead,copySz);     //�������ݵ�data
        rb->rbHead += copySz;               //ͷָ����϶�ȡ�ĸ���
        return copySz;                      //���ض�ȡ�ĸ���
    }
    else                                    //ͷ���ڵ�����β
    {
        if (count < rbCapacity(rb)-(rb->rbHead-rb->rbBuff))         //���ĸ���С��ͷ�����������
        {
            copySz = count;                 //�����ĸ���
            memcpy(data,rb->rbHead,copySz);
            rb->rbHead += copySz;
            return copySz;
        }
        else                                //���ĸ�������ͷ�����������
        {
            copySz = rbCapacity(rb)-(rb->rbHead-rb->rbBuff);        //�ȶ�����ͷ���������
            memcpy(data,rb->rbHead,copySz);
            rb->rbHead = rb->rbBuff;                                //ͷָ��ָ��������׵�ַ
            copySz += rbRead(rb,(char*)data+copySz,count-copySz);   //���Ŷ�ʣ��Ҫ���ĸ���
            return copySz;
        }
    }
}

/**************************************************************************
�������ܣ�д���������
��ڲ�����rb:���ζ��ж���data:д������ݡ�count:д�ĸ���
����  ֵ��ʣ��ռ�
**************************************************************************/
int32_t rbWrite(rb_t *rb, const void *data, uint32_t count)
{
    int tailAvailSz = 0;
    if(NULL == rb)              //����Ϊ��
    {
        //printf("ERROR: rb is empty \n");
        return -1;
    }
    if(NULL == data)            //д������ݲ�Ϊ��
    {
        //printf("ERROR: data is empty \n");
        return -1;
    }
    if(count >= rbCanWrite(rb)) //���ʣ��Ŀռ䲻��
    {
        //printf("ERROR: no memory \n");
        return -1;
    }
    if(rb->rbHead <= rb->rbTail)                    //ͷС�ڵ���β
    {
        tailAvailSz = rbCapacity(rb)-(rb->rbTail-rb->rbBuff);   //�鿴β����ʣ��Ŀռ�
        if (count <= tailAvailSz)                   //����С�ڵ���β����ʣ��Ŀռ�
        {
            memcpy(rb->rbTail,data,count);          //�������ݵ���������
            rb->rbTail += count;                    //βָ��������ݸ���
            if(rb->rbTail == rb->rbBuff+rbCapacity(rb))         //����д�����
            {
                rb->rbTail = rb->rbBuff;            //βָ��������׵�ַ
            }
            return count;                           //����д������ݸ���
        }
        else
        {
            memcpy(rb->rbTail,data,tailAvailSz);    //����β����ʣ��Ŀռ�
            rb->rbTail = rb->rbBuff;                //βָ��ָ�������׵�ַ
                    //ʣ��ռ�                   ʣ�����ݵ��׵�ַ       ʣ�����ݵĸ���
            return tailAvailSz + rbWrite(rb,(char*)data+tailAvailSz,count-tailAvailSz);//����дʣ�������
        }
    }
    else            //ͷ����β
    {
      memcpy(rb->rbTail,data,count);
      rb->rbTail += count;
      return count;
    }
}

/**************************************************************************
�������ܣ������ζ�������д������
��ڲ�����rb:���ζ��ж���data:д������ݡ�len:���ݳ���
����  ֵ��д��ɹ���� -1:����
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
�������ܣ��������ݻ������
��ڲ�����bms:�������ṹ�������buff:�������ݵ����顢BuffLen:���鳤�ȡ�
          ManageBuff:��¼ÿ�λ�������ֽڵ����顢ManageBuffLen:��¼ÿ�λ�������ֽڵ����鳤��
����  ֵ����
ע�����__disable_irq();//�������п����ε��ж�
**************************************************************************/
void BufferManageCreate(buff_manage_struct *bms,void *buff,uint32_t BuffLen,void *ManageBuff,uint32_t ManageBuffLen)
{
    __disable_irq();
    rbCreate(&(bms->Buff),buff,BuffLen);                    //����1����洢���յ�����
    rbCreate(&(bms->ManageBuff),ManageBuff,ManageBuffLen);  //����2����洢�������ݵĸ���
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
�������ܣ�д�뻺������
��ڲ�����bms:�������ṹ�������buff:д������ݡ�BuffLen:д������ݸ�����
          DataLen  ����: 0 Success;1:��������;2:���ݻ�����
����  ֵ����
ע�����__disable_irq();//�������п����ε��ж�
**************************************************************************/
void BufferManageWrite(buff_manage_struct *bms,void *buff,uint32_t BuffLen,int *DataLen)
{
    __disable_irq();
    if(rbCanWrite(&(bms->Buff)) > BuffLen)          //����д������
    {
        if(rbCanWrite(&(bms->ManageBuff)) > 4)      //���Լ�¼���ݸ���
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
�������ܣ��ӻ����ж�ȡ����
��ڲ�����bms:�������ṹ�������buff:���ص����ݵ�ַ��DataLen;��ȡ�����ݸ�����DataLen:��ȡ�����ݸ���
����  ֵ��ȡ�������ݸ���
ע�����__disable_irq();//�������п����ε��ж�
**************************************************************************/
void BufferManageRead(buff_manage_struct *bms,void *buff,int *DataLen)
{
    __disable_irq();
    *DataLen = 0;
    if(rbCanRead(&(bms->ManageBuff)) >= 4)
    {
        rbRead(&(bms->ManageBuff),&(bms->Len),4);   //��������������ݸ���
        if(bms->Len > 0)
        {
            *DataLen = rbRead(&(bms->Buff),buff, bms->Len); 
        }
    }
    __enable_irq();
}

