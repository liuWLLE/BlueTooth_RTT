#ifndef __FLASH_H__
#define __FLASH_H__

#include "sys.h"

/*
根据设备的容量型号来定义：
1、Flash的页大小
2、Flash的扇区字节数
3、Flash总大小（HEX）
4、Flash总大小（DEC）
*/
#define PAGE_SIZE               (0x800)     // 页大小 2 Kbytes
#define SECTOR_SIZE             (2048)      // 扇区大小 2048 Byte
#define FLASH_SIZE              (0x80000)   // 总大小 512 KBytes
#define FLASH_SIZE_INTEGER      (256)       // 总大小 256 KBytes

//Flash控制
#define FLASH_WREN                          //是否使能FLASH写入(0，不能使能;1，使能)
#define FLASH_BASE_SET      0x08000000      //FLASH的起始地址  

u32 FLASH_PagesMask(volatile u32 Size);                                 //计算Flash要擦除的页数
u16 FLASH_ReadHalfWord(u32 faddr);                                      //读出半字  
void FLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);    //不检查的写入flash
void FLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);            //从指定地址开始写入指定长度的数据
char FLASH_WriteHalfWord(uint32_t WriteAddress,u16 data);               //指定地址写入一个16位数据
void FLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);               //从指定地址开始读出指定长度的数据
char FLASH_Erase(uint32_t EraseAddress,u16 PageCnt);                    //擦除Flash指定页数

#endif

