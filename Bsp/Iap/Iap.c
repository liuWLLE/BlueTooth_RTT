#include "Iap.h"

// Hardware
#include "Flash.h"

/**************************************************************************
函数功能：在指定地址写入程序升级标志
入口参数：WriteAddr：起始地址、Flag：程序升级标志
返回  值：无
注意事项：无
**************************************************************************/
void FlashSetFlag_IAP(u32 WriteAddr,bool Flag) 
{
    if(Flag)
    {
        FLASH_Write(WriteAddr,&*(u16*)1,1);     //设置更新标志 
    }
    else
    {
        FLASH_Write(WriteAddr,&*(u16*)0,1);     //设置更新标志
    }
    sys_soft_reset();                           //重启单片机
}

/**************************************************************************
函数功能：在指定地址写入bin文件
入口参数：appxaddr：app程序的起始地址、appbuf：app程序内容、appsize：app程序大小(字节)
返回  值：无
注意事项：一次性写入整个bin文件
**************************************************************************/
void FlashWriteBin_IAP(u32 AppXaddr,u8 *AppBuf,u32 AppSize)
{
    u16 AppTempbuf[32];                             //app程序缓存区
    u16 count = 0;                                  //地址计数值
    u16 TempCode;                                   //app内容暂存值
    u32 RealAddr = AppXaddr;                        //当前写入的地址
    u8 *dfu = AppBuf;
    for(u16 t = 0;t < AppSize;t += 2)
    {
        TempCode = (u16)dfu[1] << 8;
        TempCode += (u16)dfu[0];
        dfu += 2;                                   //偏移2个字节
        AppTempbuf[count++] = TempCode;
        if(count == 1024)
        {
            count = 0;
            FLASH_Write(RealAddr,AppTempbuf,1024);  //app程序内容写入flash
            RealAddr += 2048;                       //偏移2048 16=2*8 所以要乘以2
        }
    }
    if(count)
    {
        FLASH_Write(RealAddr,AppTempbuf,count);     //将末尾的内容字节写入
    }  
}

/**************************************************************************
函数功能：跳转到应用程序段
入口参数：appxaddr：app代码起始地址
返回  值：无
注意事项：无
**************************************************************************/
void AddressJump_IAP(u32 AppXaddr)
{
    typedef void (*IapFun)(void);                   //定义一个函数类型的指针
    IapFun Jump2App;                                //声明函数指针变量
    //检查APP的栈顶地址与复位地址是否合法
    if(((*(vu32*)AppXaddr) & STACK_CHECK_ADDRESS) == SRAM_START_ADDRESS && \
       ((*(vu32*)(AppXaddr+4)) & RESET_CHECK_ADDRESS) == FLASH_START_ADDRESS)
    {
        Jump2App = (IapFun)*(vu32*)(AppXaddr + 4);  //用户代码区第二个字为程序开始地址(复位地址)
        __set_MSP(*(vu32*)AppXaddr);                //将主堆栈指针MSP设置为APP程序的栈顶地址
        Jump2App();                                 //跳转到app
    }
}
