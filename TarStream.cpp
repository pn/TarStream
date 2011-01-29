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

string TarStream::getChunk(size_t start, size_t size)
{
	char buf[size];
	char *p = buf;
	size_t file_size, orig_size = size;
	vector<class TarFile>::const_iterator ci;
	for (ci = files.begin(); ci != files.end(); ++ci)
	{
		file_size = ci->getSize();
		if (start >= file_size)
			start -= file_size;
		else
			break;
	}
	while(size > 0 && ci != files.end())
	{
		if (size > file_size - start)
		{
			memcpy(p, ci->getChunk(start, file_size - start).c_str(), file_size - start);
			p+=file_size - start;
			size -= (file_size - start);
			start = 0;
		}
		else
		{
			memcpy(p, ci->getChunk(start, size).c_str(), size);
			p+=size;
			size = 0;
			break;
		}
		ci++;
	}
	memset(p, 0, size);
	
	string result(buf, orig_size);
	return result;
}

size_t TarStream::getSize() const
{
	size_t result = 0;
	vector<class TarFile>::const_iterator ci;
	for (ci = files.begin(); ci != files.end(); ++ci)
	{
		result += ci->getSize();
	}
	return result + 2 * sizeof(tarHeaderBlock);
}

TarStream::TarFile::TarFile(string baseDir, string name) : name(name)
{
	const char magic = 6;
	struct stat filestat;
	stat(name.c_str(), &filestat);
	size = filestat.st_size;
	memset(&header, 0, sizeof(header));
	snprintf(header.name, sizeof(header.name), "%s", name.c_str());//TODO: long file names
	snprintf(header.mode, sizeof(header.mode), "%07o", (unsigned int)filestat.st_mode & (unsigned int)0777);
	snprintf(header.uid, sizeof(header.uid), "%07o", filestat.st_uid);
	snprintf(header.gid, sizeof(header.gid), "%07o", filestat.st_gid);
	snprintf(header.size, sizeof(header.size), "%011llo", (unsigned long long)size);
	snprintf(header.magic, sizeof(header.magic), "ustar  ");
	snprintf(header.uname, sizeof(header.uname), "");
	snprintf(header.gname, sizeof(header.gname), "");
	snprintf(header.mtime, sizeof(header.mtime), "%011lo", filestat.st_mtime);
	memset(header.chksum, ' ' + magic, sizeof(header.chksum));
	unsigned int sum = 0;
	char *p = (char *) &header;
	char *q = p + sizeof(header);
	int i=0;
	while (p < q) { sum += *p++ & 0xff; i++;}
	memset(header.chksum, ' ', sizeof(header.chksum));
	fprintf(stderr, "summed chars %d\n", i);
	snprintf(header.chksum, sizeof(header.chksum), "%06o", sum);
	header.typeflag = '0'; // regular file
	
	fprintf(stderr, "Created file %s, size %llu\n", this->name.c_str(), (unsigned long long)size);
}

TarStream::TarFile::~TarFile()
{
}

const size_t TarStream::TarFile::getSize() const
{
	return size + 2 * sizeof(header) - size % sizeof(header);
}

string TarStream::TarFile::getChunk(size_t start, size_t size) const
{
	char buf[size];
	char *p = buf;
	FILE *pFile = fopen(header.name, "r");
	if (start <= sizeof(header))
	{
		memcpy(p, (const char *)&header, sizeof(header)-start);
		p += sizeof(header)-start;
	}
	if(start+size > sizeof(header))
	{
		if(start <= sizeof(header))
			start = 0;
		else
			start -= sizeof(header);
		fseek(pFile, start, SEEK_SET);
		size_t numread = fread(p, 1, size - (p - buf), pFile);
		if (numread < size - (p - buf))
		{
			memset(p+numread, 0, size - (p - buf) - numread);
		}
		fclose(pFile);
	}
	string result(buf, size);
	return result;
}

