#ifndef __FLASH_H__
#define __FLASH_H__

#include "sys.h"

/*
�����豸�������ͺ������壺
1��Flash��ҳ��С
2��Flash�������ֽ���
3��Flash�ܴ�С��HEX��
4��Flash�ܴ�С��DEC��
*/
#define PAGE_SIZE               (0x800)     // ҳ��С 2 Kbytes
#define SECTOR_SIZE             (2048)      // ������С 2048 Byte
#define FLASH_SIZE              (0x80000)   // �ܴ�С 512 KBytes
#define FLASH_SIZE_INTEGER      (256)       // �ܴ�С 256 KBytes

//Flash����
#define FLASH_WREN                          //�Ƿ�ʹ��FLASHд��(0������ʹ��;1��ʹ��)
#define FLASH_BASE_SET      0x08000000      //FLASH����ʼ��ַ  

u32 FLASH_PagesMask(volatile u32 Size);                                 //����FlashҪ������ҳ��
u16 FLASH_ReadHalfWord(u32 faddr);                                      //��������  
void FLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);    //������д��flash
void FLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);            //��ָ����ַ��ʼд��ָ�����ȵ�����
char FLASH_WriteHalfWord(uint32_t WriteAddress,u16 data);               //ָ����ַд��һ��16λ����
void FLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);               //��ָ����ַ��ʼ����ָ�����ȵ�����
char FLASH_Erase(uint32_t EraseAddress,u16 PageCnt);                    //����Flashָ��ҳ��

#endif

