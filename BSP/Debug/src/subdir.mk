################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/adc.c \
../src/at24C04.c \
../src/dma.c \
../src/font8x16.c \
../src/iic.c \
../src/iis.c \
../src/keypad.c \
../src/l3.c \
../src/lcd.c \
../src/leds.c \
../src/pbs.c \
../src/rtc.c \
../src/segs.c \
../src/system.c \
../src/timers.c \
../src/ts.c \
../src/uart.c \
../src/uda1341ts.c 

OBJS += \
./src/adc.o \
./src/at24C04.o \
./src/dma.o \
./src/font8x16.o \
./src/iic.o \
./src/iis.o \
./src/keypad.o \
./src/l3.o \
./src/lcd.o \
./src/leds.o \
./src/pbs.o \
./src/rtc.o \
./src/segs.o \
./src/system.o \
./src/timers.o \
./src/ts.o \
./src/uart.o \
./src/uda1341ts.o 

C_DEPS += \
./src/adc.d \
./src/at24C04.d \
./src/dma.d \
./src/font8x16.d \
./src/iic.d \
./src/iis.d \
./src/keypad.d \
./src/l3.d \
./src/lcd.d \
./src/leds.d \
./src/pbs.d \
./src/rtc.d \
./src/segs.d \
./src/system.d \
./src/timers.d \
./src/ts.d \
./src/uart.d \
./src/uda1341ts.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC C Compiler'
	arm-none-eabi-gcc -I..\..\BSP\include -O0 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -mapcs-frame -fno-builtin -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


