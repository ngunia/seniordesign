################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/src/DEVICE/misc.c \
../system/src/DEVICE/stm32f4xx_adc.c \
../system/src/DEVICE/stm32f4xx_dac.c \
../system/src/DEVICE/stm32f4xx_dma.c \
../system/src/DEVICE/stm32f4xx_exti.c \
../system/src/DEVICE/stm32f4xx_gpio.c \
../system/src/DEVICE/stm32f4xx_pwr.c \
../system/src/DEVICE/stm32f4xx_rcc.c \
../system/src/DEVICE/stm32f4xx_rtc.c \
../system/src/DEVICE/stm32f4xx_syscfg.c \
../system/src/DEVICE/stm32f4xx_tim.c 

OBJS += \
./system/src/DEVICE/misc.o \
./system/src/DEVICE/stm32f4xx_adc.o \
./system/src/DEVICE/stm32f4xx_dac.o \
./system/src/DEVICE/stm32f4xx_dma.o \
./system/src/DEVICE/stm32f4xx_exti.o \
./system/src/DEVICE/stm32f4xx_gpio.o \
./system/src/DEVICE/stm32f4xx_pwr.o \
./system/src/DEVICE/stm32f4xx_rcc.o \
./system/src/DEVICE/stm32f4xx_rtc.o \
./system/src/DEVICE/stm32f4xx_syscfg.o \
./system/src/DEVICE/stm32f4xx_tim.o 

C_DEPS += \
./system/src/DEVICE/misc.d \
./system/src/DEVICE/stm32f4xx_adc.d \
./system/src/DEVICE/stm32f4xx_dac.d \
./system/src/DEVICE/stm32f4xx_dma.d \
./system/src/DEVICE/stm32f4xx_exti.d \
./system/src/DEVICE/stm32f4xx_gpio.d \
./system/src/DEVICE/stm32f4xx_pwr.d \
./system/src/DEVICE/stm32f4xx_rcc.d \
./system/src/DEVICE/stm32f4xx_rtc.d \
./system/src/DEVICE/stm32f4xx_syscfg.d \
./system/src/DEVICE/stm32f4xx_tim.d 


# Each subdirectory must supply rules for building sources it contributes
system/src/DEVICE/%.o: ../system/src/DEVICE/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -Wall -Wextra  -g -DOS_USE_TRACE_ITM -I"../include" -I"../system/include" -I"../system/include/CMSIS" -I"../system/include/DEVICE" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


