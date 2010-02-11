################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/gxt/GXTArchive.cpp \
../src/gxt/GXTPlainKeyTable.cpp \
../src/gxt/GXTTable.cpp 

C_SRCS += \
../src/gxt/utf8.c 

OBJS += \
./src/gxt/GXTArchive.o \
./src/gxt/GXTPlainKeyTable.o \
./src/gxt/GXTTable.o \
./src/gxt/utf8.o 

C_DEPS += \
./src/gxt/utf8.d 

CPP_DEPS += \
./src/gxt/GXTArchive.d \
./src/gxt/GXTPlainKeyTable.d \
./src/gxt/GXTTable.d 


# Each subdirectory must supply rules for building sources it contributes
src/gxt/%.o: ../src/gxt/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -Wconversion -fstack-protector-all -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/gxt/%.o: ../src/gxt/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


