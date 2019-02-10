/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "network.h"
#include <cstdlib>
#include <iostream>
#include <boost/array.hpp>
#include <ctime>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/interprocess/detail/atomic.hpp>
#include <string>
#include <vector>
#include <cstdio>
#include <unistd.h>

boost::mutex global_stream_lock;

class MyConnection : public Connection
{
private:

private:
	void OnAccept( const std::string & host, uint16_t port )
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << host << ":" << port << std::endl;
		global_stream_lock.unlock();

		// Start the next receive
		Recv();
	}

	void OnConnect( const std::string & host, uint16_t port )
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << host << ":" << port << std::endl;
		global_stream_lock.unlock();

		// Start the next receive
		Recv();
	}

	void OnSend( const std::vector< uint8_t > & buffer )
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << buffer.size() << " bytes" << std::endl;
		for( size_t x = 0; x < buffer.size(); ++x )
		{
			std::cout << std::hex << std::setfill( '0' ) << 
				std::setw( 2 ) << (int)buffer[ x ] << " ";
			if( ( x + 1 ) % 16 == 0 )
			{
				std::cout << std::endl;
			}
		}
		std::cout << std::endl;
		global_stream_lock.unlock();
	}

	void OnRecv( std::vector< uint8_t > & buffer )
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << buffer.size() << " bytes" << std::endl;
		for( size_t x = 0; x < buffer.size(); ++x )
		{
			std::cout << std::hex << std::setfill( '0' ) << 
				std::setw( 2 ) << (int)buffer[ x ] << " ";
			if( ( x + 1 ) % 16 == 0 )
			{
				std::cout << std::endl;
			}
		}
		std::cout << std::endl;
		global_stream_lock.unlock();

		// Start the next receive
		Recv();

		// Echo the data back
		Send( buffer );
	}

	void OnTimer( const boost::posix_time::time_duration & delta )
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << delta << std::endl;
		global_stream_lock.unlock();
	}

	void OnError( const boost::system::error_code & error )
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << error << std::endl;
		global_stream_lock.unlock();
	}

public:
	MyConnection( boost::shared_ptr< Hive > hive )
		: Connection( hive )
	{
	}

	~MyConnection()
	{
	}
};

class MyAcceptor : public Acceptor
{
private:

private:
	bool OnAccept( boost::shared_ptr< Connection > connection, const std::string & host, uint16_t port )
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << host << ":" << port << std::endl;
		global_stream_lock.unlock();

		return true;
	}

	void OnTimer( const boost::posix_time::time_duration & delta )
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << delta << std::endl;
		global_stream_lock.unlock();
	}

	void OnError( const boost::system::error_code & error )
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << error << std::endl;
		global_stream_lock.unlock();
	}

public:
	MyAcceptor( boost::shared_ptr< Hive > hive )
		: Acceptor( hive )
	{
	}

	~MyAcceptor()
	{
	}
};

int main( int argc, char * argv[] )
{
	boost::shared_ptr< Hive > hive( new Hive() );

	boost::shared_ptr< MyAcceptor > acceptor( new MyAcceptor( hive ) );
	acceptor->Listen( "127.0.0.1", 7777 );

	boost::shared_ptr< MyConnection > connection( new MyConnection( hive ) );
	acceptor->Accept( connection );

	while( true )
	{
		hive->Poll();
		usleep( 1 );
	}

	hive->Stop();

	return 0;
}