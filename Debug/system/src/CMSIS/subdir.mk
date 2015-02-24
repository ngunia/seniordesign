################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/src/CMSIS/system_stm32f4xx.c \
../system/src/CMSIS/vectors_stm32f4xx.c 

OBJS += \
./system/src/CMSIS/system_stm32f4xx.o \
./system/src/CMSIS/vectors_stm32f4xx.o 

C_DEPS += \
./system/src/CMSIS/system_stm32f4xx.d \
./system/src/CMSIS/vectors_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
system/src/CMSIS/%.o: ../system/src/CMSIS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -Wall -Wextra  -g3 -DUSE_FULL_ASSERT -DOS_USE_TRACE_ITM -I"../include" -I"../system/include" -I"../system/include/CMSIS" -I"../system/include/DEVICE" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


