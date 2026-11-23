CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -static
LDFLAGS ?=

descend += src
descend += desktop

export CC
export CFLAGS
export LDFLAGS

.PHONY: all clean $(descend)

all: $(descend)

$(descend):
	@$(MAKE) -C $@ all

clean:
	@for dir in $(descend); do \
		$(MAKE) -C $$dir clean; \
	done
