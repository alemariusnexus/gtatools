################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/gtaformats/gf_filetype.cpp \
../src/gtaformats/gta.cpp 

OBJS += \
./src/gtaformats/gf_filetype.o \
./src/gtaformats/gta.o 

CPP_DEPS += \
./src/gtaformats/gf_filetype.d \
./src/gtaformats/gta.d 


# Each subdirectory must supply rules for building sources it contributes
src/gtaformats/%.o: ../src/gtaformats/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


