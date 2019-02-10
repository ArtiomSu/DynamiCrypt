/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   network.h
 * Author: mrarchinton
 *
 * Created on 08 February 2019, 19:30
 */

#ifndef NETWORK_H
#define NETWORK_H

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
#include <string>
#include <vector>
#include <cstdio>

using boost::uint64_t;
using boost::uint32_t;
using boost::uint16_t;
using boost::uint8_t;

using boost::int64_t;
using boost::int32_t;
using boost::int16_t;
using boost::int8_t;


class Hive;
class Acceptor;
class Connection;

class Connection : public boost::enable_shared_from_this< Connection > {
    friend class Acceptor; // these classes can access private members of Connection class
    friend class Hive;

private:
    boost::shared_ptr< Hive > m_hive;
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::strand m_io_strand;
    boost::asio::deadline_timer m_timer;
    boost::posix_time::ptime m_last_time;
    std::vector< uint8_t > m_recv_buffer;
    std::list< int32_t > m_pending_recvs;
    std::list< std::vector< uint8_t > > m_pending_sends;
    int32_t m_receive_buffer_size;
    int32_t m_timer_interval;
    volatile uint32_t m_error_state;

protected:
    Connection( boost::shared_ptr< Hive > hive );
    virtual ~Connection();

private:
    Connection( const Connection & rhs );
    Connection & operator =( const Connection & rhs );
    void StartSend();
    void StartRecv( int32_t total_bytes );
    void StartTimer();
    void StartError( const boost::system::error_code & error );
    void DispatchSend( std::vector< uint8_t > buffer );
    void DispatchRecv( int32_t total_bytes );
    void DispatchTimer( const boost::system::error_code & error );
    void HandleConnect( const boost::system::error_code & error );
    void HandleSend( const boost::system::error_code & error,  std::list< std::vector< uint8_t > >::iterator itr );
    void HandleRecv( const boost::system::error_code & error, int32_t actual_bytes );
    void HandleTimer( const boost::system::error_code & error );

private:
	// Called when the connection has successfully connected to the local host.
	virtual void OnAccept( const std::string & host, uint16_t port ) = 0;

	// Called when the connection has successfully connected to the remote host.
	virtual void OnConnect( const std::string & host, uint16_t port ) = 0;

	// Called when data has been sent by the connection.
	virtual void OnSend( const std::vector< uint8_t > & buffer ) = 0;

	// Called when data has been received by the connection. 
	virtual void OnRecv( std::vector< uint8_t > & buffer ) = 0;

	// Called on each timer event.
	virtual void OnTimer( const boost::posix_time::time_duration & delta ) = 0;

	// Called when an error is encountered.
	virtual void OnError( const boost::system::error_code & error ) = 0;

public:
	// Returns the Hive object.
	boost::shared_ptr< Hive > GetHive();

	// Returns the socket object.
	boost::asio::ip::tcp::socket & GetSocket();

	// Returns the strand object.
	boost::asio::strand & GetStrand();

	// Sets the application specific receive buffer size used. For stream 
	// based protocols such as HTTP, you want this to be pretty large, like 
	// 64kb. For packet based protocols, then it will be much smaller, 
	// usually 512b - 8kb depending on the protocol. The default value is
	// 4kb.
	void SetReceiveBufferSize( int32_t size );

	// Returns the size of the receive buffer size of the current object.
	int32_t GetReceiveBufferSize() const;

	// Sets the timer interval of the object. The interval is changed after 
	// the next update is called.
	void SetTimerInterval( int32_t timer_interval_ms );

	// Returns the timer interval of the object.
	int32_t GetTimerInterval() const;

	// Returns true if this object has an error associated with it.
	bool HasError();

	// Binds the socket to the specified interface.
	void Bind( const std::string & ip, uint16_t port );

	// Starts an a/synchronous connect.
	void Connect( const std::string & host, uint16_t port );

	// Posts data to be sent to the connection.
	void Send( const std::vector< uint8_t > & buffer );

	// Posts a recv for the connection to process. If total_bytes is 0, then 
	// as many bytes as possible up to GetReceiveBufferSize() will be 
	// waited for. If Recv is not 0, then the connection will wait for exactly
	// total_bytes before invoking OnRecv.
	void Recv( int32_t total_bytes = 0 );

	// Posts an asynchronous disconnect event for the object to process.
	void Disconnect();
};



#endif /* NETWORK_H */

