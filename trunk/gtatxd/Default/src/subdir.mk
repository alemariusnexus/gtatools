################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ExtractVisitor.cpp \
../src/ListVisitor.cpp \
../src/cliarg.cpp \
../src/main.cpp 

OBJS += \
./src/ExtractVisitor.o \
./src/ListVisitor.o \
./src/cliarg.o \
./src/main.o 

CPP_DEPS += \
./src/ExtractVisitor.d \
./src/ListVisitor.d \
./src/cliarg.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include/gtaformats -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


