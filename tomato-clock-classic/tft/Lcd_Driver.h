/**
 * @file Lcd_Driver.h
 * @author UnnamedOrange
 * @brief 在原有的 LCD Driver 上进行了简单修改。
 * @version 0.1
 * @date 2022-04-04
 *
 * @copyright 版权归原作者所有。
 *
 */

#pragma once

#include "utils.hpp"

#define RED 0xf800
#define GREEN 0x07e0
#define BLUE 0x001f
#define WHITE 0xffff
#define BLACK 0x0000
#define YELLOW 0xFFE0
#define GRAY0 0xEF7D //灰色0 3165 00110 001011 00101
#define GRAY1 0x8410 //灰色1     00000 000000 00000
#define GRAY2 0x4208 //灰色2 1111111111011111

//#define LCD_CTRL   	  	GPIOB		//定义TFT数据端口
//#define LCD_LED        	GPIO_PIN_9  //MCU_PB9--->>TFT --BL
//#define LCD_RS         	GPIO_PIN_10	//PB11--->>TFT --RS/DC
//#define LCD_CS        	GPIO_PIN_11  //MCU_PB11--->>TFT --CS/CE
//#define LCD_RST     		GPIO_PIN_12	//PB10--->>TFT --RST
//#define LCD_SCL        	GPIO_PIN_13	//PB13--->>TFT --SCL/SCK
//#define LCD_SDA        	GPIO_PIN_15	//PB15 MOSI--->>TFT --SDA/DIN

//#define LCD_CTRL   	  	GPIOB		//定义TFT数据端口
//#define LCD_LED        	D5  //MCU_PB9--->>TFT --BL
//#define LCD_RS         	D6	//PB11--->>TFT --RS/DC
//#define LCD_CS        	D10  //MCU_PB11--->>TFT --CS/CE
//#define LCD_RST     		D9	//PB10--->>TFT --RST
//#define LCD_SCL        	D13	//PB13--->>TFT --SCL/SCK
//#define LCD_SDA        	D11	//PB15 MOSI--->>TFT --SDA/DIN

//#define LCD_CS_SET(x) LCD_CTRL->ODR=(LCD_CTRL->ODR&~LCD_CS)|(x ? LCD_CS:0)

//液晶控制口置1操作语句宏定义
//#define	LCD_CS_SET  	LCD_CTRL->BSRR=LCD_CS
//#define	LCD_RS_SET  	LCD_CTRL->BSRR=LCD_RS
//#define	LCD_SDA_SET  	LCD_CTRL->BSRR=LCD_SDA
//#define	LCD_SCL_SET  	LCD_CTRL->BSRR=LCD_SCL
//#define	LCD_RST_SET  	LCD_CTRL->BSRR=LCD_RST
//#define	LCD_LED_SET  	LCD_CTRL->BSRR=LCD_LED

//液晶控制口置0操作语句宏定义
//#define	LCD_CS_CLR  	LCD_CTRL->BRR=LCD_CS
//#define	LCD_RS_CLR  	LCD_CTRL->BRR=LCD_RS
//#define	LCD_SDA_CLR  	LCD_CTRL->BRR=LCD_SDA
//#define	LCD_SCL_CLR  	LCD_CTRL->BRR=LCD_SCL
//#define	LCD_RST_CLR  	LCD_CTRL->BRR=LCD_RST
//#define	LCD_LED_CLR  	LCD_CTRL->BRR=LCD_LED

//液晶控制口置1操作语句宏定义
#define LCD_CS_SET LCD_CS = 1
#define LCD_RS_SET LCD_RS = 1
#define LCD_SDA_SET LCD_SDA = 1
#define LCD_SCL_SET LCD_SCL = 1
#define LCD_RST_SET LCD_RST = 1
#define LCD_LED_SET LCD_LED = 1

//液晶控制口置0操作语句宏定义
#define LCD_CS_CLR LCD_CS = 0
#define LCD_RS_CLR LCD_RS = 0
#define LCD_SDA_CLR LCD_SDA = 0
#define LCD_SCL_CLR LCD_SCL = 0
#define LCD_RST_CLR LCD_RST = 0
#define LCD_LED_CLR LCD_LED = 0

#define LCD_DATAOUT(x) LCD_DATA->ODR = x; //数据输出
#define LCD_DATAIN LCD_DATA->IDR;         //数据输入

#define LCD_WR_DATA(data)                                                      \
    {                                                                          \
        LCD_RS_SET;                                                            \
        LCD_CS_CLR;                                                            \
        LCD_DATAOUT(data);                                                     \
        LCD_WR_CLR;                                                            \
        LCD_WR_SET;                                                            \
        LCD_CS_SET;                                                            \
    }

void LCD_GPIO_Init(void);
void Lcd_WriteIndex(u8 Index);
void Lcd_WriteData(u8 Data);
void Lcd_WriteReg(u8 Index, u8 Data);
u16 Lcd_ReadReg(u8 LCD_Reg);
void Lcd_Reset(void);
void Lcd_Init(u8 dir);
void Lcd_Clear(u16 Color);
void Lcd_ClearHalf(u16 Color);
void Lcd_SetXY(u16 x, u16 y);
void Lcd_SetRegion(u16 x_start, u16 y_start, u16 x_end, u16 y_end);
void Gui_DrawPoint(u16 x, u16 y, u16 Data);
unsigned int Lcd_ReadPoint(u16 x, u16 y);
void Lcd_On();
void Lcd_Off();
