//
// This file is part of the GNU ARM Eclipse Plug-ins project.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------


// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

//Includes
#include "misc.h"
#include "stm32f4xx.h"
/*
#include "stm32f4xx_dac.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"
 */

//Function prototypes
void RCCInit(void);
void GPIOInit(void);
void TimerInit(void);
void EnableTimerInterrupt(void);
void ADCInit(void);
void DACInit(void);

int main (int argc, char* argv[])
{
	// Normally at this stage most of the microcontroller subsystems, including
	// the clock, were initialised by the CMSIS SystemInit() function invoked
	// from the startup file, before calling main().
	// (see system/src/cortexm/_initialize_hardware.c)
	// If further initialisations are required, customise __initialize_hardware()
	// or add the additional initialisation here, for example:

	// Initialization
	RCCInit();
	EnableTimerInterrupt();
	GPIOInit();
	TimerInit();
	ADCInit();
	DACInit();

	// Infinite loop
	while (1)
	{

	}
	// Infinite loop, never return.
}

// Initialize clocking
void RCCInit(void)
{
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
}

// Initialize GPIOs
void GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_init_structure;

	// LED GPIO
	/*
	GPIO_init_structure.GPIO_Pin = GPIO_Pin_1;
	GPIO_init_structure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_init_structure);
	GPIO_WriteBit(GPIOC, GPIO_Pin_1, Bit_SET);
	 */

	// ADC LED/Test GPIO
	GPIO_StructInit(&GPIO_init_structure);
	GPIO_init_structure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_init_structure.GPIO_OType = GPIO_OType_PP;
	GPIO_init_structure.GPIO_Pin   = GPIO_Pin_1; // C1
	GPIO_init_structure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_init_structure.GPIO_Speed = GPIO_Medium_Speed;
	GPIO_Init(GPIOC, &GPIO_init_structure);

	// DAC GPIO
	/* Once the DAC channel is enabled, the corresponding GPIO pin is automatically
	connected to the DAC converter. In order to avoid parasitic consumption,
	the GPIO pin should be configured in analog */
	GPIO_StructInit(&GPIO_init_structure);
	GPIO_init_structure.GPIO_Pin =  GPIO_Pin_4;// A4
	GPIO_init_structure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_init_structure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_init_structure);

	// ADC GPIO
	GPIO_StructInit(&GPIO_init_structure);
	GPIO_init_structure.GPIO_Pin =  GPIO_Pin_0;// C0
	GPIO_init_structure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_init_structure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_init_structure);
}

//Function for setting up the timer
void TimerInit(void)
{
	/*
	TIM_TimeBaseInitTypeDef timerInitStructure;
	timerInitStructure.TIM_Prescaler = 40000;
	timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStructure.TIM_Period = 500;
	timerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	timerInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &timerInitStructure);
	TIM_Cmd(TIM2, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	 */

	TIM_TimeBaseInitTypeDef TIM3_TimeBase;
	TIM_TimeBaseStructInit(&TIM3_TimeBase);
	/*
	TIM3_TimeBase.TIM_Period        = (uint16_t)124; // Trigger = CK_CNT/(Period+1) = 48kHz
	TIM3_TimeBase.TIM_Prescaler     = 7;          // CK_CNT = 42MHz/Prescaler = ?
	*/
	TIM3_TimeBase.TIM_Period        = (uint16_t)209; // Trigger = CK_CNT/(Period+1) = 200kHz
	TIM3_TimeBase.TIM_Prescaler     = 1;          // CK_CNT = 42MHz/Prescaler = ?

	TIM3_TimeBase.TIM_ClockDivision = 0;
	TIM3_TimeBase.TIM_CounterMode   = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM3_TimeBase);
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);

	TIM_Cmd(TIM3, ENABLE);
}

//Function to enable interrupts for the timer
void EnableTimerInterrupt(void)
{
	/*
    NVIC_InitTypeDef nvicStructure;
    nvicStructure.NVIC_IRQChannel = TIM2_IRQn;
    nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
    nvicStructure.NVIC_IRQChannelSubPriority = 1;
    nvicStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicStructure);
	 */
	NVIC_InitTypeDef NVIC_ADC1;
	NVIC_ADC1.NVIC_IRQChannel    = ADC_IRQn;
	NVIC_ADC1.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_ADC1);
}

void ADC_IRQHandler(void) // (for testing)
{
  GPIO_ToggleBits(GPIOC, GPIO_Pin_1);

  float adcVal = (float)ADC_GetConversionValue(ADC1);
  uint16_t outVal = (uint16_t)(adcVal*1.5);

  DAC_SetChannel1Data(DAC_Align_12b_R, outVal);
  ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
}

// Function for intializing the ADC
void ADCInit(void)
{
	ADC_InitTypeDef       ADC_INIT;
	ADC_CommonInitTypeDef ADC_COMMON;

	ADC_COMMON.ADC_Mode             = ADC_Mode_Independent;
	ADC_COMMON.ADC_Prescaler        = ADC_Prescaler_Div2;
	ADC_COMMON.ADC_DMAAccessMode    = ADC_DMAAccessMode_Disabled;
	ADC_COMMON.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_COMMON);

	ADC_INIT.ADC_Resolution           = ADC_Resolution_12b;
	ADC_INIT.ADC_ScanConvMode         = DISABLE;
	ADC_INIT.ADC_ContinuousConvMode   = DISABLE; // ENABLE for max ADC sampling frequency
	ADC_INIT.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
	ADC_INIT.ADC_ExternalTrigConv     = ADC_ExternalTrigConv_T3_TRGO;
	ADC_INIT.ADC_DataAlign            = ADC_DataAlign_Right;
	ADC_INIT.ADC_NbrOfConversion      = 1;
	ADC_Init(ADC1, &ADC_INIT);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_3Cycles);
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);      // (for testing)
	ADC_Cmd(ADC1, ENABLE);
}


//Function for initializing the DAC
void DACInit(void)
{
	DAC_InitTypeDef DAC_init_structure;

	DAC_init_structure.DAC_Trigger = DAC_Trigger_None ;
	DAC_init_structure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_init_structure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1, &DAC_init_structure);
	/* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is
     automatically connected to the DAC converter. */
	DAC_Cmd(DAC_Channel_1, ENABLE);
}

/*
//Define the timer's interrupt handler
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		GPIO_ToggleBits(GPIOC, GPIO_Pin_1);
	}
}
*/

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
