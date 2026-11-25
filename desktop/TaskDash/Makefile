CC ?= cc

CPPFLAGS += -I../../include
CFLAGS += -Wall -Wextra -O2 -fPIC

obj-y += taskdash.o
obj-y += icon.o

.PHONY: all clean

all: TaskDash.so

TaskDash.so: $(obj-y)
	@echo "LD      $@"
	@$(CC) -shared $(LDFLAGS) -o $@ $(obj-y)

%.o: %.c
	@echo "CC      $<"
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(obj-y) TaskDash.so