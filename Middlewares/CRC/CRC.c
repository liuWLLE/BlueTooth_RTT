#include "CRC.h"

/**************************************************************************
函数功能：发送带有CRC16校验值的数据
入口参数：data：数据值、length：数据长度、func：发送函数
返回  值：CRC16校验值
**************************************************************************/
void CRC16_ModifyData(char *data,short int length)
{
    short int CRCValue;
    CRCValue = CRC16_CalcuValue(data,length);
    data[length] = CRCValue >> 8;   //CRC16校验值的高八位
    data[length + 1] = CRCValue;    //CRC16校验值的低八位
}

/**************************************************************************
函数功能：计算CRC16校验值
入口参数：data：数据值、length：数据长度
返回  值：CRC16校验值
**************************************************************************/
short int CRC16_CalcuValue(char *data,short int length)
{
    //校验初始值，也可以为0，不同初始值有不同的校验效果
    short int crc = 0xffff;
    for (short int i = 0; i < length; i++)
    {
        crc ^= data[i] & 0xff;
        for (short int j = 0; j < 8; j++)
        {
            if((crc & 0x01) == 1)
            {
                //反射多项式a001，对应CRC-16-IBM多项式8005
                crc = (crc >> 1) ^ 0xa001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}

/**************************************************************************
函数功能：验算CRC16
入口参数：data：数据值、length：数据长度
返回  值：CRC16校验值
**************************************************************************/
bool CRC16_CheckValue(char *data,short int length)  
{
    short int ReceiveCRC = 0, Calculation = 0;
    if(length <= 2)
    {
        return false;
    }
    ReceiveCRC = CRC16_CalcuValue(data,length - 2);
    Calculation = data[length - 2];
    Calculation <<= 8;
    Calculation += data[length - 1];
    if(Calculation != ReceiveCRC)
    {
        return false;
    }
    return true;
}

