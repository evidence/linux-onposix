.PHONY: all clean

all:
	-rm -fr html
	doxygen doxygen.conf
	( cat doxygen.conf ; echo "PROJECT_NUMBER = $(ONPOSIX_VERSION)" ) | doxygen -

clean:
	-rm -fr html
