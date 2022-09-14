#include "INA219.h"
#include "timeServer.h"

I2C_HandleTypeDef I2cHandle4;

/* I2C TIMING Register define when I2C clock source is SYSCLK */
/* I2C TIMING is calculated in case of the I2C Clock source is the SYSCLK = 32 MHz */

#define I2C_TIMING    0x10A13E56 /* 100 kHz with analog Filter ON, Rise Time 400ns, Fall Time 100ns */ 
//#define I2C_TIMING      0x00B1112E /* 400 kHz with analog Filter ON, Rise Time 250ns, Fall Time 100ns */

//Lifted straight from the Adafruit driver
static uint16_t CalibrationRegVal = 4096; 
static uint16_t ConfigurationRegVal = 0x2000 | 0x1800 | 0x0180 | 0x0018 | 0x03;
static uint16_t  INA219_address = 0x40<<1;

static uint16_t INA219_read_register(uint8_t addr);
static void INA219_send(uint8_t *p_Data, uint16_t Size);
static uint32_t currentDivider = 10;

void BSP_INA219_Init(void)
{
  /*##-1- Configure the I2C peripheral ######################################*/
  I2cHandle4.Instance              = I2Cx;
  I2cHandle4.Init.Timing           = I2C_TIMING;
  I2cHandle4.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
  I2cHandle4.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
  I2cHandle4.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  I2cHandle4.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
  I2cHandle4.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;  
  I2cHandle4.Init.OwnAddress1      = 0xF0;
  I2cHandle4.Init.OwnAddress2      = 0xFE;
  
  if(HAL_I2C_Init(&I2cHandle4) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Enable the Analog I2C Filter */
  HAL_I2CEx_ConfigAnalogFilter(&I2cHandle4,I2C_ANALOGFILTER_ENABLE);  
}

void INA219_Init (void)
{
	PPRINTF("INA219 CAL \r\n");
	uint8_t addr[1];
  uint8_t msg[2];
	
	BSP_INA219_Init();

	addr[0] = 0x05;
  msg[0] = CalibrationRegVal>>8;
  msg[1] = CalibrationRegVal & 0xFF;

  //HAL_I2C_Mem_Write(&I2cHandle4,INA219_address,0x05,1,msg,2,1000);
	INA219_send(addr,1);
	INA219_send(msg,2);

	addr[0] = 0x00;
  msg[0] = ConfigurationRegVal>>8;
  msg[1] = ConfigurationRegVal & 0xFF;

  //HAL_I2C_Mem_Write(&I2cHandle4,INA219_address,0x00,1,msg,2,1000);
	INA219_send(addr,1);
	INA219_send(msg,2);
}

void INA219_DeInit (void)
{
	PPRINTF("INA219 Power-down \r\n");
	uint8_t addr[1];
  uint8_t msg[2];
	
	addr[0] = 0x00;
  msg[0] = ConfigurationRegVal>>8;
  msg[1] = (ConfigurationRegVal & 0xFF) & (~0x03) ;

  //HAL_I2C_Mem_Write(&I2cHandle4,INA219_address,0x00,1,msg,2,1000);
	INA219_send(addr,1);
	INA219_send(msg,2);
}

uint16_t INA219_read_bus_voltage(void)
{
	uint16_t ret = INA219_read_register(0x02);
  return (ret>>3)*4; //mV  
}

uint16_t INA219_read_bus_current(void)
{
	uint16_t ret = INA219_read_register(0x04);
  return ret/currentDivider; //mV  
}

static void INA219_send(uint8_t *p_Data, uint16_t Size)
{
	uint32_t currentTime = TimerGetCurrentTime();
	while(HAL_I2C_Master_Transmit(&I2cHandle4,INA219_address,p_Data,Size,1000) != HAL_OK)
	{
			if(TimerGetElapsedTime(currentTime) >= 300)
			{
				//read_status=0;
				break;
			}
			if(HAL_I2C_GetError(&I2cHandle4) != HAL_I2C_ERROR_AF)
			{}
	}
}

static uint16_t INA219_read_register(uint8_t addr)
{
  uint8_t msg[2] = {0,0};

  INA219_send(&addr, 1);

	uint32_t currentTime = TimerGetCurrentTime();		
	while(HAL_I2C_Master_Receive(&I2cHandle4,INA219_address,msg,2,1000) != HAL_OK)  
	{
			if(TimerGetElapsedTime(currentTime) >= 300)
			{
				break;
			}
			if(HAL_I2C_GetError(&I2cHandle4) != HAL_I2C_ERROR_AF)
			{}
	}	
	
	PPRINTF("%x%x\r\n",msg[0],msg[1]);

  return ((uint16_t)((msg[0]<<8) | msg[1])); //mV  
}

