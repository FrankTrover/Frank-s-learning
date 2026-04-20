#ifndef __LCD_H
#define __LCD_H 

#include "main.h"
#include "stdlib.h"
#include <stdint.h>


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

/*
 * LCD 引脚映射统一引用 main.h / LCD pin mapping is taken from main.h.
 * 移植时优先修改 main.h，再核对这里的映射是否一致。
 * When porting, update main.h first, then verify this mapping.
 */
#define LCD_SCL_GPIO_PORT                SCL_GPIO_Port
#define LCD_SCL_GPIO_PIN                 SCL_Pin

#define LCD_SDA_GPIO_PORT                SDA_GPIO_Port
#define LCD_SDA_GPIO_PIN                 SDA_Pin

#define LCD_RST_GPIO_PORT                RES_GPIO_Port
#define LCD_RST_GPIO_PIN                 RES_Pin

#define LCD_DC_GPIO_PORT                 DC_GPIO_Port
#define LCD_DC_GPIO_PIN                  DC_Pin

#define LCD_CS_GPIO_PORT                 CS_GPIO_Port
#define LCD_CS_GPIO_PIN                  CS_Pin

#define LCD_BLK_GPIO_PORT                BLK_GPIO_Port
#define LCD_BLK_GPIO_PIN                 BLK_Pin

#define LCD_SCLK_Clr() HAL_GPIO_WritePin(LCD_SCL_GPIO_PORT, LCD_SCL_GPIO_PIN, GPIO_PIN_RESET)
#define LCD_SCLK_Set() HAL_GPIO_WritePin(LCD_SCL_GPIO_PORT, LCD_SCL_GPIO_PIN, GPIO_PIN_SET)

#define LCD_MOSI_Clr() HAL_GPIO_WritePin(LCD_SDA_GPIO_PORT, LCD_SDA_GPIO_PIN, GPIO_PIN_RESET)
#define LCD_MOSI_Set() HAL_GPIO_WritePin(LCD_SDA_GPIO_PORT, LCD_SDA_GPIO_PIN, GPIO_PIN_SET)

#define LCD_RES_Clr()  HAL_GPIO_WritePin(LCD_RST_GPIO_PORT, LCD_RST_GPIO_PIN, GPIO_PIN_RESET)
#define LCD_RES_Set()  HAL_GPIO_WritePin(LCD_RST_GPIO_PORT, LCD_RST_GPIO_PIN, GPIO_PIN_SET)

#define LCD_DC_Clr()   HAL_GPIO_WritePin(LCD_DC_GPIO_PORT, LCD_DC_GPIO_PIN, GPIO_PIN_RESET)
#define LCD_DC_Set()   HAL_GPIO_WritePin(LCD_DC_GPIO_PORT, LCD_DC_GPIO_PIN, GPIO_PIN_SET)

#define LCD_CS_Clr()   HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_GPIO_PIN, GPIO_PIN_RESET)
#define LCD_CS_Set()   HAL_GPIO_WritePin(LCD_CS_GPIO_PORT, LCD_CS_GPIO_PIN, GPIO_PIN_SET)

#define LCD_BLK_Clr()  HAL_GPIO_WritePin(LCD_BLK_GPIO_PORT, LCD_BLK_GPIO_PIN, GPIO_PIN_RESET)
#define LCD_BLK_Set()  HAL_GPIO_WritePin(LCD_BLK_GPIO_PORT, LCD_BLK_GPIO_PIN, GPIO_PIN_SET)

/*
 * 背光极性 / Backlight polarity.
 * 1=高电平点亮，0=低电平点亮。
 * 1 means active-high, 0 means active-low.
 */
#define LCD_BLK_ACTIVE_HIGH 1

#if LCD_BLK_ACTIVE_HIGH
#define LCD_BLK_On() LCD_BLK_Set()
#define LCD_BLK_Off() LCD_BLK_Clr()
#else
#define LCD_BLK_On() LCD_BLK_Clr()
#define LCD_BLK_Off() LCD_BLK_Set()
#endif


/*
 * 屏幕方向 / Display orientation.
 * 0/1=竖屏，2/3=横屏。
 * 0/1 portrait, 2/3 landscape.
 */
#define USE_HORIZONTAL 1


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 128
#define LCD_H 160

