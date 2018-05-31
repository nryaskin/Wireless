#include "button.h"

#define B_PIN_INTENSITY GPIO_Pin_4
#define B_PIN_CHANNEL GPIO_Pin_3

//PB4, PB3
//EXTI4_IRQn -> EXTI4_IRQHandler
//EXTI3_IRQn -> EXTI3_IRQHandler

void Init_Peripherals(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitStruct.GPIO_Pin = B_PIN_INTENSITY | B_PIN_CHANNEL;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);


    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource4);

    EXTI_InitStruct.EXTI_Line = EXTI_Line4;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init(&EXTI_InitStruct);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource3);

    EXTI_InitStruct.EXTI_Line = EXTI_Line3;
    EXTI_Init(&EXTI_InitStruct);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, DISABLE);

    NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_Init(&NVIC_InitStruct);

}

void EXTI4_IRQHandler(void){
    if(EXTI_GetITStatus(EXTI_Line4) != RESET) {
        handle_left_button();
        EXTI_ClearITPendingBit(EXTI_Line4);
    }
}

void EXTI3_IRQHandler(void){
    if(EXTI_GetITStatus(EXTI_Line3) != RESET) {
        handle_right_button();
        EXTI_ClearITPendingBit(EXTI_Line3);
    }
}

