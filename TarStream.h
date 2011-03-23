#ifndef _TARSTREAM_H_
#define _TARSTREAM_H_
#include <string>
#include <vector>

struct TarHeaderBlock {
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
		/*
		 * Creates 'empty' TarStream. Sets internal read cursor to the beginning of the stream.
		 */
		TarStream();

		/*
		 * Releases internal structures if needed.
		 */
		virtual ~TarStream();

		/*
		 * This function adds file (or directory) pointed by 'filePath' into tar stream. 
		 * 'pathInTar' specifies where 'filePath' should be placed inside Tar.
		 * Function should throw std::invalid_argument when:
		 *  * 'filePath' doesn't exist ( use stat function to implement this )
		 *  * 'filePath' can't be open for reading ( use ifstream class to check whether file can be read ),
		 *  * 'filePath' has been already added into 'pathInTar'. Note that it should be possible to put 'filePath' more than once into different 'pathInTar'. ( This could be implemented easly by using
		 *  std::set<TarFile>. See explanation below regarding std::set<TarFile>
		 */
		void putFile( const std::string& filePath, const std::string& pathInTar );

		/*
		 * This function adds directory into TarStream.
		 * Function should throw std::invalid_argument exception if dirPath has been already added.
		 */
		void putDirectory( const std::string& dirPath );

		/*
		 * Puts tar stream data into 'buffer'. 'bufferLen' specifies size of 'buffer'.
		 * Function returns number of bytes copied into 'buffer'. Note that 'getChunk' function moves internal read cursor into new position.
		 * Function should throw std::runtime_exception in case of problems with opening files for reading etc.
		 */
		size_t getChunk(char* buffer, size_t bufferLen);

		/*
		 * Moves internal read cursor into new position in the stream ( counting from the beginning of the stream ).
		 * Should return true if operation succeeded. Otherwise false. False should be returned if pos is greater or equal of the stream size ( see 'getSize() const' function )
		 */
		bool seekg( size_t pos );

		/*
		 * Function should return size of stream calculated basen on already added files.
		 * Function should take into consideration TarHeaders, Long file names ( in that case extra TarHeaders are included ) and so on.
		 * TODO: Add support for long file names.
		 */
		size_t getSize() const;
	private:
		class TarEntry {
			public:
				const size_t getSize() const;
				TarEntry(const TarEntry &entry) : name(entry.name), path(entry.path), size(entry.size), isDir(entry.isDir), header(entry.header)
				{
				}
				TarEntry& operator=(const TarEntry &entry)
				{
					this->name = entry.name;
					this->path = entry.path;
					this->size = entry.size;
					this->isDir = entry.isDir;
					this->header = entry.header;
					return *this;
				}
				TarEntry(std::string, std::string="", bool=false);
				~TarEntry();
				std::string getChunk(size_t start, size_t size);
			private:
				unsigned int calculateChkSum(const char *header, const size_t s);
				std::string name;
				std::string path;
				size_t size;
				bool isDir;
				TarHeaderBlock header;
				std::ifstream file;
		};
		std::vector<class TarEntry> files;
		//std::set<class TarEntry> files; TODO
		size_t readCursor;
};

#endif //_TARSTREAM_H_
