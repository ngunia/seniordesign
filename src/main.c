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
#include "tm_stm32f4_hd44780.h"

//Function prototypes
void RCCInit(void);
void GPIOInit(void);
void TimerInit(void);
void EnableTimerInterrupt(void);
void ADCInit(void);
void DACInit(void);
void init_pots(void);
uint16_t delay(uint16_t);
void num2Str(char*, uint16_t);
void display_pots(void);

/* for delay effect test*/
#define FS 48000
#define DEL_TIME 500 // ms
#define BUFF_SIZE (FS*DEL_TIME/1000)
uint16_t delayBuff[BUFF_SIZE];

// holds pot values
uint16_t pot[5] = {0,0,0,0,0};

int main (int argc, char* argv[])
{
	// Initialization
	RCCInit();
	TM_HD44780_Init(16, 2);
	EnableTimerInterrupt();
	GPIOInit();
	TimerInit();
	ADCInit();
	DACInit();
	init_pots();

	// TODO: move this somewhere more appropriate, make names configurable?
	TM_HD44780_Puts(0, 0, "AAA:");
	TM_HD44780_Puts(4, 0, "BBB:");
	TM_HD44780_Puts(8, 0, "CCC:");
	TM_HD44780_Puts(12, 0, "DDD:");

	while (1)
	{
		display_pots();
	}
	// Infinite loop, never return.
}

void display_pots(void) {
	// TODO add one for expression pedal..

	static uint8_t count = 0;

	// strings to hold values
	static char pot0Str[4];
	static char pot1Str[4];
	static char pot2Str[4];
	static char pot3Str[4];

	// scale to 0-100
	uint16_t pot0Scaled = (uint16_t)((pot[0] / 4095.0) * 100);
	uint16_t pot1Scaled = (uint16_t)((pot[1] / 4095.0) * 100);
	uint16_t pot2Scaled = (uint16_t)((pot[2] / 4095.0) * 100);
	uint16_t pot3Scaled = (uint16_t)((pot[3] / 4095.0) * 100);

	// convert int to str
	num2Str(pot0Str, pot0Scaled);
	num2Str(pot1Str, pot1Scaled);
	num2Str(pot2Str, pot2Scaled);
	num2Str(pot3Str, pot3Scaled);

	// print to LCD
	TM_HD44780_Puts(1, 1, pot0Str);
	TM_HD44780_Puts(5, 1, pot1Str);
	TM_HD44780_Puts(9, 1, pot2Str);
	TM_HD44780_Puts(13, 1, pot3Str);

	count++;
}

// converts a uint16_t to a char* for printing
void num2Str(char *res, uint16_t val) {
	res[3] = '\0'; // null terminate the string
	int i;
	for(i=2; i >= 0; i--) {
		uint8_t newVal = val/10;
		uint8_t rem = val%10;
		if (i < 2 && (rem == 0 && newVal == 0)) { // remove leading zero(s)
			res[i] = ' ';
		} else {// convert digit 0-9 as int to corresponding ascii char
			res[i] = (char)((int)'0'+rem);
		}
		val = newVal;
	}
}

// Initialize clocking
void RCCInit(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
}

// Initialize GPIOs
void GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_init_structure;

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
	GPIO_init_structure.GPIO_Pin =  GPIO_Pin_7;// A7
	GPIO_init_structure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_init_structure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_init_structure);
}

//Function for setting up the timer
void TimerInit(void)
{
	TIM_TimeBaseInitTypeDef TIM3_TimeBase;
	// TODO: remove unused speeds or make configurable with an enum
	TIM3_TimeBase.TIM_Period        = (uint16_t)209;  // Trigger = CK_CNT/(Period+1) = 200 kHz
	TIM3_TimeBase.TIM_Prescaler     = 1;          	  // CK_CNT = 42MHz/Prescaler = 42 Mhz
//
//	TIM3_TimeBase.TIM_Period        = (uint16_t)61;   // Trigger = CK_CNT/(Period+1) = 96.774 kHz
//	TIM3_TimeBase.TIM_Prescaler     = 7;          	  // CK_CNT = 42MHz/Prescaler = 6 Mhz
//
//	TIM3_TimeBase.TIM_Period        = (uint16_t)124;  // Trigger = CK_CNT/(Period+1) = 48 kHz
//	TIM3_TimeBase.TIM_Prescaler     = 7;          	  // CK_CNT = 42MHz/Prescaler = 6 Mhz
//
//	TIM3_TimeBase.TIM_Period        = (uint16_t)135; // Trigger = CK_CNT/(Period+1) = 44.117 kHz
//	TIM3_TimeBase.TIM_Prescaler     = 7;          	  // CK_CNT = 42MHz/Prescaler = 6 Mhz
//	TIM3_TimeBase.TIM_Period        = (uint16_t)1049;  // Trigger = CK_CNT/(Period+1) = 48 kHz
//	TIM3_TimeBase.TIM_Prescaler     = 40000;          	  // CK_CNT = 42MHz/Prescaler = 6 Mhz

	TIM3_TimeBase.TIM_ClockDivision = 0;
	TIM3_TimeBase.TIM_CounterMode   = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM3_TimeBase);
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);

	TIM_Cmd(TIM3, ENABLE);
}

