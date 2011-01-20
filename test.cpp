#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include "TarStream.h"
#include "stdio.h"

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

	int f = open("tmp.tar", O_WRONLY);
	int i;
	for (i = 0; i < tar.getSize(); i+=1024)
	{
		string s = tar.getChunk(i, 1024);
		if (tar.getSize() - i >= 1024)
			write(f, (void*)s.c_str(), 1024);
		else
			write(f, (void*)s.c_str(), tar.getSize() - i);
	}
	close(f);
	return 0;
}
