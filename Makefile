CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra
LDFLAGS ?= -rdynamic
LDLIBS += -ldl

descend += src
descend += desktop

export CC
export CFLAGS
export LDFLAGS
export LDLIBS

.PHONY: all clean $(descend)

all: $(descend)

$(descend):
	@$(MAKE) -C $@ all

clean:
	@for dir in $(descend); do \
		$(MAKE) -C $$dir clean; \
	done
