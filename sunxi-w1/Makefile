obj-m	+= sunxi-w1.o

all:
	make -j4 ARCH=arm CROSS_COMPILE=arm-none-eabi- -C ../../kernel/src M=$(PWD) modules

clean:
	make -j4 ARCH=arm CROSS_COMPILE=arm-none-eabi- -C ../../kernel/src M=$(PWD) clean