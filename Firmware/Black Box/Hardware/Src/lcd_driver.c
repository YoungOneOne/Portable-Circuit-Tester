#include "stm32f4xx_hal.h"
#include "lcd_driver.h"
#include "LCD_Config.h"
#include "gpio.h"


/******************************************************************************
接口定义在Lcd_Driver.h内
#define LCD_CTRL   	  	GPIOD		
#define LCD_SCL        	GPIO_Pin_0	//TFT --SCL
#define LCD_SDA        	GPIO_Pin_1	//TFT --SDA
#define LCD_RS         	GPIO_Pin_2	//TFT --RES
#define LCD_RST     	GPIO_Pin_3	//TFT --DC
#define LCD_CS        	GPIO_Pin_4  //TFT --CS
*******************************************************************************/
void LCD_GPIO_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	      
	 __HAL_RCC_GPIOD_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
      
}


/****************************************************************************
* 名    称：Lcd_WriteIndex(uint8_t Index)
* 功    能：写 ST7735 控制器寄存器地址
* 入口参数：Index   寄存器地址
* 出口参数：无
* 说    明：调用前需先选中控制器，内部函数
* 调用方法：Lcd_WriteIndex(0xB1);
****************************************************************************/
void Lcd_WriteIndex(uint8_t Index)
{
   uint8_t i=0;
   //SPI 写命令时序开始
   LCD_CS_CLR;
   LCD_RS_CLR;
   //刷屏100次  32秒   320ms/屏
   for(i=8;i>0;i--)
   {
    if(Index&0x80){	
	  LCD_SDA_SET; }//输出数据
    else 
	  LCD_SDA_CLR; 

      LCD_SCL_CLR;      
      LCD_SCL_SET;
      Index<<=1; 
   }
   LCD_CS_SET;
}

/****************************************************************************
* 名    称：void Lcd_WriteData(uint8_t Data)
* 功    能：写 ST7735 寄存器数据
* 入口参数：Data     寄存器数据
* 出口参数：无
* 说    明：向控制器指定地址写入数据，调用前需先写寄存器地址，内部函数
* 调用方法：Lcd_WriteData(0x01);
****************************************************************************/
void Lcd_WriteData(uint8_t Data)
{
   uint8_t i=0;
   LCD_CS_CLR;
   LCD_RS_SET;
    //刷屏100次  32秒   320ms/屏
   for(i=8;i>0;i--)
   {
      if(Data&0x80)	{
	  LCD_SDA_SET;} //输出数据
      else LCD_SDA_CLR;
	   
      LCD_SCL_CLR;       
      LCD_SCL_SET;
      Data<<=1; 
   }
   LCD_CS_SET; 
}

/*************************************************
函数名：Lcd_WriteReg
功能：写寄存器
入口参数：Index,Data
返回值：无
*************************************************/
void Lcd_WriteReg(uint8_t Index,uint8_t Data)
{
	Lcd_WriteIndex(Index);
   	Lcd_WriteData(Data);
}

/*************************************************
函数名：Lcd_Reset
功能：重置LCD
入口参数：无
返回值：无
*************************************************/
void Lcd_Reset(void)
{
	LCD_RST_CLR;
	HAL_Delay(100);
	LCD_RST_SET;
	HAL_Delay(50);
}

