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

io_service service;
size_t read_complete(char * buff, const boost::system::error_code & err, size_t bytes) {
    if ( err) return 0;
        bool found = std::find(buff, buff + bytes, '\n') < buff + bytes;
    // we read one-by-one until we get to enter, no buffering
    return found ? 0 : 1;
}

void handle_connections() {
    ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(),8001));
    char buff[1024];
    while ( true) {
        cout << "Client attached" << endl;
        ip::tcp::socket sock(service);
        acceptor.accept(sock);
        int bytes = read(sock, buffer(buff),boost::bind(read_complete,buff,_1,_2));
        std::string msg(buff, bytes);
        cout << "Client message is " << msg << endl;
        sock.write_some(buffer(msg));
        sock.close();
    }
}
int main(int argc, char* argv[]) {
    handle_connections();
}

/*
 
Since we're single-threaded, we accept a new client, read the message it sends us, echo it back, and then wait for the next client. 
Let's say, if two clients connect at once, the second client will have to wait for the server to service the first client.
Notice again that since we're synchronous, there's no need to call service.run() .
 
 
 */