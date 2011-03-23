#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include "TarStream.h"
#include "stdio.h"

const int CHUNK = 10000;

using namespace std;

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		cout << "Usage: " << argv[0] << " basedir file1 [file2] [...]" << endl;
		return 1;
	}
	TarStream tar;
	for (int i=2; i < argc; i++)
	{
		cout << argv[i] << endl;
		tar.putFile(argv[i], argv[i]);
	}

	int f = open("test.tar", O_WRONLY|O_CREAT, 0644);
	int i;
	char buf[CHUNK];
	for (i = 0; i < tar.getSize(); i+=CHUNK)
	{
		tar.getChunk(buf, CHUNK);
		if (tar.getSize() - i >= CHUNK)
			write(f, (void*)buf, CHUNK);
		else
			write(f, (void*)buf, tar.getSize() - i);
	}
	close(f);
	return 0;
}
