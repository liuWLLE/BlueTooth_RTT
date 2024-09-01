#include "Flash.h"

u16 FLASH_BUF[SECTOR_SIZE / 2];     //最多是2K字节

/**************************************************************************
函数功能：计算Flash要擦除的页数
入口参数：Size：镜像大小
返回  值：要擦除的页数
注意事项：无
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
函数功能：读取指定地址的半字(16位数据)
入口参数：faddr：读地址
返回  值：对应数据
注意事项：flash读取的地址必须为2的倍数
**************************************************************************/
u16 FLASH_ReadHalfWord(u32 faddr)
{
    return *(vu16*)faddr; 
}

/**************************************************************************
函数功能：不检查的写入flash
入口参数：WriteAddr：起始地址、pBuffer：数据指针、NumToWrite：半字(16位)数   
返回  值：无
注意事项：半字(16位)数就是要写入的16位数据的个数 
**************************************************************************/
#if defined(FLASH_WREN)
void FLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{
    for(u16 i = 0;i < NumToWrite;i++)
    {
        FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
        WriteAddr += 2;                                 //地址增加2
    }  
} 

/**************************************************************************
函数功能：从指定地址开始写入指定长度的数据到flash
入口参数：WriteAddr：起始地址、pBuffer：数据指针、NumToWrite：半字(16位)数
返回  值：无
注意事项：flash写入的地址必须为2的倍数、半字(16位)数就是要写入的16位数据的个数 
          flash的操作是先擦除再写入
**************************************************************************/
void FLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)
{
    u16 i;              //循环次数
    u32 secpos;         //扇区地址
    u16 secoff;         //扇区内偏移地址(16位字计算)
    u16 secremain;      //扇区内剩余地址(16位字计算)
    u32 offaddr;        //去掉0X08000000后的地址
    
    if(WriteAddr<FLASH_BASE_SET || (WriteAddr>=(FLASH_BASE_SET+1024*FLASH_SIZE_INTEGER)))
    {
        return;                                 //非法地址
    }
    FLASH_Unlock();                             //flash操作前需要解锁
    offaddr = WriteAddr - FLASH_BASE_SET;       //实际偏移地址.
    secpos = offaddr / SECTOR_SIZE;             //扇区地址（0~127 for STM32F103RBT6）
    secoff = (offaddr % SECTOR_SIZE) / 2;       //在扇区内的偏移(2个字节为基本单位.)
    secremain = SECTOR_SIZE / 2 - secoff;       //扇区剩余空间大小   
    if(NumToWrite <= secremain)
    {
        secremain = NumToWrite;                 //不大于该扇区范围
    }
    while(true) 
    {
        //读出整个扇区的内容
        FLASH_Read(secpos*SECTOR_SIZE+FLASH_BASE_SET,FLASH_BUF,SECTOR_SIZE/2);
        for(i = 0;i < secremain;i++)            //校验数据
        {
            if(FLASH_BUF[secoff + i] != 0XFFFF)
            {
                break;                          //需要擦除
            }
        }
        if(i < secremain)                       //需要擦除
        {
            //擦除指定地址的扇区
            FLASH_ErasePage(secpos*SECTOR_SIZE + FLASH_BASE_SET);
            for(i = 0;i < secremain;i++)        //复制
            {
                FLASH_BUF[i+secoff] = pBuffer[i]; 
            }
            //写入整个扇区
            FLASH_Write_NoCheck(secpos*SECTOR_SIZE+FLASH_BASE_SET,FLASH_BUF,SECTOR_SIZE/2);
        }
        else
        {
            //写已经擦除了的,直接写入扇区剩余区间
            FLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);
        }            
        if(NumToWrite == secremain)
        {
            break;                              //写入结束了
        }
        else                                    //写入未结束
        {       
            secpos++;                           //扇区地址增1
            secoff = 0;                         //偏移位置为0
            pBuffer += secremain;               //指针偏移
            WriteAddr += secremain;             //写地址偏移
            NumToWrite -= secremain;            //字节(16位)数递减
            if(NumToWrite > (SECTOR_SIZE/2))
            {
                secremain = SECTOR_SIZE/2;      //下一个扇区还是写不完
            }
            else
            {
                secremain=NumToWrite;           //下一个扇区可以写完了
            }
        } 
    }
    FLASH_Lock();                               //flash操作完成后需要上锁
}

/**************************************************************************
函数功能：指定地址写入一个16位数据
入口参数：WriteAddress：写入的地址、data：写入的数据
返回  值：0:成功
注意事项：写入的地址请先擦除
**************************************************************************/
char FLASH_WriteHalfWord(uint32_t WriteAddress,u16 data)
{
    FLASH_Status FlashStatus;                                   //Flash读写状态结构体
    FlashStatus = FLASH_BUSY;                                   //设置为忙
    if(FLASH_GetStatus() == FLASH_COMPLETE)                     //可以操作Flash
    {
        FLASH_Unlock();                                         //Flash解锁
        FlashStatus = FLASH_ProgramHalfWord(WriteAddress,data); //写入数据
        if(FlashStatus == FLASH_COMPLETE)                       //操作完成
        {
            if(FLASH_ReadHalfWord(WriteAddress) == data)        //读出的和写入的一致
                FlashStatus = (FLASH_Status)0;                  //读出和写入的一致
            else
                FlashStatus = (FLASH_Status)5;
        }
        FLASH_Lock();                                           //Flash上锁
    }
    return FlashStatus;                                         //返回写入状态
}
#endif

/**************************************************************************
函数功能：从指定地址开始读出指定长度的数据
入口参数：ReadAddr:起始地址、pBuffer:数据指针、NumToRead:半字(16位)数
返回  值：无
注意事项：无
**************************************************************************/
void FLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)
{
    for(u16 i = 0;i < NumToRead;i++)
    {
        pBuffer[i] = FLASH_ReadHalfWord(ReadAddr);      //读取2个字节
        ReadAddr += 2;                                  //偏移2个字节
    }
}

/**************************************************************************
函数功能：擦除Flash指定页数
入口参数：EraseAddress：起擦除的地址地址、PageCnt：擦除连续的几页
返回  值：擦除是否成功
注意事项：无
**************************************************************************/
char FLASH_Erase(uint32_t EraseAddress,u16 PageCnt) 
{
    u32 secpos;                 //扇区地址
    FLASH_Status FlashStatus;   //Flash状态
    if(EraseAddress<FLASH_BASE||(EraseAddress>=(FLASH_BASE+1024*FLASH_SIZE_INTEGER)))
    {
        return 0;                               //非法地址
    }
    secpos = EraseAddress - FLASH_BASE;         //实际地址
    secpos = secpos/SECTOR_SIZE;                //扇区地址 
    if(SECTOR_SIZE == 2048)
    {
        PageCnt = PageCnt / 2;
    }
    if(FLASH_GetStatus() == FLASH_COMPLETE)     //可以操作Flash
    {
        FLASH_Unlock();                         //解锁Flash
        for(u32 i = 0;i < PageCnt;i++)
        {
            FlashStatus = FLASH_ErasePage(secpos*SECTOR_SIZE+FLASH_BASE);//擦除这个扇区
            if(FlashStatus != FLASH_COMPLETE)
            {
                break;
            }
            secpos++;
        }
        FLASH_Lock();                           //Flash上锁
    }
    return FlashStatus;
} 

