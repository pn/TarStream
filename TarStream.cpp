#include <string.h>
#include <vector>
#include <sys/stat.h>
#include <stdio.h>
#include "TarStream.h"
using namespace std;

TarStream::TarStream(string baseDir, vector<string> files)
{
	vector<string>::const_iterator ci;
	for (ci = files.begin(); ci != files.end(); ++ci)
	{
		TarFile file(baseDir, *ci);
		this->files.push_back(file);
	}
}

TarStream::~TarStream()
{
}

string TarStream::getChunk(FileLen start, FileLen size)
{
	string result;
	vector<class TarFile>::const_iterator ci;
	FileLen file_size;
	for (ci = files.begin(); ci != files.end(); ++ci)
	{
		file_size = ci->getSize();
		if (start >= file_size)
			start -= file_size;
		else
			break;
	}
	while(size > 0 || ci == files.end())
	{
		if (size > file_size)
		{
			result += ci->getChunk(start, file_size);
			size -= file_size;
			start = 0;
		}
		else
		{
			result += ci->getChunk(start, size);
			break;
		}
		ci++;
	}
	
	return result;
}

TarStream::TarFile::TarFile(string baseDir, string name) : name(name)
{
	struct stat filestat;
	stat(name.c_str(), &filestat);
	size = filestat.st_size;
	memset(&header, 0, sizeof(header));
	snprintf(header.size, sizeof(header.size), "%llu", size);
	printf("Created file %s, size %llu\n", this->name.c_str(), size);
}

TarStream::TarFile::~TarFile()
{
}

const FileLen TarStream::TarFile::getSize() const
{
	return (FileLen)header.size + sizeof(header);
}

string TarStream::TarFile::getChunk(FileLen start, FileLen size) const
{
}
