CC ?= cc

descend += $(patsubst %/,%,$(wildcard */))

.PHONY: all clean $(descend)

all: $(descend)

$(descend):
	@$(MAKE) -C $@ all

clean:
	@for dir in $(descend); do \
		$(MAKE) -C $$dir clean; \
	done
