################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/AboutFrame.cpp \
../src/Application.cpp \
../src/DataSource.cpp \
../src/DisplayManager.cpp \
../src/FileDataSource.cpp \
../src/FileDisplayer.cpp \
../src/FormatProvider.cpp \
../src/MainFrame.cpp \
../src/WxGUI.cpp 

OBJS += \
./src/AboutFrame.o \
./src/Application.o \
./src/DataSource.o \
./src/DisplayManager.o \
./src/FileDataSource.o \
./src/FileDisplayer.o \
./src/FormatProvider.o \
./src/MainFrame.o \
./src/WxGUI.o 

CPP_DEPS += \
./src/AboutFrame.d \
./src/Application.d \
./src/DataSource.d \
./src/DisplayManager.d \
./src/FileDataSource.d \
./src/FileDisplayer.d \
./src/FormatProvider.d \
./src/MainFrame.d \
./src/WxGUI.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ `wx-config --cxxflags --unicode=yes` -DUNICODE -DwxUSE_GLCANVAS=1 -DwxUSE_CMDLINE_PARSER=1 -DwxUSE_CLIPBOARD=1 -D_UNICODE -I/usr/local/include/gtaformats -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


