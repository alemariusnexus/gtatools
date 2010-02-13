################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/dff/DFFException.cpp \
../src/dff/DFFFrame.cpp \
../src/dff/DFFGeometry.cpp \
../src/dff/DFFGeometryPart.cpp \
../src/dff/DFFMaterial.cpp \
../src/dff/DFFTexture.cpp \
../src/dff/DffLoader.cpp \
../src/dff/DffMesh.cpp 

OBJS += \
./src/dff/DFFException.o \
./src/dff/DFFFrame.o \
./src/dff/DFFGeometry.o \
./src/dff/DFFGeometryPart.o \
./src/dff/DFFMaterial.o \
./src/dff/DFFTexture.o \
./src/dff/DffLoader.o \
./src/dff/DffMesh.o 

CPP_DEPS += \
./src/dff/DFFException.d \
./src/dff/DFFFrame.d \
./src/dff/DFFGeometry.d \
./src/dff/DFFGeometryPart.d \
./src/dff/DFFMaterial.d \
./src/dff/DFFTexture.d \
./src/dff/DffLoader.d \
./src/dff/DffMesh.d 


# Each subdirectory must supply rules for building sources it contributes
src/dff/%.o: ../src/dff/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -Wconversion -fstack-protector-all -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


