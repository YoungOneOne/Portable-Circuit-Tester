#include "stm32f4xx_hal.h"
#include "stm32f407xx.h"


//LCD重要参数集
typedef struct  
{										    
	uint16_t width;			//LCD 宽度
	uint16_t height;			//LCD 高度
	uint16_t id;				//LCD ID
	uint8_t  dir;			//横屏还是竖屏控制：0，竖屏；1，横屏。	
	uint16_t	 wramcmd;		//开始写gram指令
	uint16_t  setxcmd;		//设置x坐标指令
	uint16_t  setycmd;		//设置y坐标指令	 
}_lcd_dev; 	
//LCD参数
extern _lcd_dev lcddev;	//管理LCD重要参数


#define RED  	0xf800
#define GREEN	0x07e0
#define BLUE 	0x001f
#define WHITE	0xffff
#define BLACK	0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D   	//灰色0 3165 00110 001011 00101
#define GRAY1   0x8410      	//灰色1      00000 000000 00000
#define GRAY2   0x4208      	//灰色2  1111111111011111

/////////////////////////////////////用户配置区///////////////////////////////////	 
//支持横竖屏快速定义切换，支持8/16位模式切换
#define USE_HORIZONTAL  	0	    //定义是否使用横屏 		0,不使用.  1,使用.
//使用模拟SPI作为测试


#define LCD_CTRL   	  	GPIOD		//定义TFT数据端口    TFT用的都是口都是D

#define LCD_SCL        	GPIO_PIN_0	//TFT --SCL
#define LCD_SDA        	GPIO_PIN_1	//TFT --SDA
#define LCD_RS         	GPIO_PIN_2	//TFT --DC
#define LCD_RST     	GPIO_PIN_3	//TFT --RES
#define LCD_CS        	GPIO_PIN_4  //TFT --CS

//SET操作语句宏定义
#define	LCD_CS_SET  	LCD_CTRL->BSRR=LCD_CS    
#define	LCD_RS_SET  	LCD_CTRL->BSRR=LCD_RS    
#define	LCD_SDA_SET  	LCD_CTRL->BSRR=LCD_SDA    
#define	LCD_SCL_SET  	LCD_CTRL->BSRR=LCD_SCL    
#define	LCD_RST_SET  	LCD_CTRL->BSRR=LCD_RST    
#define	LCD_LED_SET  	LCD_CTRL->BSRR=LCD_LED   

//RESET操作语句宏定义
#define	LCD_CS_CLR  	LCD_CTRL->BSRR=(uint32_t)LCD_CS << 16U    
#define	LCD_RS_CLR  	LCD_CTRL->BSRR=(uint32_t)LCD_RS << 16U    
#define	LCD_SDA_CLR  	LCD_CTRL->BSRR=(uint32_t)LCD_SDA << 16U    
#define	LCD_SCL_CLR  	LCD_CTRL->BSRR=(uint32_t)LCD_SCL << 16U    
#define	LCD_RST_CLR  	LCD_CTRL->BSRR=(uint32_t)LCD_RST << 16U 
#define	LCD_LED_CLR  	LCD_CTRL->BSRR=(uint32_t)LCD_LED << 16U 


#define LCD_DATAOUT(x) LCD_DATA->ODR=x; //数据输出
#define LCD_DATAIN     LCD_DATA->IDR;   //数据输入

#define LCD_WR_DATA(data){\
LCD_RS_SET;\
LCD_CS_CLR;\
LCD_DATAOUT(data);\
LCD_WR_CLR;\
LCD_WR_SET;\
LCD_CS_SET;\
} 



void LCD_GPIO_Init(void);
void Lcd_WriteIndex(uint8_t Index);
void Lcd_WriteData(uint8_t Data);
void Lcd_WriteReg(uint8_t Index,uint8_t Data);
uint16_t Lcd_ReadReg(uint8_t LCD_Reg);
void Lcd_Reset(void);
void Lcd_Init(void);
void Lcd_Clear(uint16_t Color);
void Lcd_SetXY(uint16_t x,uint16_t y);
void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data);
unsigned int Lcd_ReadPoint(uint16_t x,uint16_t y);

