################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
ASM_SRCS += \
../include/s3c44b0x.asm \
../include/s3cev40.asm 

OBJS += \
./include/s3c44b0x.o \
./include/s3cev40.o 

ASM_DEPS += \
./include/s3c44b0x.d \
./include/s3cev40.d 


# Each subdirectory must supply rules for building sources it contributes
include/%.o: ../include/%.asm
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC Assembler'
	arm-none-eabi-gcc -x assembler-with-cpp -I..\..\BSP\include -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


