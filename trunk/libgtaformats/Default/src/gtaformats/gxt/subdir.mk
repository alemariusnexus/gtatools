################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/gtaformats/gxt/GXTArchive.cpp \
../src/gtaformats/gxt/GXTPlainKeyTable.cpp \
../src/gtaformats/gxt/GXTTable.cpp 

OBJS += \
./src/gtaformats/gxt/GXTArchive.o \
./src/gtaformats/gxt/GXTPlainKeyTable.o \
./src/gtaformats/gxt/GXTTable.o 

CPP_DEPS += \
./src/gtaformats/gxt/GXTArchive.d \
./src/gtaformats/gxt/GXTPlainKeyTable.d \
./src/gtaformats/gxt/GXTTable.d 


# Each subdirectory must supply rules for building sources it contributes
src/gtaformats/gxt/%.o: ../src/gtaformats/gxt/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


