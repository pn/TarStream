#include <iostream>
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
	vector<string> files;
	for (int i=2; i < argc; i++)
	{
		cout << argv[i] << endl;
		files.push_back((string)argv[i]);
	}
	TarStream tar((string)argv[1], files);

	int f = open("test.tar", O_WRONLY|O_CREAT, 0644);
	int i;
	for (i = 0; i < tar.getSize(); i+=CHUNK)
	{
		string s = tar.getChunk(i, CHUNK);
		if (tar.getSize() - i >= CHUNK)
			write(f, (void*)s.c_str(), CHUNK);
		else
			write(f, (void*)s.c_str(), tar.getSize() - i);
	}
	close(f);
	return 0;
}
