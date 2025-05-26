################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MQTT/MQTTClient.c \
../MQTT/MQTTConnectClient.c \
../MQTT/MQTTConnectServer.c \
../MQTT/MQTTDeserializePublish.c \
../MQTT/MQTTFormat.c \
../MQTT/MQTTInterface.c \
../MQTT/MQTTPacket.c \
../MQTT/MQTTSerializePublish.c \
../MQTT/MQTTSubscribeClient.c \
../MQTT/MQTTSubscribeServer.c \
../MQTT/MQTTUnsubscribeClient.c \
../MQTT/MQTTUnsubscribeServer.c 

OBJS += \
./MQTT/MQTTClient.o \
./MQTT/MQTTConnectClient.o \
./MQTT/MQTTConnectServer.o \
./MQTT/MQTTDeserializePublish.o \
./MQTT/MQTTFormat.o \
./MQTT/MQTTInterface.o \
./MQTT/MQTTPacket.o \
./MQTT/MQTTSerializePublish.o \
./MQTT/MQTTSubscribeClient.o \
./MQTT/MQTTSubscribeServer.o \
./MQTT/MQTTUnsubscribeClient.o \
./MQTT/MQTTUnsubscribeServer.o 

C_DEPS += \
./MQTT/MQTTClient.d \
./MQTT/MQTTConnectClient.d \
./MQTT/MQTTConnectServer.d \
./MQTT/MQTTDeserializePublish.d \
./MQTT/MQTTFormat.d \
./MQTT/MQTTInterface.d \
./MQTT/MQTTPacket.d \
./MQTT/MQTTSerializePublish.d \
./MQTT/MQTTSubscribeClient.d \
./MQTT/MQTTSubscribeServer.d \
./MQTT/MQTTUnsubscribeClient.d \
./MQTT/MQTTUnsubscribeServer.d 


# Each subdirectory must supply rules for building sources it contributes
MQTT/%.o MQTT/%.su MQTT/%.cyclo: ../MQTT/%.c MQTT/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-MQTT

clean-MQTT:
	-$(RM) ./MQTT/MQTTClient.cyclo ./MQTT/MQTTClient.d ./MQTT/MQTTClient.o ./MQTT/MQTTClient.su ./MQTT/MQTTConnectClient.cyclo ./MQTT/MQTTConnectClient.d ./MQTT/MQTTConnectClient.o ./MQTT/MQTTConnectClient.su ./MQTT/MQTTConnectServer.cyclo ./MQTT/MQTTConnectServer.d ./MQTT/MQTTConnectServer.o ./MQTT/MQTTConnectServer.su ./MQTT/MQTTDeserializePublish.cyclo ./MQTT/MQTTDeserializePublish.d ./MQTT/MQTTDeserializePublish.o ./MQTT/MQTTDeserializePublish.su ./MQTT/MQTTFormat.cyclo ./MQTT/MQTTFormat.d ./MQTT/MQTTFormat.o ./MQTT/MQTTFormat.su ./MQTT/MQTTInterface.cyclo ./MQTT/MQTTInterface.d ./MQTT/MQTTInterface.o ./MQTT/MQTTInterface.su ./MQTT/MQTTPacket.cyclo ./MQTT/MQTTPacket.d ./MQTT/MQTTPacket.o ./MQTT/MQTTPacket.su ./MQTT/MQTTSerializePublish.cyclo ./MQTT/MQTTSerializePublish.d ./MQTT/MQTTSerializePublish.o ./MQTT/MQTTSerializePublish.su ./MQTT/MQTTSubscribeClient.cyclo ./MQTT/MQTTSubscribeClient.d ./MQTT/MQTTSubscribeClient.o ./MQTT/MQTTSubscribeClient.su ./MQTT/MQTTSubscribeServer.cyclo ./MQTT/MQTTSubscribeServer.d ./MQTT/MQTTSubscribeServer.o ./MQTT/MQTTSubscribeServer.su ./MQTT/MQTTUnsubscribeClient.cyclo ./MQTT/MQTTUnsubscribeClient.d ./MQTT/MQTTUnsubscribeClient.o ./MQTT/MQTTUnsubscribeClient.su ./MQTT/MQTTUnsubscribeServer.cyclo ./MQTT/MQTTUnsubscribeServer.d ./MQTT/MQTTUnsubscribeServer.o ./MQTT/MQTTUnsubscribeServer.su

.PHONY: clean-MQTT

