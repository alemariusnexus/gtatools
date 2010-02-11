################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/dff/DFFException.cpp \
../src/dff/DffLoader.cpp \
../src/dff/DffMesh.cpp 

OBJS += \
./src/dff/DFFException.o \
./src/dff/DffLoader.o \
./src/dff/DffMesh.o 

CPP_DEPS += \
./src/dff/DFFException.d \
./src/dff/DffLoader.d \
./src/dff/DffMesh.d 


# Each subdirectory must supply rules for building sources it contributes
src/dff/%.o: ../src/dff/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -Wconversion -fstack-protector-all -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


