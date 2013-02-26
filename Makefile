obj-m	+= dht22-sun5i/dht22-sun5i.o
obj-m	+= mod-io/mod-io.o
obj-m	+= w1-sun5i/w1-sun5i.o
obj-m	+= lcd1602/lcd1602.o

all:
	make -j4 ARCH=arm CROSS_COMPILE=arm-none-eabi- -C ../kernel/src M=$(PWD) modules

clean:
	make -j4 ARCH=arm CROSS_COMPILE=arm-none-eabi- -C ../kernel/src M=$(PWD) clean