//Aidina Takhtanova
//eie3105

#include <stdio.h>
#include <stdlib.h>
#include "stdbool.h"
#include "stm32f10x_exti.h"
#include "stm32f10x.h"                  // Device header

// TIM3 PWM
#define TIM3_PWM_RCC_GPIO RCC_APB2Periph_GPIOA
#define TIM3_PWM_GPIOA GPIOA
#define TIM3_PWM_GPIOB GPIOB

#define TIM3_CH1_PWM_PIN GPIO_Pin_6
#define TIM3_CH2_PWM_PIN GPIO_Pin_7
#define TIM3_CH3_PWM_PIN GPIO_Pin_1
#define TIM3_CH4_PWM_PIN GPIO_Pin_0

int msTicks = 0;
int state = 0;

void RCC_init(void) {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_SPI2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
}

void SPI2_init(void) {
    //Initialize PB14(input), PB13, 15(output)
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //Set as input
    GPIO_Init(GPIOB, & GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //Set as output
    GPIO_Init(GPIOB, & GPIO_InitStructure);

    //Define SPI
    //Default settings
    SPI_InitTypeDef SPI_InitStructure;
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //Receive and transmit at same time
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master; //Use master SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; //Send and receive 8 bits
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPI2, & SPI_InitStructure);
    SPI_Cmd(SPI2, ENABLE);

    //NVIC -> Handle interrupt priority
    NVIC_InitTypeDef NVIC_InitStructure;
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE); //Enable SPI interrupt [linked with SR register, RXNE]
    NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( & NVIC_InitStructure);
}

void TIM2_init() {
    TIM_TimeBaseInitTypeDef timerInitStructure;
    timerInitStructure.TIM_Prescaler = 18000;
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = 50;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, & timerInitStructure);
    TIM_Cmd(TIM2, ENABLE);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    NVIC_EnableIRQ(TIM2_IRQn); //Enable interrupt of timer 2
    NVIC_InitTypeDef NVIC_InitStructure;
    // Enable the USART2 TX Interrupt
    USART_ITConfig(USART2, USART_IT_TC, ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( & NVIC_InitStructure);
}

void USART2_init(void) {
    //USART2 TX RX
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, & GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, & GPIO_InitStructure);

    //USART2 ST-LINK USB
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    USART_InitTypeDef USART_InitStructure;
    //USART_ClockInitTypeDef USART_ClockInitStructure;

    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, & USART_InitStructure);
    USART_Cmd(USART2, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;
    // Enable the USART2 TX Interrupt
    USART_ITConfig(USART2, USART_IT_TC, ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( & NVIC_InitStructure);
    // Enable the USART2 RX Interrupt
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( & NVIC_InitStructure);
}

void TIM3_PWM_init(void) {
    RCC_APB2PeriphClockCmd(TIM3_PWM_RCC_GPIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    // Configure I/O for Tim3 Ch1 PWM pin
    GPIO_InitStructure.GPIO_Pin = TIM3_CH1_PWM_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(TIM3_PWM_GPIOA, & GPIO_InitStructure);

    // Configure I/O for Tim3 Ch2 PWM pin
    GPIO_InitStructure.GPIO_Pin = TIM3_CH2_PWM_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(TIM3_PWM_GPIOA, & GPIO_InitStructure);

    // Configure I/O for Tim3 Ch3 PWM pin
    GPIO_InitStructure.GPIO_Pin = TIM3_CH3_PWM_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(TIM3_PWM_GPIOB, & GPIO_InitStructure);

    // Configure I/O for Tim3 Ch4 PWM pin
    GPIO_InitStructure.GPIO_Pin = TIM3_CH4_PWM_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(TIM3_PWM_GPIOB, & GPIO_InitStructure);

    //Tim3 set up
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseInitTypeDef timerInitStructure;
    timerInitStructure.TIM_Prescaler = 720 - 1; //1/(72Mhz/1440)=0.2ms
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = 200 - 1;
    timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    timerInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, & timerInitStructure);
    TIM_Cmd(TIM3, ENABLE);

    TIM_OCInitTypeDef outputChannelInit;
    //Enable Tim3 Ch1 PWM
    outputChannelInit.TIM_OCMode = TIM_OCMode_PWM1;
    outputChannelInit.TIM_Pulse = 1 - 1;
    outputChannelInit.TIM_OutputState = TIM_OutputState_Enable;
    outputChannelInit.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM3, & outputChannelInit);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

    //Enable Tim3 Ch2 PWM
    outputChannelInit.TIM_OCMode = TIM_OCMode_PWM1;
    outputChannelInit.TIM_Pulse = 1 - 1;
    outputChannelInit.TIM_OutputState = TIM_OutputState_Enable;
    outputChannelInit.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC2Init(TIM3, & outputChannelInit);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

    //Enable Tim3 Ch3 PWM
    outputChannelInit.TIM_OCMode = TIM_OCMode_PWM1;
    outputChannelInit.TIM_Pulse = 1 - 1;
    outputChannelInit.TIM_OutputState = TIM_OutputState_Enable;
    outputChannelInit.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC3Init(TIM3, & outputChannelInit);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

    //Enable Tim3 Ch4 PWM
    outputChannelInit.TIM_OCMode = TIM_OCMode_PWM1;
    outputChannelInit.TIM_Pulse = 1 - 1;
    outputChannelInit.TIM_OutputState = TIM_OutputState_Enable;
    outputChannelInit.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC4Init(TIM3, & outputChannelInit);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
}

