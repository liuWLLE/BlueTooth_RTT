#ifndef __LCDSPI169_H
#define __LCDSPI169_H

#include "sys.h"

//LCD-SPI引脚配置（电平控制频繁，使用寄存器效率高）
#define LCD_SPI                     SPI1                                            // SPI1
#define LCD_SPI_REG_DR              LCD_SPI->DR                                     // SPI数据寄存器
#define LCD_SPI_REG_CR              LCD_SPI->CR1                                    // SPI控制寄存器
#define LCD_SPI_CLOCK               RCC_APB2Periph_SPI1                             // SPI1外设时钟
#define LCD_SPI_CLOCK_RATE          SPI_BaudRatePrescaler_4                         // SPI1时钟分频系数（72/x MHZ）

#define LCD_SPI_DMA_CLOCK           RCC_AHBPeriph_DMA1                              // DMA时钟
#define LCD_SPI_DMA_CHANNEL         DMA1_Channel3                                   // DMA发送通道


#define GPIO_LCD_SCK_CLK            RCC_APB2Periph_GPIOB                            // SCK引脚时钟
#define LCD_SCK_PIN                 GPIO_Pin_3                                      // SCK引脚，需要重定义SPI1的IO口复用
#define LCD_SCK_PORT                GPIOB                                           // SCK引脚端口  

#define GPIO_LCD_SDA_CLK            RCC_APB2Periph_GPIOB                            // SDA引脚时钟
#define LCD_SDA_PIN                 GPIO_Pin_5                                      // SDA引脚，需要重定义SPI1的IO口复用
#define LCD_SDA_PORT                GPIOB                                           // SDA引脚端口  

#define GPIO_LCD_CS_CLK             RCC_APB2Periph_GPIOB                            // CS引脚时钟
#define LCD_CS_PIN                  GPIO_Pin_4                                      // CS片选引脚，低电平有效
#define LCD_CS_PORT                 GPIOB                                           // CS引脚端口 
#define LCD_CS_H                    LCD_CS_PORT->BSRR = LCD_CS_PIN                  // 片选拉高
#define LCD_CS_L                    LCD_CS_PORT->BRR = LCD_CS_PIN                   // 片选拉低

#define GPIO_LCD_DC_CLK             RCC_APB2Periph_GPIOB                            // DC引脚时钟
#define LCD_DC_PIN                  GPIO_Pin_6                                      // DC引脚，数据指令选择  
#define LCD_DC_PORT                 GPIOB                                           // DC引脚端口 
#define LCD_DC_COMMAND              LCD_DC_PORT->BRR = LCD_DC_PIN                   // 低电平，指令传输 
#define LCD_DC_DATA                 LCD_DC_PORT->BSRR = LCD_DC_PIN                  // 高电平，数据传输

#define GPIO_LCD_BACKLIGHT_CLK      RCC_APB2Periph_GPIOB                            // 背光引脚时钟
#define LCD_BACKLIGHT_PIN           GPIO_Pin_7                                      // 背光引脚
#define LCD_BACKLIGHT_PORT          GPIOB                                           // 背光引脚端口
#define LCD_BACKLIGHT_ON            LCD_BACKLIGHT_PORT->BSRR = LCD_BACKLIGHT_PIN    // 高电平，开启背光
#define LCD_BACKLIGHT_OFF           LCD_BACKLIGHT_PORT->BRR = LCD_BACKLIGHT_PIN     // 低电平，关闭背光

//LCD屏幕硬件参数宏
#define LCD_Width                   240         //像素长度
#define LCD_Height                  280         //像素宽度
#define LCD_DMA_BUF_SIZE            256         //DMA缓存大小

//显示方向参数
#define Direction_H                 0           //横屏显示
#define Direction_H_Flip            1           //横屏显示,上下翻转
#define Direction_V                 2           //竖屏显示 
#define Direction_V_Flip            3           //竖屏显示,上下翻转 

//显示时多余位补0还是补空格
#define Fill_Zero                   0           //填充0
#define Fill_Space                  1           //填充空格

//常用颜色
/*
    1. 定义的是24位 RGB888颜色，然后再通过代码自动转换成 16位 RGB565 的颜色
    2. 24位的颜色中，从高位到低位分别对应 R、G、B  3个颜色通道
    3. 用调色板获取24位RGB颜色，再将颜色输入LCD_SetColor()或LCD_SetBackColor()就可以显示出相应的颜色 
*/
#define LCD_WHITE                   0xFFFFFF    //纯白色
#define LCD_BLACK                   0x000000    //纯黑色

#define LCD_BLUE                    0x0000FF    //纯蓝色
#define LCD_GREEN                   0x00FF00    //纯绿色
#define LCD_RED                     0xFF0000    //纯红色
#define LCD_CYAN                    0x00FFFF    //蓝绿色
#define LCD_MAGENTA                 0xFF00FF    //紫红色
#define LCD_YELLOW                  0xFFFF00    //黄色
#define LCD_GREY                    0x2C2C2C    //灰色

