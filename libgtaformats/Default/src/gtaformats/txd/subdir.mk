################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/gtaformats/txd/TXDArchive.cpp \
../src/gtaformats/txd/TXDException.cpp \
../src/gtaformats/txd/TXDTexture.cpp \
../src/gtaformats/txd/TXDVisitor.cpp 

OBJS += \
./src/gtaformats/txd/TXDArchive.o \
./src/gtaformats/txd/TXDException.o \
./src/gtaformats/txd/TXDTexture.o \
./src/gtaformats/txd/TXDVisitor.o 

CPP_DEPS += \
./src/gtaformats/txd/TXDArchive.d \
./src/gtaformats/txd/TXDException.d \
./src/gtaformats/txd/TXDTexture.d \
./src/gtaformats/txd/TXDVisitor.d 


# Each subdirectory must supply rules for building sources it contributes
src/gtaformats/txd/%.o: ../src/gtaformats/txd/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


