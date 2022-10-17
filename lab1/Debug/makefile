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
lab1.hex \

SECONDARY_LIST += \
lab1.lst \

SECONDARY_SIZE += \
lab1.siz \


# All Target
all: lab1.elf secondary-outputs

# Tool invocations
lab1.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Sourcery Windows GCC C Linker'
	arm-none-eabi-gcc -T"../lab1.ld" -nostartfiles -Wl,-Map,lab1.map -mcpu=arm7tdmi -g3 -gdwarf-2 -o "lab1.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

lab1.hex: lab1.elf
	@echo 'Invoking: ARM Sourcery Windows GNU Create Flash Image'
	arm-none-eabi-objcopy -O binary lab1.elf  "lab1.hex"
	@echo 'Finished building: $@'
	@echo ' '

lab1.lst: lab1.elf
	@echo 'Invoking: ARM Sourcery Windows GNU Create Listing'
	arm-none-eabi-objdump -h -S lab1.elf > "lab1.lst"
	@echo 'Finished building: $@'
	@echo ' '

lab1.siz: lab1.elf
	@echo 'Invoking: ARM Sourcery Windows GNU Print Size'
	arm-none-eabi-size  --format=berkeley lab1.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SECONDARY_SIZE)$(OBJS)$(C_DEPS)$(ASM_DEPS)$(SECONDARY_FLASH)$(EXECUTABLES)$(SECONDARY_LIST)$(S_UPPER_DEPS) lab1.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
