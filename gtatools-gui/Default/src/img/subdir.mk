################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/img/IMGFormatProvider.cpp \
../src/img/IMGPanel.cpp 

OBJS += \
./src/img/IMGFormatProvider.o \
./src/img/IMGPanel.o 

CPP_DEPS += \
./src/img/IMGFormatProvider.d \
./src/img/IMGPanel.d 


# Each subdirectory must supply rules for building sources it contributes
src/img/%.o: ../src/img/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ `wx-config --cxxflags --unicode=yes` -DUNICODE -DwxUSE_GLCANVAS=1 -DwxUSE_CMDLINE_PARSER=1 -DwxUSE_CLIPBOARD=1 -D_UNICODE -I/usr/local/include/gtaformats -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


