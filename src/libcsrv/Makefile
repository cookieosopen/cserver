CC ?= cc

CPPFLAGS += -I../../include
CFLAGS += -Wall -Wextra -O2 -fPIC

obj-y += $(patsubst %.c,%.o,$(shell find . -type f -name '*.c' -print))

.PHONY: all clean install

all: libcsrv.so

libcsrv.so: $(obj-y)
	@echo "LD      $@"
	@$(CC) -shared -Wl,-soname,libcsrv.so $(LDFLAGS) -o $@ $(obj-y) $(LDLIBS)

%.o: %.c
	@echo "CC      $<"
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(obj-y) libcsrv.a libcsrv.so