#define LIGHT_BLUE                  0x8080FF    //亮蓝色
#define LIGHT_GREEN                 0x80FF80    //亮绿色
#define LIGHT_RED                   0xFF8080    //亮红色
#define LIGHT_CYAN                  0x80FFFF    //亮蓝绿色
#define LIGHT_MAGENTA               0xFF80FF    //亮紫红色
#define LIGHT_YELLOW                0xFFFF80    //亮黄色
#define LIGHT_GREY                  0xA3A3A3    //亮灰色

#define DARK_BLUE                   0x000080    //暗蓝色
#define DARK_GREEN                  0x008000    //暗绿色
#define DARK_RED                    0x800000    //暗红色
#define DARK_CYAN                   0x008080    //暗蓝绿色
#define DARK_MAGENTA                0x800080    //暗紫红色
#define DARK_YELLOW                 0x808000    //暗黄色
#define DARK_GREY                   0x404040    //暗灰色


typedef struct _pFont           // 字体相关结构定义
{       
    const uint8_t *pTable;      // 字模数组地址
    uint16_t Width;             // 单个字符的字模宽度
    uint16_t Height;            // 单个字符的字模长度
    uint16_t Sizes;             // 单个字符的字模数据个数
    uint16_t Table_Rows;        // 该参数只有汉字字模用到，表示二维数组的行大小
}pFONT;

extern pFONT ASCII_Font32;      // 3216 字体
extern pFONT ASCII_Font24;      // 2412 字体
extern pFONT ASCII_Font20;      // 2010 字体
extern pFONT ASCII_Font16;      // 1608 字体
extern pFONT ASCII_Font12;      // 1206 字体

//LCD基础设置
void LCD_Init(void);                                                                    //液晶屏以及SPI初始化   
void LCD_Clear(void);                                                                   //清屏函数
void LCD_ClearRect(uint16_t x,uint16_t y,uint16_t width,uint16_t height);               //局部清屏函数
        
void LCD_SetAddress(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);                   //设置坐标
void LCD_SetColor(uint32_t Color);                                                      //设置画笔颜色
void LCD_SetBackColor(uint32_t Color);                                                  //设置背景颜色
void LCD_SetDirection(uint8_t direction);                                               //设置显示方向
        
//显示ASCII字符     
void LCD_SetAsciiFont(pFONT *fonts);                                                    //设置ASCII字体
void LCD_DisplayChar(uint16_t x,uint16_t y,uint8_t c);                                  //显示单个ASCII字符
void LCD_DisplayString( uint16_t x,uint16_t y,char *p);                                 //显示ASCII字符串
        
//显示中文字符，包括ASCII码       
void LCD_SetTextFont(pFONT *fonts);                                                     //设置文本字体，包括中文和ASCII字体
void LCD_DisplayChinese(uint16_t x,uint16_t y,char *pText);                             //显示单个汉字
void LCD_DisplayText(uint16_t x,uint16_t y,char *pText);                                //显示字符串，包括中文和ASCII字符

//显示整数或小数
void LCD_ShowNumMode(uint8_t mode);                                                     //设置变量显示模式，多余位填充空格还是填充0
void LCD_DisplayNumber(uint16_t x,uint16_t y,int32_t number,uint8_t len) ;              //显示整数
void LCD_DisplayDecimals(uint16_t x,uint16_t y,double number,uint8_t len,uint8_t decs); //显示小数

//2D图形函数
void LCD_DrawPoint(uint16_t x,uint16_t y,uint32_t color);                               //画点

void LCD_DrawLine_V(uint16_t x,uint16_t y,uint16_t height);                             //画垂直线
void LCD_DrawLine_H(uint16_t x,uint16_t y,uint16_t width);                              //画水平线
void LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2, uint16_t y2);                    //两点之间画线

void LCD_DrawRect(uint16_t x,uint16_t y,uint16_t width,uint16_t height);                //画矩形
void LCD_DrawCircle(uint16_t x,uint16_t y,uint16_t r);                                  //画圆
void LCD_DrawEllipse(int x,int y,int r1,int r2);                                        //画椭圆

//区域填充函数        
void LCD_FillRect(uint16_t x,uint16_t y,uint16_t width,uint16_t height);                //填充矩形
void LCD_FillCircle(uint16_t x,uint16_t y,uint16_t r);                                  //填充圆
void LCD_FillColor(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t *color);    //区域填充颜色 

//绘制单色图片
void LCD_DrawImage(uint16_t x,uint16_t y,uint16_t width,uint16_t height,const uint8_t *pImage);

#endif  