//PID Control Variables
int R_count = 0, L_count = 0;

void STATE_Control() {
    L_count = 0;
    R_count = 0;

    // Forward
    if (state == 1) {
        L_count = 50;
        R_count = 54;
    }
    // Backward
    else if (state == 2) {
        L_count = -55;
        R_count = -50;
    }
    // left-forward
    else if (state == 3) {
        L_count = 0;
        R_count = 55;
    }
    // left-backward
    else if (state == 4) {
        L_count = 0;
        R_count = -50;
    }
    // right-forward
    else if (state == 5) {
        L_count = 50;
        R_count = 0;
    }
    // right-backward
    else if (state == 6) {
        L_count = -60;
        R_count = 0;
    } else {
        L_count = 0;
        R_count = 0;
    }
}

int checkMe = 0;

void PID_compare() {
    if (msTicks > 1000) {
        STATE_Control();
        if (checkMe == 0) {
            if (R_count < 0) {
               
                TIM_SetCompare4(TIM3, -R_count);
            } else {
                
                TIM_SetCompare3(TIM3, R_count);
            }
            if (L_count < 0) {
             
                TIM_SetCompare1(TIM3, -L_count);
            } else {
               
                TIM_SetCompare2(TIM3, L_count);
            }
        } else {
            L_count = 0;
            R_count = 0;
            TIM_SetCompare1(TIM3, 0);
            TIM_SetCompare2(TIM3, 0);
            TIM_SetCompare3(TIM3, 0);
            TIM_SetCompare4(TIM3, 0);
        }
    } else {
        L_count = 0;
        R_count = 0;
        TIM_SetCompare1(TIM3, 0);
        TIM_SetCompare2(TIM3, 0);
        TIM_SetCompare3(TIM3, 0);
        TIM_SetCompare4(TIM3, 0);
    }
    R_count = 0;
    L_count = 0;
}

