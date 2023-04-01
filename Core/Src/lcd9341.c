#include "lcd9341.h"
#include "mygpio.h"
#include "tim.h"
#include "font.h"
#include "string.h"
#include "flash.h" 

uint16_t POINT_COLOR=0x0000;     
uint16_t BACK_COLOR=0xFFFF;      

_lcddev lcddev;

//写寄存器函数
//data:寄存器值
void LCD_WR_REG(uint16_t data)
{ 
    LCD_RS_CLR;//写地址
    LCD_CS_CLR;
    DATAOUT(data);
    LCD_WR_CLR;
    LCD_WR_SET;
    LCD_CS_SET;
}

//写数据函数
//可以替代LCD_WR_DATAX宏,拿时间换空间.
//data:寄存器值
void LCD_WR_DATAX(uint16_t data)
{
    LCD_RS_SET;
    LCD_CS_CLR;
    DATAOUT(data);
    LCD_WR_CLR;
    LCD_WR_SET;
    LCD_CS_SET;
}

//读LCD数据
//返回值:读到的值
uint16_t LCD_RD_DATA(void)
{
    uint16_t t;
    GPIOB->CRL=0X88888888; //PB0-7  上拉输入
    GPIOB->CRH=0X88888888; //PB8-15 上拉输入
    GPIOB->ODR=0X0000;     //全部输出0

    LCD_RS_SET;
    LCD_CS_CLR;
    LCD_RD_CLR;
    
    tim6_delay_us(9);
    t=DATAIN;
    
    LCD_RD_SET;
    LCD_CS_SET; 

    GPIOB->CRL=0X33333333;  //PB0-7  上拉输出
    GPIOB->CRH=0X33333333;  //PB8-15 上拉输出
    GPIOB->ODR=0XFFFF;      //全部输出高
    return t;
}

//写寄存器
//LCD_Reg:寄存器编号
//LCD_RegValue:要写入的值
void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue)
{
    LCD_WR_REG(LCD_Reg);  
    LCD_WR_DATA(LCD_RegValue);
}

//读寄存器
//LCD_Reg:寄存器编号
//返回值:读到的值
uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
    LCD_WR_REG(LCD_Reg);        //写入要读的寄存器号
    return LCD_RD_DATA();
}

//开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
    LCD_WR_REG(lcddev.wramcmd);
}

//LCD写GRAM
//RGB_Code:颜色值
void LCD_WriteRAM(uint16_t RGB_Code)
{
    LCD_WR_DATA(RGB_Code);      //写十六位GRAM
}

//LCD开启显示
void LCD_DisplayOn(void)
{
    LCD_WR_REG(0X29);       //开启显示
}

//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(Xpos >> 8);
    LCD_WR_DATA(Xpos & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(Ypos >> 8);
    LCD_WR_DATA(Ypos & 0XFF);
}

