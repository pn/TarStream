test: TarStream.o test.cpp
	g++ -g test.cpp TarStream.o -o test
TarStream.o: TarStream.cpp TarStream.h
	g++ -g TarStream.cpp -c
clean:
	-rm -f smtest test *.o
runtest: test TarStream.o
	-@rm -rf tmp
	mkdir -p tmp
	./test . TarStream.o test
	mv test.tar tmp
	tar xf tmp/test.tar -C tmp
	md5sum tmp/TarStream.o > tmp/TarStream.o.md5
	md5sum tmp/test > tmp/test.md5
	md5sum -c tmp/TarStream.o.md5
	md5sum -c tmp/test.md5
	-rm -rf tmp
smtest: smtest.cpp TarStream.o
	g++ -g -I /usr/include/boost smtest.cpp -lboost -c -o smtest.o
	g++ -g smtest.o TarStream.o -o smtest
runsmtest: smtest
	-rm -rf foo b c test1 test2 stream.out stream.tar
	mkdir -p foo b c test1 test2
	cp /etc/motd foo
	tar cf stream.tar foo/motd TarStream.cpp b c
	./smtest
	cd test1 && tar xf ../stream.tar
	cd test2 && tar xf ../stream.out
	diff test1 test2
	rm -rf foo b c test1 test2 stream.out stream.tar
debug:
	gdb --tui --args ./test . TarStream.o test
.PHONY: clean runtest runsmtest debug
