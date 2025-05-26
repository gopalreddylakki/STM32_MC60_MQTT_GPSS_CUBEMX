################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Systick/systick.c 

OBJS += \
./Systick/systick.o 

C_DEPS += \
./Systick/systick.d 


# Each subdirectory must supply rules for building sources it contributes
Systick/%.o Systick/%.su Systick/%.cyclo: ../Systick/%.c Systick/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Systick

clean-Systick:
	-$(RM) ./Systick/systick.cyclo ./Systick/systick.d ./Systick/systick.o ./Systick/systick.su

.PHONY: clean-Systick