//设置LCD的自动扫描方向
//dir:0~7,代表8个方向
//一般设置为L2R_U2D即可
void LCD_Scan_Dir(uint8_t dir)
{
    uint16_t regval = 0;
    uint16_t dirreg = 0;
    uint16_t temp;

    //横屏时需要改变扫描方向
    if (lcddev.dir == 1)
    {
        switch (dir)   //方向转换
        {
            case 0:
                dir = 6;
                break;
            case 1:
                dir = 7;
                break;
            case 2:
                dir = 4;
                break;
            case 3:
                dir = 5;
                break;
            case 4:
                dir = 1;
                break;
            case 5:
                dir = 0;
                break;
            case 6:
                dir = 3;
                break;
            case 7:
                dir = 2;
                break;
        }
    }

    switch (dir)
    {
        case L2R_U2D://从左到右,从上到下
            regval |= (0 << 7) | (0 << 6) | (0 << 5);
            break;
        case L2R_D2U://从左到右,从下到上
            regval |= (1 << 7) | (0 << 6) | (0 << 5);
            break;
        case R2L_U2D://从右到左,从上到下
            regval |= (0 << 7) | (1 << 6) | (0 << 5);
            break;
        case R2L_D2U://从右到左,从下到上
            regval |= (1 << 7) | (1 << 6) | (0 << 5);
            break;
        case U2D_L2R://从上到下,从左到右
            regval |= (0 << 7) | (0 << 6) | (1 << 5);
            break;
        case U2D_R2L://从上到下,从右到左
            regval |= (0 << 7) | (1 << 6) | (1 << 5);
            break;
        case D2U_L2R://从下到上,从左到右
            regval |= (1 << 7) | (0 << 6) | (1 << 5);
            break;
        case D2U_R2L://从下到上,从右到左
            regval |= (1 << 7) | (1 << 6) | (1 << 5);
            break;
    }

	dirreg = 0X36;
    regval |= 0X08;								//9341要设置BGR位
    LCD_WriteReg(dirreg, regval);
        if (regval & 0X20)
        {
            if (lcddev.width < lcddev.height)   //交换X,Y
            {
                temp = lcddev.width;
                lcddev.width = lcddev.height;
                lcddev.height = temp;
            }
        }
        else
        {
            if (lcddev.width > lcddev.height)   //交换X,Y
            {
                temp = lcddev.width;
                lcddev.width = lcddev.height;
                lcddev.height = temp;
            }
        }
    

    //设置显示区域(开窗)大小
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(0);
        LCD_WR_DATA(0);
        LCD_WR_DATA((lcddev.width - 1) >> 8);
        LCD_WR_DATA((lcddev.width - 1) & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(0);
        LCD_WR_DATA(0);
        LCD_WR_DATA((lcddev.height - 1) >> 8);
        LCD_WR_DATA((lcddev.height - 1) & 0XFF);
    
}

//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(uint16_t x,uint16_t y)
{
    LCD_SetCursor(x, y);        //设置光标位置
    LCD_WriteRAM_Prepare();     //开始写入GRAM
    LCD_WR_DATA(POINT_COLOR);
}

//快速画点
//x,y:坐标
//color:颜色
void LCD_Fast_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
    
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(x >> 8);
    LCD_WR_DATA(x & 0XFF);				//设置光标
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(y >> 8);
    LCD_WR_DATA(y & 0XFF);
    
    LCD_RS_CLR;
    LCD_CS_CLR;
    DATAOUT(lcddev.wramcmd);    //写指令
    LCD_WR_CLR;
    LCD_WR_SET;
    LCD_CS_SET;
    LCD_WR_DATA(color);         //写数据
}

//设置LCD显示方向
//dir:0,竖屏；1,横屏
void LCD_Display_Dir(uint8_t dir)
{
    lcddev.dir = dir;       //竖屏/横屏

    if (dir == 0)           //竖屏
    {
        lcddev.width = 240;
        lcddev.height = 320;
        lcddev.wramcmd = 0X2C;
        lcddev.setxcmd = 0X2A;
        lcddev.setycmd = 0X2B;
    }
    else     //横屏
    {
        lcddev.width = 320;
        lcddev.height = 240;
        lcddev.wramcmd = 0X2C;
        lcddev.setxcmd = 0X2A;
        lcddev.setycmd = 0X2B;
    }

    LCD_Scan_Dir(DFT_SCAN_DIR);     //默认扫描方向
}


