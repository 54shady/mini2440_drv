KERN_DIR =/opt/FriendlyARM/mini2440/linux-2.6.22.6_jz2440/
OUT_DIR =$(KERN_DIR)/out

all:
	make -C $(KERN_DIR) M=`pwd` modules O=$(OUT_DIR)

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean O=$(OUT_DIR)
	rm -rf modules.order Module.symvers

obj-m	+=	first_drv.o
