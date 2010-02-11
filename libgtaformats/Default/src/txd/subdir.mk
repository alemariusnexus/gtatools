################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/txd/TXDArchive.cpp \
../src/txd/TXDException.cpp \
../src/txd/TXDTexture.cpp \
../src/txd/TXDVisitor.cpp 

OBJS += \
./src/txd/TXDArchive.o \
./src/txd/TXDException.o \
./src/txd/TXDTexture.o \
./src/txd/TXDVisitor.o 

CPP_DEPS += \
./src/txd/TXDArchive.d \
./src/txd/TXDException.d \
./src/txd/TXDTexture.d \
./src/txd/TXDVisitor.d 


# Each subdirectory must supply rules for building sources it contributes
src/txd/%.o: ../src/txd/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -Wconversion -fstack-protector-all -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


