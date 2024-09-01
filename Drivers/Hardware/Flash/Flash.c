#include "Flash.h"

u16 FLASH_BUF[SECTOR_SIZE / 2];     //�����2K�ֽ�

/**************************************************************************
�������ܣ�����FlashҪ������ҳ��
��ڲ�����Size�������С
����  ֵ��Ҫ������ҳ��
ע�������
**************************************************************************/
u32 FLASH_PagesMask(volatile u32 Size)
{
    u32 pagenumber = 0x00;
    u32 size = Size;

    if((size % PAGE_SIZE) != 0)
    {
        pagenumber = (size / PAGE_SIZE) + 1;
    }
    else
    {
        pagenumber = size / PAGE_SIZE;
    }
    return pagenumber;
}

/**************************************************************************
�������ܣ���ȡָ����ַ�İ���(16λ����)
��ڲ�����faddr������ַ
����  ֵ����Ӧ����
ע�����flash��ȡ�ĵ�ַ����Ϊ2�ı���
**************************************************************************/
u16 FLASH_ReadHalfWord(u32 faddr)
{
    return *(vu16*)faddr; 
}

/**************************************************************************
�������ܣ�������д��flash
��ڲ�����WriteAddr����ʼ��ַ��pBuffer������ָ�롢NumToWrite������(16λ)��   
����  ֵ����
ע���������(16λ)������Ҫд���16λ���ݵĸ��� 
**************************************************************************/
#if defined(FLASH_WREN)
void FLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{
    for(u16 i = 0;i < NumToWrite;i++)
    {
        FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
        WriteAddr += 2;                                 //��ַ����2
    }  
} 

/**************************************************************************
�������ܣ���ָ����ַ��ʼд��ָ�����ȵ����ݵ�flash
��ڲ�����WriteAddr����ʼ��ַ��pBuffer������ָ�롢NumToWrite������(16λ)��
����  ֵ����
ע�����flashд��ĵ�ַ����Ϊ2�ı���������(16λ)������Ҫд���16λ���ݵĸ��� 
          flash�Ĳ������Ȳ�����д��
**************************************************************************/
void FLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)
{
    u16 i;              //ѭ������
    u32 secpos;         //������ַ
    u16 secoff;         //������ƫ�Ƶ�ַ(16λ�ּ���)
    u16 secremain;      //������ʣ���ַ(16λ�ּ���)
    u32 offaddr;        //ȥ��0X08000000��ĵ�ַ
    
    if(WriteAddr<FLASH_BASE_SET || (WriteAddr>=(FLASH_BASE_SET+1024*FLASH_SIZE_INTEGER)))
    {
        return;                                 //�Ƿ���ַ
    }
    FLASH_Unlock();                             //flash����ǰ��Ҫ����
    offaddr = WriteAddr - FLASH_BASE_SET;       //ʵ��ƫ�Ƶ�ַ.
    secpos = offaddr / SECTOR_SIZE;             //������ַ��0~127 for STM32F103RBT6��
    secoff = (offaddr % SECTOR_SIZE) / 2;       //�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
    secremain = SECTOR_SIZE / 2 - secoff;       //����ʣ��ռ��С   
    if(NumToWrite <= secremain)
    {
        secremain = NumToWrite;                 //�����ڸ�������Χ
    }
    while(true) 
    {
        //������������������
        FLASH_Read(secpos*SECTOR_SIZE+FLASH_BASE_SET,FLASH_BUF,SECTOR_SIZE/2);
        for(i = 0;i < secremain;i++)            //У������
        {
            if(FLASH_BUF[secoff + i] != 0XFFFF)
            {
                break;                          //��Ҫ����
            }
        }
        if(i < secremain)                       //��Ҫ����
        {
            //����ָ����ַ������
            FLASH_ErasePage(secpos*SECTOR_SIZE + FLASH_BASE_SET);
            for(i = 0;i < secremain;i++)        //����
            {
                FLASH_BUF[i+secoff] = pBuffer[i]; 
            }
            //д����������
            FLASH_Write_NoCheck(secpos*SECTOR_SIZE+FLASH_BASE_SET,FLASH_BUF,SECTOR_SIZE/2);
        }
        else
        {
            //д�Ѿ������˵�,ֱ��д������ʣ������
            FLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);
        }            
        if(NumToWrite == secremain)
        {
            break;                              //д�������
        }
        else                                    //д��δ����
        {       
            secpos++;                           //������ַ��1
            secoff = 0;                         //ƫ��λ��Ϊ0
            pBuffer += secremain;               //ָ��ƫ��
            WriteAddr += secremain;             //д��ַƫ��
            NumToWrite -= secremain;            //�ֽ�(16λ)���ݼ�
            if(NumToWrite > (SECTOR_SIZE/2))
            {
                secremain = SECTOR_SIZE/2;      //��һ����������д����
            }
            else
            {
                secremain=NumToWrite;           //��һ����������д����
            }
        } 
    }
    FLASH_Lock();                               //flash������ɺ���Ҫ����
}

