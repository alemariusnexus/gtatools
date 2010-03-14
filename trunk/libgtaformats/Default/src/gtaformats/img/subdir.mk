################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/gtaformats/img/IMGArchive.cpp \
../src/gtaformats/img/IMGException.cpp \
../src/gtaformats/img/IMGVisitor.cpp 

OBJS += \
./src/gtaformats/img/IMGArchive.o \
./src/gtaformats/img/IMGException.o \
./src/gtaformats/img/IMGVisitor.o 

CPP_DEPS += \
./src/gtaformats/img/IMGArchive.d \
./src/gtaformats/img/IMGException.d \
./src/gtaformats/img/IMGVisitor.d 


# Each subdirectory must supply rules for building sources it contributes
src/gtaformats/img/%.o: ../src/gtaformats/img/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


