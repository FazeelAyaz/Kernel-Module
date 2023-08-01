# Makefile for keyboard character driver

obj-m += my_keyboard.o

# Path to kernel source
KDIR := /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
    



