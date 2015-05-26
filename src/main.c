// Drexel University ECE Senior Design Team 6: Programmable Digital Audio Effect Processor
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
#include "lut.h"

//Function prototypes
// peripherals
void RCCInit(void);
void GPIOInit(void);
void TimerInit(void);
void EnableADCInterrupt(void);
void ADCInit(void);
void DACInit(void);
// LCD
void init_pots(void);
void num2Str(char*, uint16_t);
void display_pots(void);
// effects
uint16_t delay(uint16_t);
uint16_t overdrive(uint16_t);

/* for delay effect test*/
#define BUFF_SIZE 60000
uint16_t delayBuff[BUFF_SIZE];

// holds pot values
uint16_t pot[5] = {0,0,0,0,0};

int main (int argc, char* argv[])
{
	// Initialization
	RCCInit();
	//TM_HD44780_Init(16, 2);
	EnableADCInterrupt();
	GPIOInit();
	TimerInit();
	ADCInit();
	DACInit();
	//init_pots();

	// TODO: move this somewhere more appropriate, make names configurable
	/*
	TM_HD44780_Puts(0, 0, "AAA:");
	TM_HD44780_Puts(4, 0, "BBB:");
	TM_HD44780_Puts(8, 0, "CCC:");
	TM_HD44780_Puts(12, 0, "DDD:");
	*/

	while (1)
	{
		//display_pots();
	}
	// Infinite loop, never return.
}


void display_pots(void) {
	// TODO add one for expression pedal..
	// TODO stabilize the values
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
	GPIO_init_structure.GPIO_Pin =  GPIO_Pin_6;// A6
	GPIO_init_structure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_init_structure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_init_structure);
}

//Function for setting up the timer
void TimerInit(void)
{
	TIM_TimeBaseInitTypeDef TIM3_TimeBase;
	TIM_TimeBaseStructInit(&TIM3_TimeBase);

	TIM3_TimeBase.TIM_Period        = (uint16_t)15;
	TIM3_TimeBase.TIM_Prescaler     = 3;
	TIM3_TimeBase.TIM_ClockDivision = 0;
	TIM3_TimeBase.TIM_CounterMode   = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM3_TimeBase);
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);

	// Set up TIM3 interrupt
	NVIC_InitTypeDef nvicStructure;
	nvicStructure.NVIC_IRQChannel = TIM3_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
	nvicStructure.NVIC_IRQChannelSubPriority = 0;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStructure);

	TIM_Cmd(TIM3, ENABLE);
}

//Function to enable interrupts for the timer
void EnableADCInterrupt(void)
{
	// enable all ADCS to be triggered
    NVIC_InitTypeDef nvicStructure;
    nvicStructure.NVIC_IRQChannel    = ADC_IRQn;
    nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStructure);
}

// interrupt where ADC read, DAC write occurs
void ADC_IRQHandler(void)
{
  //DAC_SetChannel1Data(DAC_Align_12b_R, delay( ADC_GetConversionValue(ADC1) ) );
  DAC_SetChannel1Data(DAC_Align_12b_R, overdrive( ADC_GetConversionValue(ADC1) ) );
  //DAC_SetChannel1Data(DAC_Align_12b_R, ADC_GetConversionValue(ADC1) ); // pipe through
  ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
}


uint16_t delay(uint16_t currSamp)
{
    const float feedback_degree = 0.7f; //how much of the previous do you want to keep?
    const float mix = 0.5f;
	static uint16_t buffInd = 0;

    //calculate value
    float prev = (float)delayBuff[buffInd];
    float curr = (float)currSamp;
	delayBuff[buffInd] = (uint16_t)((feedback_degree * prev) + ((1-feedback_degree) * curr));

    uint16_t ret = (uint16_t)((mix * delayBuff[buffInd]) + ((1-mix) * curr));

    buffInd = (buffInd+1) % BUFF_SIZE; //move to next buffer spot

    return ret;
}


uint16_t overdrive(uint16_t currSamp) {
	return (2*overdrive_lut[currSamp]/3) + (1*currSamp/3) ;
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
	ADC_INIT.ADC_ExternalTrigConv     = ADC_ExternalTrigConv_T3_TRGO;
	ADC_INIT.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
	ADC_INIT.ADC_DataAlign            = ADC_DataAlign_Right;
	ADC_INIT.ADC_NbrOfConversion      = 1;
	ADC_Init(ADC1, &ADC_INIT);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_28Cycles);
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
}

//Function for initializing the DAC
void DACInit(void)
{
	DAC_InitTypeDef DAC_init_structure;

	DAC_init_structure.DAC_Trigger = DAC_Trigger_None;
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
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Configure GPIO pins */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;// PC0
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;//The pins are configured in analog mode
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;//We don't need any pull up or pull down
	GPIO_Init(GPIOC, &GPIO_InitStruct);//Initialize GPIOC pins with the configuration

	/* ADC Common Init */
	ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStruct);

	/* ADC3 Init */
	ADC_DeInit();
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;//Input voltage is converted into a 12bit int (max 4095)
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;//The scan is configured in multiple channels
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;//Continuous conversion: input signal is sampled more than once
	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStruct.ADC_ExternalTrigConv = 0;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//Data converted will be shifted to right
	ADC_InitStruct.ADC_NbrOfConversion = 5;
	ADC_Init(ADC3, &ADC_InitStruct);//Initialize ADC with the configuration

	/* Select the channels to be read from */
	ADC_RegularChannelConfig(ADC3, ADC_Channel_10, 1, ADC_SampleTime_144Cycles);//PC0

	/* Enable ADC3 */
	ADC_Cmd(ADC3, ENABLE);

}


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