//Function to enable interrupts for the timer
void EnableTimerInterrupt(void)
{
    NVIC_InitTypeDef nvicStructure;

    nvicStructure.NVIC_IRQChannel = TIM2_IRQn;
    nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
    nvicStructure.NVIC_IRQChannelSubPriority = 1;
    nvicStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicStructure);

	// enable all ADCS to be triggered
    nvicStructure.NVIC_IRQChannel    = ADC_IRQn;
    nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStructure);
}

// interrupt where ADC read, DAC write occurs
void ADC_IRQHandler(void) // (for testing)
{
  //DAC_SetChannel1Data(DAC_Align_12b_R, delay( ADC_GetConversionValue(ADC1) ) );
  DAC_SetChannel1Data(DAC_Align_12b_R, ADC_GetConversionValue(ADC1) );
  ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
}

uint16_t delay(uint16_t currSamp)
{
	static uint16_t buffInd = 0;
	uint16_t delaySamples = 12000;   // 250*FS/1000
	delayBuff[buffInd] = currSamp;

	int delInd = buffInd-delaySamples;

	if(delInd < 0)
		delInd = BUFF_SIZE + delInd; // actually subtraction

	if(++buffInd >= BUFF_SIZE)
		buffInd = 0;

	float currSampF = (float)currSamp;
	float delSampF = (float)delayBuff[delInd];

	return (uint16_t)(4095.0 * (0.5*currSampF + 0.5*delSampF) ); // dry mix + wet mix

}

// Function for intializing the ADC
void ADCInit(void)
{
	ADC_InitTypeDef       ADC_INIT;
	ADC_CommonInitTypeDef ADC_COMMON;

	// adc for left channel
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

	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_3Cycles);
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

void init_pots(void) {
	ADC_InitTypeDef ADC_InitStruct;
	ADC_CommonInitTypeDef ADC_CommonInitStruct;
	DMA_InitTypeDef DMA_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;

	/* DMA2 Stream0 channel0 configuration **************************************/
	DMA_InitStruct.DMA_Channel = DMA_Channel_2;
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC3->DR;//ADC3's data register
	DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)&pot;
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStruct.DMA_BufferSize = 5;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//Reads 16 bit values
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//Stores 16 bit values
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStruct);
	DMA_Cmd(DMA2_Stream0, ENABLE);

	/* Configure GPIO pins ******************************************************/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;// PC0, PC1, PC2, PC3
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;//The pins are configured in analog mode
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;//We don't need any pull up or pull down
	GPIO_Init(GPIOC, &GPIO_InitStruct);//Initialize GPIOC pins with the configuration
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;//PA0
	GPIO_Init(GPIOA, &GPIO_InitStruct);//Initialize GPIOA pins with the configuration

	/* ADC Common Init **********************************************************/
	ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStruct);

	/* ADC3 Init ****************************************************************/
	ADC_DeInit();
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;//Input voltage is converted into a 12bit int (max 4095)
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;//The scan is configured in multiple channels
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;//Continuous conversion: input signal is sampled more than once
	ADC_InitStruct.ADC_ExternalTrigConv = 0;
	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//Data converted will be shifted to right
	ADC_InitStruct.ADC_NbrOfConversion = 5;
	ADC_Init(ADC3, &ADC_InitStruct);//Initialize ADC with the configuration

	/* Select the channels to be read from **************************************/
	ADC_RegularChannelConfig(ADC3, ADC_Channel_10, 1, ADC_SampleTime_144Cycles);//PC0
	ADC_RegularChannelConfig(ADC3, ADC_Channel_11, 2, ADC_SampleTime_144Cycles);//PC1
	ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 3, ADC_SampleTime_144Cycles);//PC2
	ADC_RegularChannelConfig(ADC3, ADC_Channel_13, 4, ADC_SampleTime_144Cycles);//PC3
	ADC_RegularChannelConfig(ADC3, ADC_Channel_0, 5, ADC_SampleTime_144Cycles);//PA0

	/* Enable DMA request after last transfer (Single-ADC mode) */
	ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);

	/* Enable ADC3 DMA */
	ADC_DMACmd(ADC3, ENABLE);

	/* Enable ADC3 */
	ADC_Cmd(ADC3, ENABLE);

	/* Start ADC3 Software Conversion */
	ADC_SoftwareStartConv(ADC3);
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
