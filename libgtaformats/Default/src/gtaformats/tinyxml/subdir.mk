################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/gtaformats/tinyxml/tinystr.cpp \
../src/gtaformats/tinyxml/tinyxml.cpp \
../src/gtaformats/tinyxml/tinyxmlerror.cpp \
../src/gtaformats/tinyxml/tinyxmlparser.cpp 

OBJS += \
./src/gtaformats/tinyxml/tinystr.o \
./src/gtaformats/tinyxml/tinyxml.o \
./src/gtaformats/tinyxml/tinyxmlerror.o \
./src/gtaformats/tinyxml/tinyxmlparser.o 

CPP_DEPS += \
./src/gtaformats/tinyxml/tinystr.d \
./src/gtaformats/tinyxml/tinyxml.d \
./src/gtaformats/tinyxml/tinyxmlerror.d \
./src/gtaformats/tinyxml/tinyxmlparser.d 


# Each subdirectory must supply rules for building sources it contributes
src/gtaformats/tinyxml/%.o: ../src/gtaformats/tinyxml/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


