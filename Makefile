ONPOSIX_VERSION=1.2
export ONPOSIX_VERSION
LIBNAME = libonposix
.PHONY: all clean install doc

all:
	$(MAKE) -C src

ifeq ($(strip $(TARGET_DIR)),)
install:
	sudo cp -f -a $(LIBNAME)* /usr/lib/ 
else
install:
	cp -f -a $(LIBNAME)* $(TARGET_DIR)/usr/lib/ 
endif

clean:
	$(MAKE) -C src clean
	$(MAKE) -C doc clean
	-rm -f index.html

doc:
	$(MAKE) -C doc
	ln -s -f doc/html/index.html index.html
