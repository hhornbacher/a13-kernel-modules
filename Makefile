obj-m	+= rht22/rht22.o
obj-m	+= sunxi-w1/sunxi-w1.o
obj-m	+= mod-io/mod-io.o
obj-m	+= gpio-test/gpio-test.o

all:
	make -j4 ARCH=arm CROSS_COMPILE=arm-none-eabi- -C ../kernel/src M=$(PWD) modules

clean:
	make -j4 ARCH=arm CROSS_COMPILE=arm-none-eabi- -C ../kernel/src M=$(PWD) clean