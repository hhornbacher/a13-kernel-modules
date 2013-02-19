DIRS	= mod-io sunxi-w1

all : mod-io/mod-io.o sunxi-w1/sunxi-w1.o move

mod-io/mod-io.o : force_look
	cd mod-io; $(MAKE)

sunxi-w1/sunxi-w1.o : force_look
	cd sunxi-w1; $(MAKE)
	
move: mod-io/mod-io.o sunxi-w1/sunxi-w1.o
	mv mod-io/mod-io.ko ./; mv sunxi-w1/sunxi-w1.ko ./
	
clean :
	-for d in $(DIRS); do (cd $$d; $(MAKE) clean ); done; rm *.ko
	
force_look :
	true
