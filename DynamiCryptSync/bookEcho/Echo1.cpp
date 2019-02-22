
/*
 
 * a server that echoes back anything a client writes, and then closes the client's
 connection. The server can handle any number of clients. As each client connects, it
 sends a message. The server receives the full message and sends it back. After that, it
 closes the connection.
 
 each Echo client connects to the server, sends a message, and reads what
the server replies, making sure it's the same message it sent and finishes talking to
the server.
 
 */

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


using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;



ip::tcp::endpoint ep( ip::address::from_string("127.0.0.1"), 8001);
boost::asio::io_service service;

size_t read_complete(char * buf, const  boost::system::error_code & err, size_t bytes){
    if ( err) return 0;
        bool found = std::find(buf, buf + bytes, '\n') < buf + bytes;
    // we read one-by-one until we get to enter, no buffering
    return found ? 0 : 1;
}

void sync_echo(std::string msg) {
    msg += "\n";
    ip::tcp::socket sock(service);
    sock.connect(ep);
    sock.write_some(buffer(msg));
    char buf[1024];
    int bytes = read(sock, buffer(buf), boost::bind(read_complete,buf,_1,_2));
    /*
     used the free function read() , because I want to read everything up to '\n' . The sock.read_some() function would not be
     enough, since that would only read what's available, which is not necessarily the whole message.
     
     The third argument to the read() function is a completion handler. It will return 0
     when it's read the full message. Otherwise, it will return the maximum buffer it can
     read in the next step (until read is complete). In our case, this is always 1 , because
     we never want to mistakenly read more than we need.  
     
     */
    
    std::string copy(buf, bytes - 1);
    msg = msg.substr(0, msg.size() - 1);
    std::cout << "server echoed our " << msg << ": " << (copy == msg ? "OK" : "FAIL") << std::endl;
    sock.close();
}


int main(int argc, char* argv[]) {
    char* messages[] = { "John says hi", "so does James", "Lucy just got home", "Boost.Asio is Fun!", 0 };
    boost::thread_group threads;
    for ( char ** message = messages; *message; ++message) {
        threads.create_thread( boost::bind(sync_echo, *message));
        boost::this_thread::sleep( boost::posix_time::millisec(100));
    }
    threads.join_all();
    
    // since we're synchronous, there's no need to call service.run() .
    
}