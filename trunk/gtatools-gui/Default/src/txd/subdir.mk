################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/txd/TXDBitmapBuilder.cpp \
../src/txd/TXDFormatProvider.cpp \
../src/txd/TXDPanel.cpp \
../src/txd/TXDTextureDisplayer.cpp 

OBJS += \
./src/txd/TXDBitmapBuilder.o \
./src/txd/TXDFormatProvider.o \
./src/txd/TXDPanel.o \
./src/txd/TXDTextureDisplayer.o 

CPP_DEPS += \
./src/txd/TXDBitmapBuilder.d \
./src/txd/TXDFormatProvider.d \
./src/txd/TXDPanel.d \
./src/txd/TXDTextureDisplayer.d 


# Each subdirectory must supply rules for building sources it contributes
src/txd/%.o: ../src/txd/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ `wx-config --cxxflags --unicode=yes` -DUNICODE -DwxUSE_GLCANVAS=1 -DwxUSE_CMDLINE_PARSER=1 -DwxUSE_CLIPBOARD=1 -D_UNICODE -I/usr/local/include/gtaformats -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


