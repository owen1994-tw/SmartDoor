################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/OLED/OLED.c \
../Drivers/BSP/OLED/OLED_Data.c 

OBJS += \
./Drivers/BSP/OLED/OLED.o \
./Drivers/BSP/OLED/OLED_Data.o 

C_DEPS += \
./Drivers/BSP/OLED/OLED.d \
./Drivers/BSP/OLED/OLED_Data.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/OLED/%.o Drivers/BSP/OLED/%.su: ../Drivers/BSP/OLED/%.c Drivers/BSP/OLED/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/owenw/STM32Cube/FinalProject/SmartDoor/FreeRTOS/include" -I"C:/Users/owenw/STM32Cube/FinalProject/SmartDoor/FreeRTOS/portable/ARM_CM4F" -I"C:/Users/owenw/STM32Cube/FinalProject/SmartDoor/Drivers/BSP" -I"C:/Users/owenw/STM32Cube/FinalProject/SmartDoor/Tasks" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-OLED

clean-Drivers-2f-BSP-2f-OLED:
	-$(RM) ./Drivers/BSP/OLED/OLED.d ./Drivers/BSP/OLED/OLED.o ./Drivers/BSP/OLED/OLED.su ./Drivers/BSP/OLED/OLED_Data.d ./Drivers/BSP/OLED/OLED_Data.o ./Drivers/BSP/OLED/OLED_Data.su

.PHONY: clean-Drivers-2f-BSP-2f-OLED

