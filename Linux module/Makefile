obj-m += tryButton.o
ksrc = /lib/modules/$(shell uname -r)/build
PWD =$(shell pwd)
all:
	make -C $(ksrc) M=$(PWD) modules

clean:
	make -C $(ksrc) M=$(PWD) clean
