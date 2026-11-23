CC ?= cc

CPPFLAGS += -I../../include
CFLAGS += -Wall -Wextra -O2 -fPIC

obj-y += hello.o

.PHONY: all clean

all: HelloDesktop.so

HelloDesktop.so: $(obj-y)
	@echo "LD      $@"
	@$(CC) -shared $(LDFLAGS) -o $@ $(obj-y)

%.o: %.c
	@echo "CC      $<"
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(obj-y) HelloDesktop.so