/**************************************************************************
�������ܣ�ָ����ַд��һ��16λ����
��ڲ�����WriteAddress��д��ĵ�ַ��data��д�������
����  ֵ��0:�ɹ�
ע�����д��ĵ�ַ���Ȳ���
**************************************************************************/
char FLASH_WriteHalfWord(uint32_t WriteAddress,u16 data)
{
    FLASH_Status FlashStatus;                                   //Flash��д״̬�ṹ��
    FlashStatus = FLASH_BUSY;                                   //����Ϊæ
    if(FLASH_GetStatus() == FLASH_COMPLETE)                     //���Բ���Flash
    {
        FLASH_Unlock();                                         //Flash����
        FlashStatus = FLASH_ProgramHalfWord(WriteAddress,data); //д������
        if(FlashStatus == FLASH_COMPLETE)                       //�������
        {
            if(FLASH_ReadHalfWord(WriteAddress) == data)        //�����ĺ�д���һ��
                FlashStatus = (FLASH_Status)0;                  //������д���һ��
            else
                FlashStatus = (FLASH_Status)5;
        }
        FLASH_Lock();                                           //Flash����
    }
    return FlashStatus;                                         //����д��״̬
}
#endif

/**************************************************************************
�������ܣ���ָ����ַ��ʼ����ָ�����ȵ�����
��ڲ�����ReadAddr:��ʼ��ַ��pBuffer:����ָ�롢NumToRead:����(16λ)��
����  ֵ����
ע�������
**************************************************************************/
void FLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)
{
    for(u16 i = 0;i < NumToRead;i++)
    {
        pBuffer[i] = FLASH_ReadHalfWord(ReadAddr);      //��ȡ2���ֽ�
        ReadAddr += 2;                                  //ƫ��2���ֽ�
    }
}

/**************************************************************************
�������ܣ�����Flashָ��ҳ��
��ڲ�����EraseAddress��������ĵ�ַ��ַ��PageCnt�����������ļ�ҳ
����  ֵ�������Ƿ�ɹ�
ע�������
**************************************************************************/
char FLASH_Erase(uint32_t EraseAddress,u16 PageCnt) 
{
    u32 secpos;                 //������ַ
    FLASH_Status FlashStatus;   //Flash״̬
    if(EraseAddress<FLASH_BASE||(EraseAddress>=(FLASH_BASE+1024*FLASH_SIZE_INTEGER)))
    {
        return 0;                               //�Ƿ���ַ
    }
    secpos = EraseAddress - FLASH_BASE;         //ʵ�ʵ�ַ
    secpos = secpos/SECTOR_SIZE;                //������ַ 
    if(SECTOR_SIZE == 2048)
    {
        PageCnt = PageCnt / 2;
    }
    if(FLASH_GetStatus() == FLASH_COMPLETE)     //���Բ���Flash
    {
        FLASH_Unlock();                         //����Flash
        for(u32 i = 0;i < PageCnt;i++)
        {
            FlashStatus = FLASH_ErasePage(secpos*SECTOR_SIZE+FLASH_BASE);//�����������
            if(FlashStatus != FLASH_COMPLETE)
            {
                break;
            }
            secpos++;
        }
        FLASH_Lock();                           //Flash����
    }
    return FlashStatus;
} 

