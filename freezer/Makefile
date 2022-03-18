obj-m := freezer_module.o
freezer_module-objs := freezer_hook.o freezer_netlink.o

all:
	make -C /lib/modules/$$ENV_KERNEL_CUSTOM/build M=$(PWD) modules
clean:
	make -C /lib/modules/$$ENV_KERNEL_CUSTOM/build M=$(PWD) clean
