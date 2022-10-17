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
lab3.hex \

SECONDARY_LIST += \
lab3.lst \

SECONDARY_SIZE += \
lab3.siz \


# All Target
all: lab3.elf secondary-outputs

# Tool invocations
lab3.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Sourcery Windows GCC C Linker'
	arm-none-eabi-gcc -T"../lab3.ld" -nostartfiles -L"../../BSP/Debug" -Wl,-Map,lab3.map -mcpu=arm7tdmi -g3 -gdwarf-2 -o "lab3.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

lab3.hex: lab3.elf
	@echo 'Invoking: ARM Sourcery Windows GNU Create Flash Image'
	arm-none-eabi-objcopy -O binary lab3.elf  "lab3.hex"
	@echo 'Finished building: $@'
	@echo ' '

lab3.lst: lab3.elf
	@echo 'Invoking: ARM Sourcery Windows GNU Create Listing'
	arm-none-eabi-objdump -h -S lab3.elf > "lab3.lst"
	@echo 'Finished building: $@'
	@echo ' '

lab3.siz: lab3.elf
	@echo 'Invoking: ARM Sourcery Windows GNU Print Size'
	arm-none-eabi-size  --format=berkeley lab3.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SECONDARY_SIZE)$(OBJS)$(C_DEPS)$(ASM_DEPS)$(SECONDARY_FLASH)$(EXECUTABLES)$(SECONDARY_LIST)$(S_UPPER_DEPS) lab3.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
