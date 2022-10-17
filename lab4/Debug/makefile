################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := cs-rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
lab4.hex \

SECONDARY_LIST += \
lab4.lst \

SECONDARY_SIZE += \
lab4.siz \


# All Target
all: lab4.elf secondary-outputs

# Tool invocations
lab4.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Sourcery Windows GCC C Linker'
	arm-none-eabi-gcc -T"../lab4.ld" -nostartfiles -L../../BSP/Debug -Wl,-Map,lab4.map -mcpu=arm7tdmi -g3 -gdwarf-2 -o "lab4.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

lab4.hex: lab4.elf
	@echo 'Invoking: ARM Sourcery Windows GNU Create Flash Image'
	arm-none-eabi-objcopy -O binary lab4.elf  "lab4.hex"
	@echo 'Finished building: $@'
	@echo ' '

lab4.lst: lab4.elf
	@echo 'Invoking: ARM Sourcery Windows GNU Create Listing'
	arm-none-eabi-objdump -h -S lab4.elf > "lab4.lst"
	@echo 'Finished building: $@'
	@echo ' '

lab4.siz: lab4.elf
	@echo 'Invoking: ARM Sourcery Windows GNU Print Size'
	arm-none-eabi-size  --format=berkeley lab4.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SECONDARY_SIZE)$(OBJS)$(C_DEPS)$(ASM_DEPS)$(SECONDARY_FLASH)$(EXECUTABLES)$(SECONDARY_LIST)$(S_UPPER_DEPS) lab4.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
