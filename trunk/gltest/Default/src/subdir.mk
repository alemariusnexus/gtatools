################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/TextureProvider.cpp \
../src/VehicleRenderer.cpp \
../src/main.cpp 

OBJS += \
./src/TextureProvider.o \
./src/VehicleRenderer.o \
./src/main.o 

CPP_DEPS += \
./src/TextureProvider.d \
./src/VehicleRenderer.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


