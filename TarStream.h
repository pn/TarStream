#ifndef _TARSTREAM_H_
#define _TARSTREAM_H_
#include <string>
#include <vector>
using namespace std;

struct tarHeaderBlock {
    char name[100];     // file name
    char mode[8];       // file mode
    char uid[8];        // owner user ID
    char gid[8];        // owner group ID
    char size[12];      // length of file in bytes
    char mtime[12];     // modified time of file
    char chksum[8];     // header checksum
    char typeflag;      // see type constants
    char linkname[100]; // name of linked file
    char magic[8];      // "ustar  " + null terminator
    char uname[32];     // owner user name
    char gname[32];     // owner group name
    char devmajor[8];   // device major number
    char devminor[8];   // device minor number
    char prefix[155];   // prefix for file names longer than 100 bytes
    char filler[12];    // filler to make header even 512 bytes
};

class TarStream {
	public:
	TarStream(string baseDir, vector<string>);
	~TarStream();
	const string& getChunk(unsigned long start, unsigned long size);
};

#endif //_TARSTREAM_H_
