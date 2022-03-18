obj-m := freezer_module.o
freezer_module-objs := freezer_hook.o freezer_netlink.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