/*************************************************
函数名：Lcd_Init
功能：LCD初始化
入口参数：无
返回值：无
*************************************************/
void Lcd_Init(void)
{	
	LCD_GPIO_Init();
	Lcd_Reset();

	Lcd_WriteIndex(0x11);//Sleep exit 
	HAL_Delay(120);
	
//ST7735 Frame Rate
	Lcd_WriteIndex(0xB1); 
	Lcd_WriteData(0x01); Lcd_WriteData(0x2C); Lcd_WriteData(0x2D); 
	Lcd_WriteIndex(0xB2); 
	Lcd_WriteData(0x01); Lcd_WriteData(0x2C); Lcd_WriteData(0x2D); 
	Lcd_WriteIndex(0xB3); 
	Lcd_WriteData(0x01); Lcd_WriteData(0x2C); Lcd_WriteData(0x2D); 
	Lcd_WriteData(0x01); Lcd_WriteData(0x2C); Lcd_WriteData(0x2D); 
	
	Lcd_WriteIndex(0xB4); //Column inversion 
	Lcd_WriteData(0x07); 
	
	//ST7735 Power Sequence
	Lcd_WriteIndex(0xC0); 
	Lcd_WriteData(0xA2); Lcd_WriteData(0x02); Lcd_WriteData(0x84); 
	Lcd_WriteIndex(0xC1); Lcd_WriteData(0xC5); 
	Lcd_WriteIndex(0xC2); 
	Lcd_WriteData(0x0A); Lcd_WriteData(0x00); 
	Lcd_WriteIndex(0xC3); 
	Lcd_WriteData(0x8A); Lcd_WriteData(0x2A); 
	Lcd_WriteIndex(0xC4); 
	Lcd_WriteData(0x8A); Lcd_WriteData(0xEE); 
	
	Lcd_WriteIndex(0xC5); //VCOM 
	Lcd_WriteData(0x0E); 
	
	Lcd_WriteIndex(0x36); //MX, MY, RGB mode 
	Lcd_WriteData(0xC0); 
	
	//ST7735 Gamma Sequence
	Lcd_WriteIndex(0xe0); 
	Lcd_WriteData(0x0f); Lcd_WriteData(0x1a); 
	Lcd_WriteData(0x0f); Lcd_WriteData(0x18); 
	Lcd_WriteData(0x2f); Lcd_WriteData(0x28); 
	Lcd_WriteData(0x20); Lcd_WriteData(0x22); 
	Lcd_WriteData(0x1f); Lcd_WriteData(0x1b); 
	Lcd_WriteData(0x23); Lcd_WriteData(0x37); Lcd_WriteData(0x00); 
	
	Lcd_WriteData(0x07); 
	Lcd_WriteData(0x02); Lcd_WriteData(0x10); 
	Lcd_WriteIndex(0xe1); 
	Lcd_WriteData(0x0f); Lcd_WriteData(0x1b); 
	Lcd_WriteData(0x0f); Lcd_WriteData(0x17); 
	Lcd_WriteData(0x33); Lcd_WriteData(0x2c); 
	Lcd_WriteData(0x29); Lcd_WriteData(0x2e); 
	Lcd_WriteData(0x30); Lcd_WriteData(0x30); 
	Lcd_WriteData(0x39); Lcd_WriteData(0x3f); 
	Lcd_WriteData(0x00); Lcd_WriteData(0x07); 
	Lcd_WriteData(0x03); Lcd_WriteData(0x10);  
	
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData(0x00);Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);Lcd_WriteData(0x7f);
	Lcd_WriteIndex(0x2b);
	Lcd_WriteData(0x00);Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);Lcd_WriteData(0x9f);
	
	Lcd_WriteIndex(0xF0); //Enable test command  
	Lcd_WriteData(0x01); 
	Lcd_WriteIndex(0xF6); //Disable ram power save mode 
	Lcd_WriteData(0x00); 
	
	Lcd_WriteIndex(0x3A); //65k mode 
	Lcd_WriteData(0x05); 
	
	
	Lcd_WriteIndex(0x29);//Display on

}


/*************************************************
函数名：Lcd_SetRegion
功能：设置lcd显示区域，在此区域写点数据自动换行
入口参数：x_start起点,y_start起点,x_end,y_end终点
返回值：无
*************************************************/
void Lcd_SetRegion(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end)
{		
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_start);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_end);

	Lcd_WriteIndex(0x2b);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_start);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_end);

	Lcd_WriteIndex(0x2c);

}

/*************************************************
函数名：Lcd_SetXY
功能：设置lcd显示起始点
入口参数：xy坐标
返回值：无
*************************************************/
void Lcd_SetXY(uint16_t x,uint16_t y)
{
  	Lcd_SetRegion(x,y,x,y);
}

	
/*************************************************
函数名：Gui_DrawPoint
功能：画一个点
入口参数：x,y,Data
返回值：无
*************************************************/
void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data)
{
	Lcd_SetRegion(x,y,x+1,y+1);
	Lcd_WriteData(Data>>8);
	Lcd_WriteData(Data);

}    

/*****************************************
函数名：Lcd_ReadPoint
函数功能：读TFT某一点的颜色                          
出口参数：Data  点颜色值                                 
******************************************/
unsigned int Lcd_ReadPoint(uint16_t x,uint16_t y)
{
  unsigned int Data;
  Lcd_SetXY(x,y);

  //Lcd_ReadData();//丢掉无用字节
  //Data=Lcd_ReadData();
  Lcd_WriteData(Data);
  return Data;
}
/*************************************************
函数名：Lcd_Clear
功能：全屏清屏函数
入口参数：填充颜色Color
返回值：无
*************************************************/
void Lcd_Clear(uint16_t Color)
{
    uint16_t x, y;

    Lcd_WriteIndex(0x2A);
    Lcd_WriteData(0x00); Lcd_WriteData(0x02);   // 起始列
    Lcd_WriteData(0x00); Lcd_WriteData(0x81);   // 结束列 (2 + 127)

    Lcd_WriteIndex(0x2B);
    Lcd_WriteData(0x00); Lcd_WriteData(0x01);   // 起始行
    Lcd_WriteData(0x00); Lcd_WriteData(0xA0);   // 结束行 (1 + 159)

    Lcd_WriteIndex(0x2C);

    for (y = 0; y < 160; y++)
    {
        for (x = 0; x < 128; x++)
        {
            Lcd_WriteData(Color >> 8);
            Lcd_WriteData(Color & 0xFF);
        }
    }
}

