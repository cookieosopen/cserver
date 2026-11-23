CPPFLAGS += -I../include

obj-y += $(patsubst %.c,%.o,$(shell find . -type f -name '*.c' -print))

.PHONY: all clean

all: ../cserver

../cserver: $(obj-y)
	@echo "LD      $@"
	@$(CC) $(LDFLAGS) -o $@ $(obj-y)

%.o: %.c
	@echo "CC      $<"
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(obj-y) ../cserver
