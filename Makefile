SRC:=$(shell find src -regex '.*\.c')
OBJ:=$(patsubst %,%.o,$(SRC))
DEP:=$(patsubst %,%.d,$(SRC))

CC?=avr-gcc

CFLAGS?= -O2 -ggdb -std=gnu11 -fomit-frame-pointer -fmerge-all-constants\
-faggressive-loop-optimizations -finline-functions -funsafe-loop-optimizations\
-ffreestanding -Wlogical-op -Wdouble-promotion -Wformat -Winit-self -Wnormalized\
-Wmissing-include-dirs -Wswitch-default -Wpadded -Wswitch-enum -Wall\
-Wunused -Winline -Wuninitialized -Wstrict-overflow -Wpointer-sign\
-Wfloat-equal -Wstack-protector -Wtraditional-conversion -Wundef -Wvla\
-Wdeclaration-after-statement -Wshadow -Wcast-align -Wpedantic -Wextra\
-Wpointer-arith -Wbad-function-cast -Wwrite-strings -Wtrampolines -Wpacked\
-Wconversion -Wdate-time -Waggregate-return -Wstrict-prototypes\
-Wold-style-definition -Wmissing-prototypes -Wvector-operation-performance\
-Wredundant-decls -Wnested-externs -Wlong-long -Wvariadic-macros\
-Wdisabled-optimization -Wmissing-declarations -Wunsafe-loop-optimizations\
-Wunsuffixed-float-constants -pipe -Werror

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