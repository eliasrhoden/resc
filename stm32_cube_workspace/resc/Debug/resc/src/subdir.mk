################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../resc/src/CurrentCtrl.c \
../resc/src/ServoCtrl.c \
../resc/src/TrajGen.c \
../resc/src/rls.c 

OBJS += \
./resc/src/CurrentCtrl.o \
./resc/src/ServoCtrl.o \
./resc/src/TrajGen.o \
./resc/src/rls.o 

C_DEPS += \
./resc/src/CurrentCtrl.d \
./resc/src/ServoCtrl.d \
./resc/src/TrajGen.d \
./resc/src/rls.d 


# Each subdirectory must supply rules for building sources it contributes
resc/src/%.o resc/src/%.su: ../resc/src/%.c resc/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/elias/Min enhet/Projekt/Bonne ESC FOC/stm32_cube_workspace/resc/resc/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-resc-2f-src

clean-resc-2f-src:
	-$(RM) ./resc/src/CurrentCtrl.d ./resc/src/CurrentCtrl.o ./resc/src/CurrentCtrl.su ./resc/src/ServoCtrl.d ./resc/src/ServoCtrl.o ./resc/src/ServoCtrl.su ./resc/src/TrajGen.d ./resc/src/TrajGen.o ./resc/src/TrajGen.su ./resc/src/rls.d ./resc/src/rls.o ./resc/src/rls.su

.PHONY: clean-resc-2f-src