//设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height. 
void LCD_Set_Window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height)
{
    uint16_t twidth, theight;
    twidth = sx + width - 1;
    theight = sy + height - 1;
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(sx >> 8);
    LCD_WR_DATA(sx & 0XFF);
    LCD_WR_DATA(twidth >> 8);
    LCD_WR_DATA(twidth & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(sy >> 8);
    LCD_WR_DATA(sy & 0XFF);
    LCD_WR_DATA(theight >> 8);
    LCD_WR_DATA(theight & 0XFF);
}


//初始化lcd
//该初始化函数可以初始化各种ALIENTEK出品的LCD液晶屏
//本函数占用较大flash,用户可以根据自己的实际情况,删掉未用到的LCD初始化代码.以节省空间.
void LCD_Init(void)
{ 
	HAL_Delay(50); // delay 50 ms 
    LCD_WriteReg(0x0000,0x0001);
    HAL_Delay(50); // delay 50 ms 

    //尝试9341 ID的读取
    LCD_WR_REG(0XD3);
    lcddev.id = LCD_RD_DATA();          //dummy read
    lcddev.id = LCD_RD_DATA();          //读到0X00
    lcddev.id = LCD_RD_DATA();          //读取0X93
    lcddev.id <<= 8;
    lcddev.id |= LCD_RD_DATA();         //读取0X41

	//9341初始化代码，厂家提供
    LCD_WR_REG(0xCF);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0xC1);
    LCD_WR_DATAX(0X30);
    LCD_WR_REG(0xED);
    LCD_WR_DATAX(0x64);
    LCD_WR_DATAX(0x03);
    LCD_WR_DATAX(0X12);
    LCD_WR_DATAX(0X81);
    LCD_WR_REG(0xE8);
    LCD_WR_DATAX(0x85);
    LCD_WR_DATAX(0x10);
    LCD_WR_DATAX(0x7A);
    LCD_WR_REG(0xCB);
    LCD_WR_DATAX(0x39);
    LCD_WR_DATAX(0x2C);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0x34);
    LCD_WR_DATAX(0x02);
    LCD_WR_REG(0xF7);
    LCD_WR_DATAX(0x20);
    LCD_WR_REG(0xEA);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0x00);
    LCD_WR_REG(0xC0);       //Power control
    LCD_WR_DATAX(0x1B);     //VRH[5:0]
    LCD_WR_REG(0xC1);       //Power control
    LCD_WR_DATAX(0x01);     //SAP[2:0];BT[3:0]
    LCD_WR_REG(0xC5);       //VCM control
    LCD_WR_DATAX(0x30);     //3F
    LCD_WR_DATAX(0x30);     //3C
    LCD_WR_REG(0xC7);       //VCM control2
    LCD_WR_DATAX(0XB7);
    LCD_WR_REG(0x36);       // Memory Access Control
    LCD_WR_DATAX(0x48);
    LCD_WR_REG(0x3A);
    LCD_WR_DATAX(0x55);
    LCD_WR_REG(0xB1);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0x1A);
    LCD_WR_REG(0xB6);       // Display Function Control
    LCD_WR_DATAX(0x0A);
    LCD_WR_DATAX(0xA2);
    LCD_WR_REG(0xF2);       // 3Gamma Function Disable
    LCD_WR_DATAX(0x00);
    LCD_WR_REG(0x26);       //Gamma curve selected
    LCD_WR_DATAX(0x01);
    LCD_WR_REG(0xE0);       //Set Gamma
    LCD_WR_DATAX(0x0F);
    LCD_WR_DATAX(0x2A);
    LCD_WR_DATAX(0x28);
    LCD_WR_DATAX(0x08);
    LCD_WR_DATAX(0x0E);
    LCD_WR_DATAX(0x08);
    LCD_WR_DATAX(0x54);
    LCD_WR_DATAX(0XA9);
    LCD_WR_DATAX(0x43);
    LCD_WR_DATAX(0x0A);
    LCD_WR_DATAX(0x0F);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0x00);
    LCD_WR_REG(0XE1);       //Set Gamma
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0x15);
    LCD_WR_DATAX(0x17);
    LCD_WR_DATAX(0x07);
    LCD_WR_DATAX(0x11);
    LCD_WR_DATAX(0x06);
    LCD_WR_DATAX(0x2B);
    LCD_WR_DATAX(0x56);
    LCD_WR_DATAX(0x3C);
    LCD_WR_DATAX(0x05);
    LCD_WR_DATAX(0x10);
    LCD_WR_DATAX(0x0F);
    LCD_WR_DATAX(0x3F);
    LCD_WR_DATAX(0x3F);
    LCD_WR_DATAX(0x0F);
    LCD_WR_REG(0x2B);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0x01);
    LCD_WR_DATAX(0x3f);
    LCD_WR_REG(0x2A);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0x00);
    LCD_WR_DATAX(0xef);
    LCD_WR_REG(0x11);       //Exit Sleep
    HAL_Delay(120);
    LCD_WR_REG(0x29);       //display on
	LCD_Display_Dir(0);         //默认为竖屏
    LCD_LED_ON;                //点亮背光
	LCD_Clear(BLACK);
	font_init();//初始化中文字库
}

//清屏函数
//color:要清屏的填充色
void LCD_Clear(uint16_t color)
{
    uint32_t index = 0;
    uint32_t totalpoint = lcddev.width;
    totalpoint *= lcddev.height;    //得到总点数

    LCD_SetCursor(0x00, 0x0000);    //设置光标位置
    LCD_WriteRAM_Prepare();         //开始写入GRAM

    for (index = 0; index < totalpoint; index++)
    {
        LCD_WR_DATA(color);
    }
}

//在指定区域内填充指定颜色
//区域大小:(xend-xsta+1)*(yend-ysta+1)
//xsta
//color:要填充的颜色
void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
{          
    uint16_t i, j;
    uint16_t xlen = 0;

    xlen = ex - sx + 1;

    for (i = sy; i <= ey; i++)
    {
        LCD_SetCursor(sx, i);       //设置光标位置
        LCD_WriteRAM_Prepare();     //开始写入GRAM

        for (j = 0; j < xlen; j++)
        {
            LCD_WR_DATA(color);     //设置光标位置
        }
    }
}

