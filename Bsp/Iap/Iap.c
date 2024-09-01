#include "Iap.h"

// Hardware
#include "Flash.h"

/**************************************************************************
�������ܣ���ָ����ַд�����������־
��ڲ�����WriteAddr����ʼ��ַ��Flag������������־
����  ֵ����
ע�������
**************************************************************************/
void FlashSetFlag_IAP(u32 WriteAddr,bool Flag) 
{
    if(Flag)
    {
        FLASH_Write(WriteAddr,&*(u16*)1,1);     //���ø��±�־ 
    }
    else
    {
        FLASH_Write(WriteAddr,&*(u16*)0,1);     //���ø��±�־
    }
    sys_soft_reset();                           //������Ƭ��
}

/**************************************************************************
�������ܣ���ָ����ַд��bin�ļ�
��ڲ�����appxaddr��app�������ʼ��ַ��appbuf��app�������ݡ�appsize��app�����С(�ֽ�)
����  ֵ����
ע�����һ����д������bin�ļ�
**************************************************************************/
void FlashWriteBin_IAP(u32 AppXaddr,u8 *AppBuf,u32 AppSize)
{
    u16 AppTempbuf[32];                             //app���򻺴���
    u16 count = 0;                                  //��ַ����ֵ
    u16 TempCode;                                   //app�����ݴ�ֵ
    u32 RealAddr = AppXaddr;                        //��ǰд��ĵ�ַ
    u8 *dfu = AppBuf;
    for(u16 t = 0;t < AppSize;t += 2)
    {
        TempCode = (u16)dfu[1] << 8;
        TempCode += (u16)dfu[0];
        dfu += 2;                                   //ƫ��2���ֽ�
        AppTempbuf[count++] = TempCode;
        if(count == 1024)
        {
            count = 0;
            FLASH_Write(RealAddr,AppTempbuf,1024);  //app��������д��flash
            RealAddr += 2048;                       //ƫ��2048 16=2*8 ����Ҫ����2
        }
    }
    if(count)
    {
        FLASH_Write(RealAddr,AppTempbuf,count);     //��ĩβ�������ֽ�д��
    }  
}

/**************************************************************************
�������ܣ���ת��Ӧ�ó����
��ڲ�����appxaddr��app������ʼ��ַ
����  ֵ����
ע�������
**************************************************************************/
void AddressJump_IAP(u32 AppXaddr)
{
    typedef void (*IapFun)(void);                   //����һ���������͵�ָ��
    IapFun Jump2App;                                //��������ָ�����
    //���APP��ջ����ַ�븴λ��ַ�Ƿ�Ϸ�
    if(((*(vu32*)AppXaddr) & STACK_CHECK_ADDRESS) == SRAM_START_ADDRESS && \
       ((*(vu32*)(AppXaddr+4)) & RESET_CHECK_ADDRESS) == FLASH_START_ADDRESS)
    {
        Jump2App = (IapFun)*(vu32*)(AppXaddr + 4);  //�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)
        __set_MSP(*(vu32*)AppXaddr);                //������ջָ��MSP����ΪAPP�����ջ����ַ
        Jump2App();                                 //��ת��app
    }
}
