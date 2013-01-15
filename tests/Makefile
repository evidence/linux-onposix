../test: ../$(LIBNAME).a ../$(LIBNAME).so test.o
	$(CXX) $(CXXFLAGS) -o ../test -I ../include test.o $(OBJECTS) -I $(GTEST_INCLUDE_DIR) -L $(GTEST_LIB_DIR) -lpthread -lrt -lgtest -lonposix

test.o: test.cpp
	$(CXX) $(CXXFLAGS) -c -o test.o test.cpp -I ../include -I $(GTEST_INCLUDE_DIR)

.PHONY: clean

clean:
	-rm -fr *.o ../test

