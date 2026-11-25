CC ?= cc

CPPFLAGS += -I../../include
CFLAGS += -Wall -Wextra -O2

LDFLAGS +=
LDLIBS += -lcsrv

obj-y += cterm.o

.PHONY: all clean

all: cterm

cterm: $(obj-y)
	@echo "LD      $@"
	@$(CC) $(LDFLAGS) -o $@ $(obj-y) $(LDLIBS)

%.o: %.c
	@echo "CC      $<"
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(obj-y) cterm
