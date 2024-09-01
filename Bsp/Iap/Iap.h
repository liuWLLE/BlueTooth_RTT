#ifndef __IAP_H__
#define __IAP_H__

#include "sys.h"  

//����ַ
#define SRAM_START_ADDRESS      0x2FFF0000      //SRAM��ʼ��ַ
#define STACK_CHECK_ADDRESS     0x2FFF0000      //APP����ջ������ַ
#define FLASH_START_ADDRESS     0xFFFF0000      //FLASH��ʼ��ַ
#define RESET_CHECK_ADDRESS     0xFFFF0000      //APP����λ����ַ

//Boot�������
#define FLASH_CAPACITY          256             //��Ƭ��Flash����
#define FLASH_BOOT_SIZE         30              //bootloader����ռ��������С(KB)
#define FLASH_DATA_SIZE         2               //�洢�û���������������С(KB)  
#define FLASH_APP_ADDR          0x08007800      //�û��������е�ַ
#define FLASH_SHIFT_ADDR        0x7800          //�ж�������ƫ�Ƶ�ַ

//�û�������ô�С = (��FLASH���� - �û����ݴ洢������С(KB) - bootloaderռ�ô�С(KB))
#define FLASH_USER_SIZE         (FLASH_CAPACITY - FLASH_DATA_SIZE - FLASH_BOOT_SIZE)

//�洢�û����ݵ�ַ��FLASH����ʼ��ַ + (��FLASH���� - �û����ݴ洢������С(KB) + 2))
#define FLASH_DATA_ADDR         (FLASH_BASE + 1024*(FLASH_CAPACITY - FLASH_DATA_SIZE) + 2) 

void FlashSetFlag_IAP(u32 WriteAddr,bool Flag);                 //��ָ����ַд�����������־
void AddressJump_IAP(u32 AppXaddr);                             //��ת��flash�����app�����ַ
void FlashWriteBin_IAP(u32 AppXaddr,u8 *AppBuf,u32 AppSize);    //��ָ����ַд��bin�ļ�

#endif

