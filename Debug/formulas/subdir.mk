################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../formulas/AllTomorrow.cpp \
../formulas/AllUntil.cpp \
../formulas/Atom.cpp \
../formulas/Conjunction.cpp \
../formulas/ExistsTomorrow.cpp \
../formulas/ExistsUntil.cpp \
../formulas/Formula.cpp \
../formulas/Negation.cpp 

OBJS += \
./formulas/AllTomorrow.o \
./formulas/AllUntil.o \
./formulas/Atom.o \
./formulas/Conjunction.o \
./formulas/ExistsTomorrow.o \
./formulas/ExistsUntil.o \
./formulas/Formula.o \
./formulas/Negation.o 

CPP_DEPS += \
./formulas/AllTomorrow.d \
./formulas/AllUntil.d \
./formulas/Atom.d \
./formulas/Conjunction.d \
./formulas/ExistsTomorrow.d \
./formulas/ExistsUntil.d \
./formulas/Formula.d \
./formulas/Negation.d 


# Each subdirectory must supply rules for building sources it contributes
formulas/%.o: ../formulas/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


