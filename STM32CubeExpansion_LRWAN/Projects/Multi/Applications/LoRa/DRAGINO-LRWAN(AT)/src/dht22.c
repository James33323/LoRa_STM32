#include "dht22.h"

void delay(uint16_t time)
{
  uint8_t i;

  while(time)
  {    
    for (i = 0; i < 31; i++)
    {
      __NOP();
    }
    time--;
  }
}


void DHT22_Start (void)
{
	
	GPIO_InitTypeDef GPIO_InitStruct={0};
	DHT22_CLK_ENABLE();
	
	GPIO_InitStruct.Pin 	= DHT22_PIN;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

  HW_GPIO_Init( DHT22_PORT, DHT22_PIN, &GPIO_InitStruct );

	HAL_GPIO_WritePin (DHT22_PORT, DHT22_PIN, 0); // pull the pin low
	delay(1200);   // wait for > 1ms

	HAL_GPIO_WritePin (DHT22_PORT, DHT22_PIN, 1); // pull the pin high
	delay (20);  // wait for 20us

	GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
	HW_GPIO_Init( DHT22_PORT, DHT22_PIN, &GPIO_InitStruct );
}

uint8_t DHT22_Check_Response (void)
{
	uint8_t Response = 0;
    delay(40);  // wait for 40us
	if (!(HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN))) // if the pin is low
	{
		delay (80); // wait for 80us

		if ((HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN))) Response = 1;  // if the pin is high, response is ok
		else Response = -1;
	}

	while ((HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)));  // wait for the pin to go low
	return Response;
}

uint8_t DHT22_Read (void)
{
	uint8_t i,j;
	for (j=0;j<8;j++)
	{
		while (!(HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)));  //wait for the pin to go high
		delay (40);  // wait for 40us

		if (!(HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)))  // if the pin is low
		{
			i&= ~(1<<(7-j));  // write 0
		}
		else i|= (1<<(7-j));  // if the pin is high, write 1
		while ((HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)));  // wait for the pin to go low
	}

	return i;
}
