test: TarStream.o test.cpp
	g++ -g test.cpp TarStream.o -o test
TarStream.o: TarStream.cpp TarStream.h
	g++ -g TarStream.cpp -c
clean:
	-rm -f test *.o
.PHONY: clean
