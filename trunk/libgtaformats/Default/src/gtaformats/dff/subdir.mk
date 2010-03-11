################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/gtaformats/dff/DFFFrame.cpp \
../src/gtaformats/dff/DFFGeometry.cpp \
../src/gtaformats/dff/DFFGeometryPart.cpp \
../src/gtaformats/dff/DFFLoader.cpp \
../src/gtaformats/dff/DFFMaterial.cpp \
../src/gtaformats/dff/DFFMesh.cpp \
../src/gtaformats/dff/DFFTexture.cpp \
../src/gtaformats/dff/DFFXmlConverter.cpp 

OBJS += \
./src/gtaformats/dff/DFFFrame.o \
./src/gtaformats/dff/DFFGeometry.o \
./src/gtaformats/dff/DFFGeometryPart.o \
./src/gtaformats/dff/DFFLoader.o \
./src/gtaformats/dff/DFFMaterial.o \
./src/gtaformats/dff/DFFMesh.o \
./src/gtaformats/dff/DFFTexture.o \
./src/gtaformats/dff/DFFXmlConverter.o 

CPP_DEPS += \
./src/gtaformats/dff/DFFFrame.d \
./src/gtaformats/dff/DFFGeometry.d \
./src/gtaformats/dff/DFFGeometryPart.d \
./src/gtaformats/dff/DFFLoader.d \
./src/gtaformats/dff/DFFMaterial.d \
./src/gtaformats/dff/DFFMesh.d \
./src/gtaformats/dff/DFFTexture.d \
./src/gtaformats/dff/DFFXmlConverter.d 


# Each subdirectory must supply rules for building sources it contributes
src/gtaformats/dff/%.o: ../src/gtaformats/dff/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


