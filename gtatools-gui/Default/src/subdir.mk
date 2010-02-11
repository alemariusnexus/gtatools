################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Application.cpp \
../src/DFFFormatProvider.cpp \
../src/DFFPanel.cpp \
../src/DataSource.cpp \
../src/DisplayManager.cpp \
../src/FileDataSource.cpp \
../src/FileDisplayer.cpp \
../src/FormatProvider.cpp \
../src/IMGFormatProvider.cpp \
../src/IMGPanel.cpp \
../src/MainFrame.cpp \
../src/TXDFormatProvider.cpp \
../src/TXDPanel.cpp \
../src/TXDTextureDisplayer.cpp \
../src/WxGUI.cpp 

OBJS += \
./src/Application.o \
./src/DFFFormatProvider.o \
./src/DFFPanel.o \
./src/DataSource.o \
./src/DisplayManager.o \
./src/FileDataSource.o \
./src/FileDisplayer.o \
./src/FormatProvider.o \
./src/IMGFormatProvider.o \
./src/IMGPanel.o \
./src/MainFrame.o \
./src/TXDFormatProvider.o \
./src/TXDPanel.o \
./src/TXDTextureDisplayer.o \
./src/WxGUI.o 

CPP_DEPS += \
./src/Application.d \
./src/DFFFormatProvider.d \
./src/DFFPanel.d \
./src/DataSource.d \
./src/DisplayManager.d \
./src/FileDataSource.d \
./src/FileDisplayer.d \
./src/FormatProvider.d \
./src/IMGFormatProvider.d \
./src/IMGPanel.d \
./src/MainFrame.d \
./src/TXDFormatProvider.d \
./src/TXDPanel.d \
./src/TXDTextureDisplayer.d \
./src/WxGUI.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ `wx-config --cxxflags --unicode=yes` -DIL_NO_UNICODE -I/usr/local/include/gtaformats -I/usr/local/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


