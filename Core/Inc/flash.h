#ifndef __FLASH__
#define __FLASH__

#include "stm32f1xx_hal.h"

/* Private typedef -----------------------------------------------------------*/
//#define  sFLASH_ID                       0xEF3015     //W25X16
//#define  sFLASH_ID                       0xEF4015	    //W25Q16
#define  sFLASH_ID                        0XEF4017     //W25Q64
//#define  sFLASH_ID                       0XEF4018    //W25Q128


//#define SPI_FLASH_PageSize            4096
#define SPI_FLASH_PageSize              256
#define SPI_FLASH_PerWritePageSize      256

/* Private define ------------------------------------------------------------*/
//指令表
#define W25X_WriteEnable		      0x06
#define W25X_WriteDisable		      0x04
#define W25X_ReadStatusReg		    0x05
#define W25X_WriteStatusReg		    0x01
#define W25X_ReadData			        0x03
#define W25X_FastReadData		      0x0B
#define W25X_FastReadDual		      0x3B
#define W25X_PageProgram		      0x02
#define W25X_BlockErase			      0xD8
#define W25X_SectorErase		      0x20
#define W25X_ChipErase			      0xC7
#define W25X_PowerDown			      0xB9
#define W25X_ReleasePowerDown	    0xAB
#define W25X_DeviceID			        0xAB
#define W25X_ManufactDeviceID   	0x90
#define W25X_JedecDeviceID		    0x9F

//#define W25X_WriteEnable		0x06 
//#define W25X_WriteDisable		0x04 
//#define W25X_ReadStatusReg1		0x05 
//#define W25X_ReadStatusReg2		0x35 
//#define W25X_ReadStatusReg3		0x15 
//#define W25X_WriteStatusReg1    0x01 
//#define W25X_WriteStatusReg2    0x31 
//#define W25X_WriteStatusReg3    0x11 
//#define W25X_ReadData			0x03 
//#define W25X_FastReadData		0x0B 
//#define W25X_FastReadDual		0x3B 
//#define W25X_PageProgram		0x02 
//#define W25X_BlockErase			0xD8 
//#define W25X_SectorErase		0x20 
//#define W25X_ChipErase			0xC7 
//#define W25X_PowerDown			0xB9 
//#define W25X_ReleasePowerDown	0xAB 
//#define W25X_DeviceID			0xAB 
//#define W25X_ManufactDeviceID	0x90 
//#define W25X_JedecDeviceID		0x9F 
//#define W25X_Enable4ByteAddr    0xB7
//#define W25X_Exit4ByteAddr      0xE9

#define WIP_Flag                      0x01  /* Write In Progress (WIP) flag */
#define Dummy_Byte                    0xFF


/* Definition for SPIx Pins */
#define FLASH_CS_PIN                     GPIO_PIN_2              
#define FLASH_CS_GPIO_PORT               GPIOA                   

#define SPI_FLASH_CS_LOW()      HAL_GPIO_WritePin(FLASH_CS_GPIO_PORT,FLASH_CS_PIN,GPIO_PIN_RESET)
#define SPI_FLASH_CS_HIGH()     HAL_GPIO_WritePin(FLASH_CS_GPIO_PORT,FLASH_CS_PIN,GPIO_PIN_SET)

/*等待超时时间*/
#define SPIT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define SPIT_LONG_TIMEOUT         ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))

/*信息输出*/
#define FLASH_DEBUG_ON         	  1

#define FLASH_INFO(fmt,arg...)           printf("<<-FLASH-INFO->> "fmt"\n",##arg)
#define FLASH_ERROR(fmt,arg...)          printf("<<-FLASH-ERROR->> "fmt"\n",##arg)
#define FLASH_DEBUG(fmt,arg...)          if(FLASH_DEBUG_ON)\
											printf("<<-FLASH-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);



void SPI_FLASH_Init(void);
void SPI_FLASH_SectorErase(uint32_t SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t SPI_FLASH_ReadID(void);
uint32_t SPI_FLASH_ReadDeviceID(void);
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr);
void SPI_Flash_PowerDown(void);
void SPI_Flash_WAKEUP(void);


uint8_t SPI_FLASH_ReadByte(void);
uint8_t SPI_FLASH_SendByte(uint8_t byte);
uint16_t SPI_FLASH_SendHalfWord(uint16_t HalfWord);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);

#endif
