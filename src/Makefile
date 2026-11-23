CC ?= cc
AR ?= ar

CPPFLAGS += -I../include $(shell pkg-config --cflags libdrm)
LDLIBS += $(shell pkg-config --libs libdrm)

CFLAGS ?= -O2 -Wall -Wextra

descend += libcsrv

obj-y += $(patsubst %.c,%.o,$(shell find . -maxdepth 1 -type f -name '*.c' -print))

.PHONY: all clean $(descend)

all: $(descend) ../cserver

$(descend):
	@$(MAKE) -C $@ all

../cserver: $(obj-y)
	@echo "LD      $@"
	@$(CC) $(LDFLAGS) -o $@ $(obj-y) $(LDLIBS)

%.o: %.c
	@echo "CC      $<"
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(obj-y) ../cserver
	@for dir in $(descend); do \
		$(MAKE) -C $$dir clean; \
	done
