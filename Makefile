

# top-level makefile


all:
	cd swwreader; make
	cd viewer; make

clean:
	cd swwreader; make clean
	cd viewer; make clean
	cd tests; make clean

test:
	cd tests; make test
	
install:
	cd swwreader; make install



