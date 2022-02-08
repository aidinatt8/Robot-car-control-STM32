#include "stm32f10x.h"

//PA6 TIM3 CH1 LEFT WHEEL BACKWARD
#define BACK_LEFT_WHEEL_PWM_RCC_GPIO  			RCC_APB2Periph_GPIOA
#define BACK_LEFT_WHEEL_PWM_GPIO      			GPIOA
#define BACK_LEFT_WHEEL_PWM_PIN       			GPIO_Pin_6

//PA8 TIM3 CH2 LEFT WHEEL FORWARD
#define FORWARD_LEFT_WHEEL_PWM_RCC_GPIO  		RCC_APB2Periph_GPIOA
#define FORWARD_LEFT_WHEEL_PWM_GPIO      		GPIOA
#define FORWARD_LEFT_WHEEL_PWM_PIN       		GPIO_Pin_7

//PB1 TIM3 CH3 RIGHT WHEEL FORWARD
#define FORWARD_RIGHT_WHEEL_PWM_RCC_GPIO  			RCC_APB2Periph_GPIOB
#define FORWARD_RIGHT_WHEEL_PWM_GPIO      			GPIOB
#define FORWARD_RIGHT_WHEEL_PWM_PIN       			GPIO_Pin_1

//PB0 TIM3 CH4 RIGHT WHEEL BACKWARD
#define BACK_RIGHT_WHEEL_PWM_RCC_GPIO  		RCC_APB2Periph_GPIOB
#define BACK_RIGHT_WHEEL_PWM_GPIO      		GPIOB
#define BACK_RIGHT_WHEEL_PWM_PIN       		GPIO_Pin_0

//PA5 LED
#define LED_RCC_GPIO  								RCC_APB2Periph_GPIOA
#define LED_GPIO      								GPIOA
#define LED_PIN       								GPIO_Pin_5

//	Function Prototype
void WHEEL_PWM_init(void);
void USART2_init(void);
void SPI2_init(void);
void USARTSend(char *pucBuffer, unsigned long ulCount);
void readFloor(void); 
void PWM_L_update(uint16_t back, uint16_t front);
void PWM_R_update(uint16_t back, uint16_t front);
void delay(int t);
void LED_Init(void);

