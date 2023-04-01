#ifndef __LCD9341__
#define __LCD9341__

#include "stm32f1xx_hal.h"

extern uint16_t POINT_COLOR,BACK_COLOR;

//LCD ��Ҫ������
typedef struct 
{ 
uint16_t width; //LCD ���
uint16_t height; //LCD �߶�
uint16_t id; //LCD ID
uint8_t dir; //���������������ƣ�0��������1��������
uint16_t wramcmd; //��ʼд gram ָ�0x2C
uint16_t setxcmd; //���� x ����ָ�0x2A
uint16_t setycmd; //���� y ����ָ�0x2B
}_lcddev; 

//LCD�˿ڶ���
#define LCD_LED_ON  HAL_GPIO_WritePin(LCD_BL_GPIO_Port,LCD_BL_Pin,GPIO_PIN_SET)      //LCD����      PC10
 
#define LCD_CS_SET 	HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin,GPIO_PIN_SET)      //Ƭѡ�˿�      PC9
#define LCD_RS_SET  HAL_GPIO_WritePin(LCD_RS_GPIO_Port,LCD_RS_Pin,GPIO_PIN_SET)      //����/����     PC8
#define LCD_WR_SET  HAL_GPIO_WritePin(LCD_WR_GPIO_Port,LCD_WR_Pin,GPIO_PIN_SET)    	 //д����        PC7
#define LCD_RD_SET  HAL_GPIO_WritePin(LCD_RD_GPIO_Port,LCD_RD_Pin,GPIO_PIN_SET)   	 //������        PC6

#define LCD_CS_CLR  HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin,GPIO_PIN_RESET)    //Ƭѡ�˿�      PC9
#define LCD_RS_CLR  HAL_GPIO_WritePin(LCD_RS_GPIO_Port,LCD_RS_Pin,GPIO_PIN_RESET)    //����/����     PC8
#define LCD_WR_CLR  HAL_GPIO_WritePin(LCD_WR_GPIO_Port,LCD_WR_Pin,GPIO_PIN_RESET)    //д����        PC7
#define LCD_RD_CLR  HAL_GPIO_WritePin(LCD_RD_GPIO_Port,LCD_RD_Pin,GPIO_PIN_RESET)     //������        PC6

//PB0~15 Ϊ������
#define DATAOUT(x) 	GPIOB->ODR=x;    //�������
#define DATAIN     	GPIOB->IDR;   //��������

//ɨ�跽����
#define L2R_U2D  	0 //������,���ϵ���
#define L2R_D2U  	1 //������,���µ���
#define R2L_U2D  	2 //���ҵ���,���ϵ���
#define R2L_D2U  	3 //���ҵ���,���µ���
#define U2D_L2R  	4 //���ϵ���,������
#define U2D_R2L  	5 //���ϵ���,���ҵ���
#define D2U_L2R  	6 //���µ���,������
#define D2U_R2L  	7 //���µ���,���ҵ���
#define DFT_SCAN_DIR  L2R_U2D  //Ĭ�Ϸ���

//������ɫ
#define WHITE       0xFFFF
#define BLACK       0x0000
#define BLUE        0x001F
#define BRED        0XF81F
#define GRED        0XFFE0
#define GBLUE       0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN       0XBC40
#define BRRED       0XFC07
#define GRAY        0X8430

//д���ݺ���
#define LCD_WR_DATA(data){\
        LCD_RS_SET;\
        LCD_CS_CLR;\
        DATAOUT(data);\
        LCD_WR_CLR;\
        LCD_WR_SET;\
        LCD_CS_SET;\
    } 

void LCD_Init(void);
void LCD_WR_REG(uint16_t data);
void LCD_WR_DATAX(uint16_t data);
uint16_t LCD_RD_DATA(void);
uint16_t LCD_ReadReg(uint16_t LCD_Reg);
void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(uint16_t RGB_Code);
void LCD_DisplayOn(void);
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);
void LCD_Scan_Dir(uint8_t dir);
void LCD_DrawPoint(uint16_t x,uint16_t y);
void LCD_Fast_DrawPoint(uint16_t x,uint16_t y,uint16_t color);
void LCD_Display_Dir(uint8_t dir);
void LCD_Set_Window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height);
void LCD_Clear(uint16_t color);
void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color);
void LCD_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode);
void LCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size);//����ʾ��λ0
void LCD_ShowxNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode);
void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p);
void Show_Str(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t*str,uint8_t size,uint8_t mode);

	
#endif