//在指定区域内填充指定颜色块
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
//color:要填充的颜色
void LCD_Color_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t *color)
{  
    uint16_t height, width;
    uint16_t i, j;
    width = ex - sx + 1;            //得到填充的宽度
    height = ey - sy + 1;           //高度

    for (i = 0; i < height; i++)
    {
        LCD_SetCursor(sx, sy + i);  //设置光标位置
        LCD_WriteRAM_Prepare();     //开始写入GRAM

        for (j = 0; j < width; j++)
        {
            LCD_WR_DATA(color[i * width + j]); //写入数据
        }
    }
}


//画线
//x1,y1:起点坐标
//x2,y2:终点坐标  
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1;              //计算坐标增量
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;

    if (delta_x > 0)incx = 1;       //设置单步方向
    else if (delta_x == 0)incx = 0; //垂直线
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)incy = 1;
    else if (delta_y == 0)incy = 0; //水平线
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y)distance = delta_x; //选取基本增量坐标轴
    else distance = delta_y;

    for (t = 0; t <= distance + 1; t++ )    //画线输出
    {
        LCD_DrawPoint(uRow, uCol); //画点
        xerr += delta_x ;
        yerr += delta_y ;

        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}


//画矩形	  
//(x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    LCD_DrawLine(x1, y1, x2, y1);
    LCD_DrawLine(x1, y1, x1, y2);
    LCD_DrawLine(x1, y2, x2, y2);
    LCD_DrawLine(x2, y1, x2, y2);
}


//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t size,uint8_t mode)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);  //得到字体一个字符对应点阵集所占的字节数
    num = num - ' ';    //得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）

    for (t = 0; t < csize; t++)
    {
        if (size == 12)temp = asc2_1206[num][t];        //调用1206字体
        else if (size == 16)temp = asc2_1608[num][t];   //调用1608字体
        else if (size == 24)temp = asc2_2412[num][t];   //调用2412字体
        else return;                                    //没有的字库

        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)LCD_Fast_DrawPoint(x, y, POINT_COLOR);
            else if (mode == 0)LCD_Fast_DrawPoint(x, y, BACK_COLOR);

            temp <<= 1;
            y++;

            if (y >= lcddev.height)return;      //超区域了

            if ((y - y0) == size)
            {
                y = y0;
                x++;

                if (x >= lcddev.width)return;   //超区域了

                break;
            }
        }
    }
}

//m^n函数
//返回值:m^n次方.
uint32_t Pow(uint8_t m,uint8_t n)
{
    uint32_t result=1;
    while(n--)result*=m;    
    return result;
}

//显示数字,高位为0,则不显示
//x,y :起点坐标
//len :数字的位数
//size:字体大小
//color:颜色 
//num:数值(0~4294967295);
void LCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)
    {
        temp = (num / Pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                LCD_ShowChar(x + (size / 2)*t, y, ' ', size, 0);
                continue;
            }
            else enshow = 1;

        }

        LCD_ShowChar(x + (size / 2)*t, y, temp + '0', size, 0);
    }
} 

//显示数字,高位为0,还是显示
//x,y:起点坐标
//num:数值(0~999999999);
//len:长度(即要显示的位数)
//size:字体大小
//mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
void LCD_ShowxNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size,uint8_t mode)
{  
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)
    {
        temp = (num / Pow(10, len - t - 1)) % 10;

        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                if (mode & 0X80)LCD_ShowChar(x + (size / 2)*t, y, '0', size, mode & 0X01);
                else LCD_ShowChar(x + (size / 2)*t, y, ' ', size, mode & 0X01);

                continue;
            }
            else enshow = 1;

        }

        LCD_ShowChar(x + (size / 2)*t, y, temp + '0', size, mode & 0X01);
    }
}


//显示字符串
//x,y:起点坐标
//width,height:区域大小
//size:字体大小
//*p:字符串起始地址
void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t size,uint8_t *p)
{         
    uint8_t x0 = x;
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' '))   //判断是不是非法字符!
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)break; //退出

        LCD_ShowChar(x, y, *p, size, 0);
        x += size / 2;
        p++;
    }
}

///////////////////////中文显示部分

_font_info ftinfo;


