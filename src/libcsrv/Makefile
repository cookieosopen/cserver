CC ?= cc
AR ?= ar

CPPFLAGS += -I../../include
CFLAGS ?= -O2 -Wall -Wextra

obj-y += $(patsubst %.c,%.o,$(shell find . -type f -name '*.c' -print))

.PHONY: all clean

all: libcsrv.a

libcsrv.a: $(obj-y)
	@echo "AR      $@"
	@$(AR) rcs $@ $(obj-y)

%.o: %.c
	@echo "CC      $<"
	@$(CC) $(CPPFLAGS) $(CFLAGS) -fPIC -c $< -o $@

clean:
	@rm -f $(obj-y) libcsrv.a