#else
#define LCD_W 160
#define LCD_H 128
#endif

/* 画笔颜色 / Common RGB565 colors. */
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			       0XFFE0
#define GBLUE			       0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN                  0XBC40 // 棕色
#define BRRED                  0XFC07 // 棕红色
#define GRAY                   0X8430 // 灰色
#define DARKBLUE               0X01CF // 深蓝色
#define LIGHTBLUE              0X7D7C // 浅蓝色
#define GRAYBLUE               0X5458 // 灰蓝色
#define LIGHTGREEN             0X841F // 浅绿色
#define LGRAY                  0XC618 // 面板背景色
#define LGRAYBLUE              0XA651 // 中间层浅灰蓝
#define LBBLUE                 0X2B12 // 更浅的蓝色

/* 底层函数 / Low-level functions. 一般不建议业务层直接调用 / Usually not used directly by app code. */
void LCD_GPIO_Init(void); /* 初始化 LCD GPIO / Initialize LCD GPIOs. */
void LCD_Writ_Bus(u8 dat); /* 软件 SPI 发送 1 字节 / Send one byte on software SPI bus. */
void LCD_WR_DATA8(u8 dat); /* 写 8 位数据 / Write 8-bit data. */
void LCD_WR_DATA(u16 dat); /* 写 16 位数据 / Write 16-bit data. */
void LCD_WR_REG(u8 dat); /* 写命令 / Write command. */
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2); /* 设置地址窗口 / Set address window. */

/*
 * LCD 总初始化入口 / LCD global initialization entry.
 * 前置条件 / Preconditions:
 *   1) 已调用 delay_init(HAL_RCC_GetSysClockFreq())
 *   2) 已调用 MX_GPIO_Init()
 */
void LCD_Init(void);

void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color); /* 区域填充 / Fill rectangle area. */
void LCD_DrawPoint(u16 x,u16 y,u16 color); /* 画点 / Draw one pixel. */
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color); /* 画线 / Draw line. */
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color); /* 画矩形 / Draw rectangle. */
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color); /* 画圆 / Draw circle. */

void LCD_ShowChinese(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode); /* 显示中文 / Show Chinese string. */
void LCD_ShowChinese12x12(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode); /* 12x12 中文字模 / 12x12 Chinese font. */
void LCD_ShowChinese16x16(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode); /* 16x16 中文字模 / 16x16 Chinese font. */
void LCD_ShowChinese24x24(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode); /* 24x24 中文字模 / 24x24 Chinese font. */
void LCD_ShowChinese32x32(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode); /* 32x32 中文字模 / 32x32 Chinese font. */

void LCD_ShowChar(u16 x,u16 y,u8 num,u16 fc,u16 bc,u8 sizey,u8 mode); /* 显示单字符 / Show one ASCII char. */
/*
 * 在指定坐标显示字符串（推荐业务层直接调用）。
 * 参数:
 *   x, y  : 起始坐标
 *   p     : 字符串指针（建议 const u8*）
 *   fc    : 字体颜色，如 WHITE/RED
 *   bc    : 背景颜色
 *   sizey : 字体高度，可选 12/16/24/32
 *   mode  : 0=覆盖背景，1=透明叠加
 * 示例:
 *   LCD_ShowString(0, 0, (const u8 *)"Freq:10kHz", WHITE, BLACK, 16, 0);
 */
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 fc,u16 bc,u8 sizey,u8 mode);
u32 mypow(u8 m,u8 n); /* 幂运算 / Integer power helper. */
/*
 * 显示无符号整数，长度不足时左侧补空格。
 * 示例:
 *   LCD_ShowIntNum(0, 20, 1234, 4, YELLOW, BLACK, 16);
 */
void LCD_ShowIntNum(u16 x,u16 y,u16 num,u8 len,u16 fc,u16 bc,u8 sizey);
void LCD_ShowFloatNum1(u16 x,u16 y,float num,u8 len,u16 fc,u16 bc,u8 sizey); /* 显示 1 位小数 / Show float with 1 decimal. */

void LCD_ShowPicture(u16 x,u16 y,u16 length,u16 width,const u8 pic[]); /* 显示图片 / Show picture. */


#endif