void SysTick_Handler(void) {
    if (msTicks > 65535) {
        msTicks = 0;
        return;
    }

    msTicks++;

    if (msTicks > 1000 && msTicks < 2000) {
        state = 1;
    } else if (msTicks > 2000 && msTicks < 3000) {
        state = 0;
        L_count = 0;
        R_count = 0;
        TIM_SetCompare1(TIM3, 0);
        TIM_SetCompare2(TIM3, 0);
        TIM_SetCompare3(TIM3, 0);
        TIM_SetCompare4(TIM3, 0);
    } else if (msTicks > 3000 && msTicks < 4000) {
        state = 2;
    } else if (msTicks > 4000 && msTicks < 5000) {
        state = 0;
        L_count = 0;
        R_count = 0;
        TIM_SetCompare1(TIM3, 0);
        TIM_SetCompare2(TIM3, 0);
        TIM_SetCompare3(TIM3, 0);
        TIM_SetCompare4(TIM3, 0);
    } else if (msTicks > 5000 && msTicks < 6000) {
        state = 3;
    } else if (msTicks > 6000 && msTicks < 7000) {
        state = 0;
        L_count = 0;
        R_count = 0;
        TIM_SetCompare1(TIM3, 0);
        TIM_SetCompare2(TIM3, 0);
        TIM_SetCompare3(TIM3, 0);
        TIM_SetCompare4(TIM3, 0);
    } else if (msTicks > 7000 && msTicks < 8000) {
        state = 4;
    } else if (msTicks > 8000 && msTicks < 9000) {
        state = 0;
        L_count = 0;
        R_count = 0;
        TIM_SetCompare1(TIM3, 0);
        TIM_SetCompare2(TIM3, 0);
        TIM_SetCompare3(TIM3, 0);
        TIM_SetCompare4(TIM3, 0);
    } else if (msTicks > 9000 && msTicks < 10000) {
        state = 5;
    } else if (msTicks > 10000 && msTicks < 11000) {
        state = 0;
        L_count = 0;
        R_count = 0;
        TIM_SetCompare1(TIM3, 0);
        TIM_SetCompare2(TIM3, 0);
        TIM_SetCompare3(TIM3, 0);
        TIM_SetCompare4(TIM3, 0);
    } else if (msTicks > 11000 && msTicks < 12000) {
        state = 6;
    } else if (msTicks > 12000) {
        state = 0;
        L_count = 0;
        R_count = 0;
        TIM_SetCompare1(TIM3, 0);
        TIM_SetCompare2(TIM3, 0);
        TIM_SetCompare3(TIM3, 0);
        TIM_SetCompare4(TIM3, 0);
        while (true);
    }
}

//SPI
char spi;
int _spi[8] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

void SPI2_IRQHandler() {
    if (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == SET && SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == SET) {
        if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_12) == SET) {
            GPIO_ResetBits(GPIOB, GPIO_Pin_12);
            SPI_I2S_SendData(SPI2, 0xff);
            char spi = SPI_I2S_ReceiveData(SPI2) & 0xff;
            for (int i = 0; i < 8; i++) {
                if (spi & (1 << i))
                    _spi[i] = 0;
                else
                    _spi[i] = 1;
            }
        } else {
            GPIO_SetBits(GPIOB, GPIO_Pin_12);
            SPI_I2S_SendData(SPI2, 0);
        }
        SPI_I2S_ClearITPendingBit(SPI2, SPI_I2S_FLAG_RXNE);
        SPI_I2S_ClearITPendingBit(SPI2, SPI_I2S_FLAG_TXE);
    }
}

//USART2
bool flag = true;
char buffer[8] = {
    ""
};
int send = 0;

void TIM2_IRQHandler(void) {

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI2, 0xff);

    sprintf(buffer, "%d%d%d%d%d%d%d%d\r\n", _spi[0], _spi[1], _spi[2], _spi[3], _spi[4], _spi[5], _spi[6], _spi[7]);

    if (!flag)
        flag = true;
    PID_compare();

    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
} 

void USART2_IRQHandler() {
    if (USART_GetITStatus(USART2, USART_IT_TC) != RESET) {
        if (send < sizeof(buffer) && flag)
            USART_SendData(USART2, buffer[send++]);
        else if (flag) {
            send = 0;
            flag = false;
            return;
        }
    }
    return;
}

int main(void) {
    USART2_init();
    RCC_init();
    SPI2_init();
    TIM2_init();
    TIM3_PWM_init();

    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = SysTick_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( & NVIC_InitStructure);

    while (1);
}
