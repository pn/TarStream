test: Tar.o test.cpp
	g++ -g test.cpp Tar.o -o test
Tar.o: Tar.cpp Tar.h
	g++ -g Tar.cpp -c
clean:
	-rm -f test *.o
.PHONY: clean
