#include <string.h>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include "TarStream.h"
using namespace std;

TarStream::TarStream() : readCursor(0)
{
}

TarStream::~TarStream()
{
}

void TarStream::putFile( const std::string& filePath, const std::string& pathInTar )
{
	TarEntry entry(filePath, pathInTar);
	this->files.push_back(entry);
}

void TarStream::putDirectory( const std::string& dirPath )
{
	TarEntry entry(dirPath, dirPath, true);
	this->files.push_back(entry);
}

size_t TarStream::getChunk(char* buf, size_t size)
{
	size_t start = readCursor;
	char *p = buf;
	size_t file_size, orig_size = size;
	vector<class TarEntry>::iterator ci;
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
		file_size = ci->getSize();
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
	readCursor += orig_size - size;
	if(getSize()-readCursor < size)
		return orig_size + getSize() - readCursor;
	return orig_size;
}

bool TarStream::seekg (size_t pos)
{
	if(pos < 0 || pos >=getSize())
	{
		return false;
	}
	readCursor = pos;
	return true;
}

size_t TarStream::getSize() const
{
	size_t result = 0;
	vector<class TarEntry>::const_iterator ci;
	for (ci = files.begin(); ci != files.end(); ++ci)
	{
		result += ci->getSize();
	}
	return result + 2 * sizeof(TarHeaderBlock);
}

unsigned int TarStream::TarEntry::calculateChkSum(const char *header, const size_t s)
{
	unsigned int sum = 0, i;
	for (i = 0; i < s; i++) {
		sum += header[i] & 0xff;
	}
	return sum;
}

TarStream::TarEntry::TarEntry(string path, string name, bool dir) : path(path)
{
	if ( name == "")
		name = path;
	this->path = path;
	this->name = name;
	isDir = dir;
	struct stat filestat;
	stat(path.c_str(), &filestat);
	if(isDir)
		size = 0;
	else
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
	if (S_ISDIR(filestat.st_mode))
		header.typeflag = '5';
	else
		header.typeflag = '0'; // regular file
	memset(header.chksum, ' ', sizeof(header.chksum));
	snprintf(header.chksum, sizeof(header.chksum), "%06o", calculateChkSum((const char *)&header, sizeof(header)));
	
	//fprintf(stderr, "Created file %s, size %llu\n", this->name.c_str(), (unsigned long long)size);
}

TarStream::TarEntry::~TarEntry()
{
}

const size_t TarStream::TarEntry::getSize() const
{
	int result = 0;
	result += sizeof(header);
	result += size;
	if ((size % sizeof(header)) > 0)
		result += sizeof(header) - (size % sizeof(header));
	//fprintf(stderr, "Size of %s is %d\n", path.c_str(), result);
	return result;
}

string TarStream::TarEntry::getChunk(size_t start, size_t size)
{
	char buf[size];
	char *p = buf;
	if (!file.is_open())
		file.open(path.c_str(), ifstream::binary);
	if (!file.is_open())
	{
		fprintf(stderr, "Cant't read file: %s\n", path.c_str());
	}

	if (start <= sizeof(header))
	{
		memcpy(p, (const char *)&header, sizeof(header)-start);
		p += sizeof(header)-start;
	}
	if(start+size > sizeof(header))
	{
		if(isDir)
		{
			string result(buf, sizeof(header)-start);
			return result;
		}
		if(start <= sizeof(header))
			start = 0;
		else
			start -= sizeof(header);
		file.seekg(start);
		file.read(p, size - (p - buf));
		streamsize numread = file.gcount();
		if (numread < size - (p - buf))
		{
			memset(p+numread, 0, size - (p - buf) - numread);
		}
		file.close();
	}
	string result(buf, size);
	return result;
}

