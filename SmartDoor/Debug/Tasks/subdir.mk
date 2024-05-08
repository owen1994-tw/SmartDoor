################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Tasks/DoorCtrl.c \
../Tasks/FingerPrint.c \
../Tasks/RFID.c 

OBJS += \
./Tasks/DoorCtrl.o \
./Tasks/FingerPrint.o \
./Tasks/RFID.o 

C_DEPS += \
./Tasks/DoorCtrl.d \
./Tasks/FingerPrint.d \
./Tasks/RFID.d 


# Each subdirectory must supply rules for building sources it contributes
Tasks/%.o Tasks/%.su: ../Tasks/%.c Tasks/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/owenw/STM32Cube/FinalProject/SmartDoor/FreeRTOS/include" -I"C:/Users/owenw/STM32Cube/FinalProject/SmartDoor/FreeRTOS/portable/ARM_CM4F" -I"C:/Users/owenw/STM32Cube/FinalProject/SmartDoor/Drivers/BSP" -I"C:/Users/owenw/STM32Cube/FinalProject/SmartDoor/Tasks" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Tasks

clean-Tasks:
	-$(RM) ./Tasks/DoorCtrl.d ./Tasks/DoorCtrl.o ./Tasks/DoorCtrl.su ./Tasks/FingerPrint.d ./Tasks/FingerPrint.o ./Tasks/FingerPrint.su ./Tasks/RFID.d ./Tasks/RFID.o ./Tasks/RFID.su

.PHONY: clean-Tasks

