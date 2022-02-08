#include "PinMap.h"


static uint16_t left, right;
static unsigned char count = 0;
void delayMs(uint32_t ms);

static __IO uint32_t msTicks;

void delay(int t){ //1000 around 1s
	int i,j;
	for(i = 0; i < 6500; i++)
		for(j = 0; j < t; j++);
}


// when this function is called, we will get the readings later
void readFloor() 
{
	// Set PB15 to 1
	GPIO_SetBits(GPIOB, GPIO_Pin_15);
	GPIO_WriteBit(LED_GPIO, LED_PIN, Bit_SET);
	// Initialize the data transmission from the master to the slave
	SPI_I2S_SendData(SPI2, 0);
	// Enable the interrupt to receive data by using the ISR handler
	NVIC_EnableIRQ(SPI2_IRQn);
}
int a = 0;

// put the readings to the variable c
void SPI2_IRQHandler() 
{
	//while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);
	char c = (char) SPI_I2S_ReceiveData(SPI2) & 0xff;
	//because of master slave connection, we have 2 8 bit data and we need only the second one
	
	if (a == 0)
		a = 1;
	else
	{
		
		//check the register before sending data
	  while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	  USART_SendData(USART2, 'a');
	
	  for (int i = 7; i>=0; i--) {
			
		  if (c & (1<<i)) {
			  while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
			  USART_SendData(USART2, '1');}
		  else {
			  while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
			  USART_SendData(USART2, '0');}
		  }
	  while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	  USART_SendData(USART2, ' ');
		
		
		a = 0;
	

	if (count == 0) { 

		if (c == 0xff || c == 0x7f) {

			GPIO_WriteBit(LED_GPIO, LED_PIN, Bit_RESET);
			count++;

		} 		
	
		else if (c & (1<<6)) {
			//turn to left 
			left = 6000;
			right = 4600;
		} 
		
		else if (c & (1<<3)){
			left = 4600;
			right = 6000;
		} 
		
	}
	
	// need to turn more for 2 of the edges 
	else if (count == 1 ) {
		if (c == 0xff || c == 0x7f) {

			GPIO_WriteBit(LED_GPIO, LED_PIN, Bit_RESET);
			count++;

		} else if (c & (1<<6)) {
			//turn to left 
			left = 6100;
			right = 4600;
		} 
		
		else if (c & (1<<3)){
			left = 4600;
			right = 6100;
		} 
	}
	else if (count == 2 ) {
		//when passing through the edges
		if (c == 0xff || c == 0x7f) {
			GPIO_WriteBit(LED_GPIO, LED_PIN, Bit_RESET);
			count++;

		} else if (c & (1<<6)) {
			//turn to left 
			left = 6000;
			right = 4400;
		} 
		
		else if (c & (1<<3)){
			//turn to right 
			left = 4400;
			right = 6000;
		} 
	}
	// to enter the circle
	else if (count == 3) {
		if (c == 0xff || c == 0x7f) {

			GPIO_WriteBit(LED_GPIO, LED_PIN, Bit_RESET);
			count++;

		} 
		else if (c & (1<<6)) {
			//turn to left 
			left = 6000;
			right = 4300;
		} 
		
		else if (c & (1<<3)){
			//turn to right 
			left = 4300;
			right = 6000;
		} 
	}
	
	else {
		right = 0;
		left = 0;
	}
	
		PWM_L_update(0, left);
		PWM_R_update(0, right);

	}

	// Check PB15. If it is 1, it means the data is ready
	if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_15) == 1) 
	{
	  // Set PB15 to 0 to trigger the shift register
	  GPIO_ResetBits(GPIOB, GPIO_Pin_15);
	  // Go to get the next reading
	  SPI_I2S_SendData(SPI2, 0);
		
	} 
	else 
	{
	  // disable the interrupt because it is not ready
	  NVIC_DisableIRQ(SPI2_IRQn);
	}
}



void delayMs(uint32_t ms)
{
	// Reload us value
	msTicks = ms;
	// Wait until usTick reach zero
	while (msTicks);
}

// SysTick_Handler function will be called every 1 m(s)
void SysTick_Handler()
{
	if (msTicks != 0)
	{
		msTicks--;
	}
}


int main(void) {
	
	USART2_init();
	WHEEL_PWM_init();
	SPI2_init();
	LED_Init();
	
	// Update SystemCoreClock value
	SystemCoreClockUpdate();
	
	// Configure the SysTick timer to overflow every 1 m(s)
	SysTick_Config(SystemCoreClock / 1000);
	

	while(1) {
		//GPIO_SetBits(GPIOA, GPIO_Pin_5);
		readFloor(); 
		//GPIO_WriteBit(LED_GPIO, LED_PIN, Bit_SET);
		delayMs(130);
		
		
	}
}

