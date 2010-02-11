################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/img/IMGArchive.cpp \
../src/img/IMGException.cpp \
../src/img/IMGVisitor.cpp 

OBJS += \
./src/img/IMGArchive.o \
./src/img/IMGException.o \
./src/img/IMGVisitor.o 

CPP_DEPS += \
./src/img/IMGArchive.d \
./src/img/IMGException.d \
./src/img/IMGVisitor.d 


# Each subdirectory must supply rules for building sources it contributes
src/img/%.o: ../src/img/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -Wconversion -fstack-protector-all -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


