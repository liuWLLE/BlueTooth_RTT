#ifndef __LOBOTSERVO_H
#define __LOBOTSERVO_H

#include "sys.h"

#define GET_LOW_BYTE(A)     ((uint8_t)(A))          //宏函数 获得A的低八位
#define GET_HIGH_BYTE(A)    ((uint8_t)((A) >> 8))   //宏函数 获得A的高八位

#define FRAME_HEADER                    0x55        //帧头
#define CMD_SERVO_MOVE                  0x03        //舵机移动指令
#define CMD_ACTION_GROUP_RUN            0x06        //运行动作组指令
#define CMD_ACTION_GROUP_STOP           0x07        //停止动作组指令
#define CMD_ACTION_GROUP_SPEED          0x0B        //设置动作组运行速度
#define CMD_GET_BATTERY_VOLTAGE         0x0F        //获取电池电压指令
#define CMD_ACTION_GROUP_COMPLETE       0x08        //动作组自然停止指令

extern bool isUartRxCompleted;
extern uint8_t LobotRxBuf[16];

typedef struct _lobot_servo_ 
{
    uint8_t ID;             //舵机ID
    uint16_t Position;      //舵机目标位置
}LobotServo;

void LobotInit(void (*SendBufFunc)());
void moveServo(uint8_t servoID, uint16_t Position, uint16_t Time);
void moveServosByArray(LobotServo servos[], uint8_t Num, uint16_t Time);
void moveServos(uint8_t Num, uint16_t Time, ...);
void runActionGroup(uint8_t numOfAction, uint16_t Times);
void stopActionGroup(void);
void setActionGroupSpeed(uint8_t numOfAction, uint16_t Speed);
void setAllActionGroupSpeed(uint16_t Speed);
void getBatteryVoltage(void);
void receiveHandle(void);

#endif

