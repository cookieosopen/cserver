CC ?= cc

CPPFLAGS += -I../../include
CFLAGS += -Wall -Wextra -O2 -fPIC

obj-y += mouse.o

.PHONY: all clean

all: DesktopMouse.so

DesktopMouse.so: $(obj-y)
	@echo "LD      $@"
	@$(CC) -shared $(LDFLAGS) -o $@ $(obj-y)

%.o: %.c
	@echo "CC      $<"
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(obj-y) DesktopMouse.so
