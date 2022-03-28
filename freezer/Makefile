# Assume the source tree is where the running kernel was built
# You should set KERNELDIR in the environment if it's elsewhere
KERNELDIR ?= /lib/modules/$(shell uname -r)/build

# The current directory is passed to sub-makes as argument
PWD := $(shell pwd)

all: modules

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

.PHONY: all modules clean

obj-m := freezer_module.o
freezer_module-objs := freezer_hook.o freezer_netlink.o array.o
