#ifndef __CRC_H
#define __CRC_H

#include "sys.h"

void CRC16_ModifyData(char *data,short int length);           //发送带有CRC校验值的数据
short int CRC16_CalcuValue(char *data,short int length);      //计算CRC16校验值
bool CRC16_CheckValue(char *data,short int length);           //验算CRC16

#endif
