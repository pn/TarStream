test: TarStream.o test.cpp
	g++ -g test.cpp TarStream.o -o test
TarStream.o: TarStream.cpp TarStream.h
	g++ -g TarStream.cpp -c
clean:
	-rm -f test *.o
.PHONY: clean simpletest debug
simpletest: test TarStream.o
	-@rm -rf tmp
	mkdir -p tmp
	./test . TarStream.o test
	mv test.tar tmp
	tar xf tmp/test.tar -C tmp
	md5sum tmp/TarStream.o > tmp/TarStream.o.md5
	md5sum tmp/test > tmp/test.md5
	md5sum -c TarStream.o
	md5sum -c test
	-rm -rf tmp
debug:
	gdb --tui --args ./test . TarStream.o test
