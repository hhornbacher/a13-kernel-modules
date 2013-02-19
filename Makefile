DIRS	= mod-io sunxi-w1 gpio-test

all : mod-io/mod-io.o sunxi-w1/sunxi-w1.o gpio-test/gpio-test.o move

mod-io/mod-io.o : force_look
	cd mod-io; $(MAKE)

sunxi-w1/sunxi-w1.o : force_look
	cd sunxi-w1; $(MAKE)
	
gpio-test/gpio-test.o : force_look
	cd gpio-test; $(MAKE)
	
move : mod-io/mod-io.o sunxi-w1/sunxi-w1.o gpio-test/gpio-test.o
	-for d in $(DIRS); do (cd $$d; mv *.ko ../ ); done;
	
clean :
	-for d in $(DIRS); do (cd $$d; $(MAKE) clean ); done; rm *.ko
	
force_look :
	true
