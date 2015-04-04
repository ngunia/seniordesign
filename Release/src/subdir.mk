################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/main.c \
../src/tm_stm32f4_delay.c \
../src/tm_stm32f4_hd44780.c 

OBJS += \
./src/main.o \
./src/tm_stm32f4_delay.o \
./src/tm_stm32f4_hd44780.o 

C_DEPS += \
./src/main.d \
./src/tm_stm32f4_delay.d \
./src/tm_stm32f4_hd44780.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -Wall -Wextra  -g -DOS_USE_TRACE_ITM -I"../include" -I"../system/include" -I"../system/include/CMSIS" -I"../system/include/DEVICE" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


