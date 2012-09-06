.PHONY: all clean

all:
	-rm -fr html
	doxygen doxygen.conf
	( cat doxygen.conf ; echo "PROJECT_NUMBER = $(ONPOSIX_VERSION)" ) | doxygen -
	cp onposix.png html

clean:
	-rm -fr html
