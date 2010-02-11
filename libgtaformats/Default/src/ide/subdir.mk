################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ide/IDEEntity.cpp \
../src/ide/IDEReader.cpp \
../src/ide/IDEStaticObject.cpp 

OBJS += \
./src/ide/IDEEntity.o \
./src/ide/IDEReader.o \
./src/ide/IDEStaticObject.o 

CPP_DEPS += \
./src/ide/IDEEntity.d \
./src/ide/IDEReader.d \
./src/ide/IDEStaticObject.d 


# Each subdirectory must supply rules for building sources it contributes
src/ide/%.o: ../src/ide/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -Wconversion -fstack-protector-all -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


