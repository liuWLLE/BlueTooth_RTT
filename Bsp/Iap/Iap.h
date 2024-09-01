#ifndef __IAP_H__
#define __IAP_H__

#include "sys.h"  

//检查地址
#define SRAM_START_ADDRESS      0x2FFF0000      //SRAM起始地址
#define STACK_CHECK_ADDRESS     0x2FFF0000      //APP程序栈顶检查地址
#define FLASH_START_ADDRESS     0xFFFF0000      //FLASH起始地址
#define RESET_CHECK_ADDRESS     0xFFFF0000      //APP程序复位检查地址

//Boot程序参数
#define FLASH_CAPACITY          256             //单片机Flash容量
#define FLASH_BOOT_SIZE         30              //bootloader程序占用容量大小(KB)
#define FLASH_DATA_SIZE         2               //存储用户数据所用容量大小(KB)  
#define FLASH_APP_ADDR          0x08007800      //用户程序运行地址
#define FLASH_SHIFT_ADDR        0x7800          //中断向量表偏移地址

//用户程序可用大小 = (总FLASH容量 - 用户数据存储容量大小(KB) - bootloader占用大小(KB))
#define FLASH_USER_SIZE         (FLASH_CAPACITY - FLASH_DATA_SIZE - FLASH_BOOT_SIZE)

//存储用户数据地址：FLASH的起始地址 + (总FLASH容量 - 用户数据存储容量大小(KB) + 2))
#define FLASH_DATA_ADDR         (FLASH_BASE + 1024*(FLASH_CAPACITY - FLASH_DATA_SIZE) + 2) 

void FlashSetFlag_IAP(u32 WriteAddr,bool Flag);                 //在指定地址写入程序升级标志
void AddressJump_IAP(u32 AppXaddr);                             //跳转到flash里面的app程序地址
void FlashWriteBin_IAP(u32 AppXaddr,u8 *AppBuf,u32 AppSize);    //在指定地址写入bin文件

#endif

