ARCH=arm
CROSS_COMPILE :=/home/nicolas/Documents/buildroot/output/host/usr/bin/arm-buildroot-linux-gnueabihf-
OUT_DIR=/home/nicolas/Documents/buildroot/output/build/linux-ACDS18.0_REL_GSRD_PR
DESTDIR = /home/nicolas/Documents/buildroot/output/target
LINUX_VARIABLES = PATH=$(PATH)
LINUX_VARIABLES += ARCH=$(ARCH)

ifneq ("$(KBUILD_BUILD_VERSION)","")
	LINUX_VARIABLES += KBUILD_BUILD_VERSION="$(KBUILD_BUILD_VERSION)"
endif

LINUX_VARIABLES += CROSS_COMPILE=$(CROSS_COMPILE)

ifneq ("$(DEVICETREE_SRC)","")
	LINUX_VARIABLES += CONFIG_DTB_SOURCE=$(DEVICETREE_SRC)
endif
LINUX_VARIABLES += INSTALL_MOD_PATH=$(INSTALL_MOD_PATH)

ifndef OUT_DIR
	$(error OUT_DIR is undefined, bad environment, you point OUT_DIR to the linux/
	kernel build output directory)
endif

KDIR ?= $(OUT_DIR)

default:
	$(MAKE) -C $(KDIR) $(LINUX_VARIABLES) M=$(CURDIR)

all:
	$(MAKE) -C $(KDIR) $(LINUX_VARIABLES) M=$(CURDIR)

clean:
	$(MAKE) -C $(KDIR) $(LINUX_VARIABLES) M=$(CURDIR) clean

help:
	$(MAKE) -C $(KDIR) $(LINUX_VARIABLES) M=$(CURDIR) help

modules:
	$(MAKE) -C $(KDIR) $(LINUX_VARIABLES) M=$(CURDIR) modules

modules_install:
	$(MAKE) -C $(KDIR) $(LINUX_VARIABLES) M=$(CURDIR) INSTALL_MOD_PATH=$(DESTDIR)/
	modules_install
