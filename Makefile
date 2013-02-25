export LIBNAME=libonposix
export ONPOSIX_VERSION=1.3
export CXX = g++
export CXXFLAGS = -O3 -Wall -Wextra -fPIC

.PHONY: clean install doc

## Add googletest information for unit testing:
export GTEST_INCLUDE_DIR=~/googletest/include
export GTEST_LIB_DIR=~/googletest/lib

$(LIBNAME).so $(LIBNAME).a:
	$(MAKE) -C src

install: $(LIBNAME).so $(LIBNAME).a
ifeq ($(strip $(TARGET_DIR)),)
	sudo cp -f -a $(LIBNAME)* /usr/lib/ 
else
	cp -f -a $(LIBNAME)* $(TARGET_DIR)/usr/lib/ 
endif

ifeq ($(strip $(GTEST_INCLUDE_DIR)),)
test:
	@echo "Missing information about google test directories"
	@echo "Please set GTEST_INCLUDE_DIR and GTEST_LIB_DIR"
else
test: $(LIBNAME).so $(LIBNAME).a
	$(MAKE) -C tests
endif

doc:
	$(MAKE) -C doc

clean:
	$(MAKE) -C src clean
	$(MAKE) -C doc clean
	$(MAKE) -C tests clean

