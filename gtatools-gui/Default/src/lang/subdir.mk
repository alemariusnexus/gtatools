################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/lang/EnglishLanguageProvider.cpp \
../src/lang/GermanLanguageProvider.cpp \
../src/lang/LanguageProvider.cpp 

OBJS += \
./src/lang/EnglishLanguageProvider.o \
./src/lang/GermanLanguageProvider.o \
./src/lang/LanguageProvider.o 

CPP_DEPS += \
./src/lang/EnglishLanguageProvider.d \
./src/lang/GermanLanguageProvider.d \
./src/lang/LanguageProvider.d 


# Each subdirectory must supply rules for building sources it contributes
src/lang/%.o: ../src/lang/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ `wx-config --cxxflags --unicode=yes` -DUNICODE -DwxUSE_GLCANVAS=1 -DwxUSE_CMDLINE_PARSER=1 -DwxUSE_CLIPBOARD=1 -D_UNICODE -I/usr/local/include/gtaformats -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


