################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/gtaformats/ide/IDEEntity.cpp \
../src/gtaformats/ide/IDEReader.cpp \
../src/gtaformats/ide/IDEStaticObject.cpp 

OBJS += \
./src/gtaformats/ide/IDEEntity.o \
./src/gtaformats/ide/IDEReader.o \
./src/gtaformats/ide/IDEStaticObject.o 

CPP_DEPS += \
./src/gtaformats/ide/IDEEntity.d \
./src/gtaformats/ide/IDEReader.d \
./src/gtaformats/ide/IDEStaticObject.d 


# Each subdirectory must supply rules for building sources it contributes
src/gtaformats/ide/%.o: ../src/gtaformats/ide/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


