

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
	mkdir /usr/local/etc/anuga-viewer
	cp -r images/ /usr/local/etc/anuga-viewer/
	cp -r bin/fonts/ /usr/local/etc/anuga-viewer/
	mv bin/anuga-viewer /usr/local/bin


