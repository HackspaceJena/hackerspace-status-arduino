SRC:=$(shell find status -regex '.*\.cpp')
OBJ:=$(patsubst %,%.o,$(SRC))
DEP:=$(patsubst %,%.d,$(SRC))

CC?=avr-gcc

CFLAGS?= -O2 -ggdb -std=gnu++11 -fomit-frame-pointer -fmerge-all-constants\
-faggressive-loop-optimizations -finline-functions -funsafe-loop-optimizations\
-ffreestanding -Wlogical-op -Wdouble-promotion -Wformat -Winit-self\
-Wmissing-include-dirs -Wswitch-default -Wpadded -Wswitch-enum -Wall\
-Wunused -Winline -Wuninitialized -Wstrict-overflow\
-Wfloat-equal -Wstack-protector -Wundef -Wvla\
-Wshadow -Wcast-align -Wpedantic -Wextra\
-Wpointer-arith -Wwrite-strings -Wtrampolines -Wpacked\
-Wconversion -Wdate-time -Waggregate-return\
-Wvector-operation-performance\
-Wredundant-decls -Wlong-long -Wvariadic-macros\
-Wdisabled-optimization -Wmissing-declarations -Wunsafe-loop-optimizations\
-pipe -Werror -fno-exceptions -fno-rtti\
-I/usr/share/arduino/hardware/arduino/cores/arduino\
-I/usr/share/arduino/hardware/arduino/variants/micro\
-I/usr/lib/avr/include/\
-DF_CPU=16000000L -MMD -DUSB_VID=0x2341 -DUSB_PID=0x8037 -DARDUINO=105 -D__PROG_TYPES_COMPAT__ -mmcu=atmega32u4

ARDUINO?=
PROGRAMMER?=

.PHONY: all clean check install build

all: build

# TODO : find a install commandline
#install: build
#	avrdude

clean:
	rm $(OBJ) $(DEP)

check:
	$(CC) $(CFLAGS) -fsyntax-only $(SRC)

build: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -MMD -MP -o hackspace-status.sys

-include $(sort $(DEP))