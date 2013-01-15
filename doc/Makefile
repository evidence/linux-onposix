.PHONY: all clean

all:
	-rm -fr html
	doxygen doxygen.conf
	( cat doxygen.conf ; echo "PROJECT_NUMBER = $(ONPOSIX_VERSION)" ) | doxygen -
	cp onposix.png html
	ln -s -f doc/html/index.html ../index.html

clean:
	-rm -fr html ../index.html
