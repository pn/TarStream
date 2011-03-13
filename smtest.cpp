#include <iostream>
#include <fstream>
#include <boost/shared_ptr.hpp>
#include "TarStream.h"

using namespace std;

void sendOverHttp( const char *buf, size_t bufLength )
{
	static fstream f;
	if( !f.is_open() )
	{
		f.open("stream.out", ios_base::out);
	}
	f.write(buf, bufLength);
}

int main(int argc, char *argv[])
{
    using namespace boost;

    shared_ptr<TarStream> tarStream( new TarStream() );

    try
    {
        tarStream->putFile( "/etc/motd", "foo/motd" );
        tarStream->putFile( "./TarStream.cpp", "TarStream.cpp" );
        tarStream->putDirectory( "b" );
        tarStream->putDirectory( "c" );
    }catch( ... )
    {
        std::cout << "ERROR while creating tar" << std::endl;
    }

    try
    {
        size_t bytesToSend = tarStream->getSize();
        char sendBuffer[1024];

        while( bytesToSend > 0 )
        {
            size_t bytesInBuffer = tarStream->getChunk( sendBuffer, 1024 );
            sendOverHttp( sendBuffer, bytesInBuffer );
            bytesToSend -= bytesInBuffer;
        }

    } catch( ... )
    {
        std::cout << "ERROR while sending tar stream" << std::endl;
    }


    std::cout << "Tar Stream sent sucessfully" << std::endl;

    return 0;
}
