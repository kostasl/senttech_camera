ifneq ($(KERNELRELEASE),)
	obj-m := stcam_dd.o
	stcam_dd-objs := src/stcam_dd_api.o src/stcam_dd_cam.o src/stcam_dd_func.o src/stcam_dd_mem.o src/stcam_dd_set.o src/stcam_dd_usb.o

else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)
	USER_INCLUDES := /usr/local/include/
	CPPFLAGS = -I $(PWD)  -Wno-implicit-function-declaration 

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules -Wno-implicit-function-declaration 

install: default copy

copy:
	@-install -d /lib/modules/${shell uname -r}/kernel/drivers/sentech/
	@-install ./stcam_dd.ko /lib/modules/${shell uname -r}/kernel/drivers/sentech/stcam_dd.ko
	@-install -d $(USER_INCLUDES)/sentech
	@-install stcam_dd.h $(USER_INCLUDES)/sentech
	@-install 50-udev-stcam.rules /lib/udev/rules.d
	@rmmod stcam_dd -s 2>/dev/null || echo
	@depmod
	@modprobe stcam_dd

uninstall:
	@rmmod stcam_dd -s 2>/dev/null || echo
	@-rm -f /lib/modules/${shell uname -r}/kernel/drivers/sentech/stcam_dd.ko
	@-rm -rf $(USER_INCLUDES)/sentech
	@-rm -f /lib/udev/rules.d/50-udev-stcam.rules
	@depmod

clean:
	@- rm -rf *.ko *.o *.mod.c Module.symvers modules.order .stcam* .tmp_versions src/*.o src/.stcam*

endif
