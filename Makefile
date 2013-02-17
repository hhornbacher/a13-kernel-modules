DIRS	= mod-io sunxi-w1

all : mod-io/mod-io.o sunxi-w1/sunxi-w1.o

mod-io/mod-io.o : force_look
	cd mod-io; $(MAKE)

sunxi-w1/sunxi-w1.o : force_look
	cd sunxi-w1; $(MAKE)
	
clean :
	-for d in $(DIRS); do (cd $$d; $(MAKE) clean ); done
	
force_look :
	true
