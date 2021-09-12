C_SRC := blinker_devmem_test.c
CFLAGS := -g -O0 -Werror -Wall -I${SOCEDS_DEST_ROOT}/ip/altera/hps/altera_hps/hwlib/include a-I${SOCEDS_DEST_ROOT}/ip/altera/hps/altera_hps/hwlib/include/soc_cv_av -D soc_cv_av

# using Buildroot toolchain
PATH=$PATH:/home/nicolas/Documents/buildroot/output/host/usr/bin
CROSS_COMPILE := arm-buildroot-linux-gnueabihf-
CC := $(CROSS_COMPILE)gcc
NM := $(CROSS_COMPILE)nm
ifeq ($(or $(COMSPEC),$(ComSpec)),)
RM := rm -rf
else
RM := cs-rm -rf
endif

ELF ?= $(basename $(firstword $(C_SRC)))
OBJ := $(patsubst %.c,%.o,$(C_SRC))

.PHONY: all
all: $(ELF)

.PHONY:
clean:
		$(RM) $(ELF) $(OBJ) *.objdump *.map

$(OBJ): %.o: %.c
		$(CC) $(CFLAGS) -c $< -o $@

$(ELF): $(OBJ)
		$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)
		$(NM) $@ > $@.map