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
	if(size > 0)
	{
		// add tar tail
		if(size > 2 * sizeof(tarHeaderBlock))
			size = 2 * sizeof(tarHeaderBlock);
		while(size--)
			result += (char)0;
	}
	
	return result;
}

TarStream::TarFile::TarFile(string baseDir, string name) : name(name)
{
	struct stat filestat;
	stat(name.c_str(), &filestat);
	size = filestat.st_size;
	memset(&header, 0, sizeof(header));
	snprintf(header.name, sizeof(header.name), "%s", name.c_str());//TODO: long file names
	snprintf(header.mode, sizeof(header.mode), "%07o", filestat.st_mode);
	snprintf(header.uid, sizeof(header.uid), "%08d", 1);
	snprintf(header.gid, sizeof(header.gid), "%08d", 1);
	snprintf(header.size, sizeof(header.size), "%llu", size);
	snprintf(header.magic, sizeof(header.magic), "ustar  ");
	snprintf(header.uname, sizeof(header.uname), "root");
	snprintf(header.gname, sizeof(header.gname), "root");
	snprintf(header.mtime, sizeof(header.mtime), "%011lo", filestat.st_mtime);
	unsigned int sum = 0;
	char *p = (char *) &header;
	char *q = p + sizeof(header);
	while (p < q) sum += *p++ & 0xff;
	for (int i = 0; i < sizeof(header.chksum); ++i) {
		sum += ' ';
	}
	snprintf(header.chksum, sizeof(header.chksum), "%06o", sum);
	
	fprintf(stderr, "Created file %s, size %llu\n", this->name.c_str(), size);
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

