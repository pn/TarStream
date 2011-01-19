#include <iostream>
#include "Tar.h"

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
	return 0;
}