//初始化字体
//返回值:0,字库完好.
//		 其他,字库丢失
uint8_t font_init(void)
{	
 	uint8_t t=0;
	while(t<10)//连续读取10次,都是错误,说明确实是有问题,得更新字库了
	{
		t++;
		SPI_FLASH_BufferRead((uint8_t*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));//读出ftinfo结构体数据
		if(ftinfo.fontok==0XAA)break;
		HAL_Delay(20);
	}
	if(ftinfo.fontok!=0XAA)return 1;
	return 0;		    	    
}


//code 字符指针开始
//从字库中查找出字模
//code 字符串的开始地址,GBK码
//mat  数据存放地址 (size/8+((size%8)?1:0))*(size) bytes大小	
//size:字体大小
void Get_HzMat(unsigned char *code,unsigned char *mat,uint8_t size)
{		    
	unsigned char qh,ql;
	unsigned char i;					  
	unsigned long foffset; 
	uint8_t csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数	 
	qh=*code;
	ql=*(++code);
	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//非常用汉字
	{   		    
	    for(i=0;i<csize;i++)*mat++=0x00;//填充满格
	    return; //结束访问
	}          
	if(ql<0x7f)ql-=0x40;//注意!
	else ql-=0x41;
	qh-=0x81;   
	foffset=((unsigned long)190*qh+ql)*csize;	//得到字库中的字节偏移量  		  
	switch(size)
	{
		case 12:
			SPI_FLASH_BufferRead(mat,foffset+ftinfo.f12addr,24);
			break;
		case 16:
			SPI_FLASH_BufferRead(mat,foffset+ftinfo.f16addr,32);
			break;
		case 24:
			SPI_FLASH_BufferRead(mat,foffset+ftinfo.f24addr,72);
			break;
			
	}
}  
//显示一个指定大小的汉字
//x,y :汉字的坐标
//font:汉字GBK码
//size:字体大小
//mode:0,正常显示,1,叠加显示	   
void Show_Font(uint16_t x,uint16_t y,uint8_t *font,uint8_t size,uint8_t mode)
{
	uint8_t temp,t,t1;
	uint16_t y0=y;
	uint8_t dzk[72];   
	uint8_t csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数	 
	if(size!=12&&size!=16&&size!=24)return;	//不支持的size
	Get_HzMat(font,dzk,size);	//得到相应大小的点阵数据 
	for(t=0;t<csize;t++)
	{   												   
		temp=dzk[t];			//得到点阵数据                          
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR); 
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
	}  
}
//在指定位置开始显示一个字符串	    
//支持自动换行
//(x,y):起始坐标
//width,height:区域
//str  :字符串
//size :字体大小
//mode:0,非叠加方式;1,叠加方式    	   		   
void Show_Str(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t*str,uint8_t size,uint8_t mode)
{					
	uint16_t x0=x;
	uint16_t y0=y;							  	  
    uint8_t bHz=0;     //字符或者中文  	    	
    while(*str!=0)//数据未结束
    { 
        if(!bHz)
        {
	        if(*str>0x80)bHz=1;//中文 
	        else              //字符
	        {      
                if(x>(x0+width-size/2))//换行
				{				   
					y+=size;
					x=x0;	   
				}							    
		        if(y>(y0+height-size))break;//越界返回      
		        if(*str==13)//换行符号
		        {         
		            y+=size;
					x=x0;
		            str++; 
		        }  
		        else LCD_ShowChar(x,y,*str,size,mode);//有效部分写入 
				str++; 
		        x+=size/2; //字符,为全字的一半 
	        }
        }else//中文 
        {
            bHz=0;//有汉字库    
            if(x>(x0+width-size))//换行
			{	    
				y+=size;
				x=x0;		  
			}
	        if(y>(y0+height-size))break;//越界返回  						     
	        Show_Font(x,y,str,size,mode); //显示这个汉字,空心显示 
	        str+=2; 
	        x+=size;//下一个汉字偏移	    
        }						 
    }   
}  			 		 
//在指定宽度的中间显示字符串
//如果字符长度超过了len,则用Show_Str显示
//len:指定要显示的宽度			  
void Show_Str_Mid(uint16_t x,uint16_t y,uint8_t*str,uint8_t size,uint8_t len)
{
	uint16_t strlenth=0;
   	strlenth=strlen((const char*)str);
	strlenth*=size/2;
	if(strlenth>len)Show_Str(x,y,lcddev.width,lcddev.height,str,size,1);
	else
	{
		strlenth=(len-strlenth)/2;
	    Show_Str(strlenth+x,y,lcddev.width,lcddev.height,str,size,1);
	}
}   
