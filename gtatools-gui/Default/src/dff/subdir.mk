################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/dff/DFFFormatProvider.cpp \
../src/dff/DFFPanel.cpp \
../src/dff/DFFRenderer.cpp 

OBJS += \
./src/dff/DFFFormatProvider.o \
./src/dff/DFFPanel.o \
./src/dff/DFFRenderer.o 

CPP_DEPS += \
./src/dff/DFFFormatProvider.d \
./src/dff/DFFPanel.d \
./src/dff/DFFRenderer.d 


# Each subdirectory must supply rules for building sources it contributes
src/dff/%.o: ../src/dff/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ `wx-config --cxxflags --unicode=yes` -DUNICODE -DwxUSE_GLCANVAS=1 -DwxUSE_CMDLINE_PARSER=1 -DwxUSE_CLIPBOARD=1 -D_UNICODE -I/usr/local/include/gtaformats -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


