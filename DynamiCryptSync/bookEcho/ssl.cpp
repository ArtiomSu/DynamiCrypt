#include <cstdlib>
#include <iostream>
#include <boost/array.hpp>
#include <ctime>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <string>
#include <boost/asio/ssl.hpp>

/*
    compile using this
    g++ ssl.cpp -lboost_system -lpthread -lcrypto -lssl -o ssl

*/

using namespace boost::asio;
io_service service;

int main(int argc, char* argv[]) {
    typedef ssl::stream<ip::tcp::socket> ssl_socket;
    ssl::context ctx(ssl::context::sslv23);
    ctx.set_default_verify_paths();
    // Open an SSL socket to the given host
    io_service service;
    ssl_socket sock(service, ctx);
    ip::tcp::resolver resolver(service);
    std::string host = "www.yahoo.com";
    ip::tcp::resolver::query query(host, "https");
    connect(sock.lowest_layer(), resolver.resolve(query));
    sock.lowest_layer().set_option(ip::tcp::no_delay(true));
    // The SSL handshake 
    sock.set_verify_mode(ssl::verify_none);
    sock.set_verify_callback(ssl::rfc2818_verification(host));
    sock.handshake(ssl_socket::client);

    std::string req = "GET /index.html HTTP/1.0\r\nHost: " 
        + host + "\r\nAccept: */*\r\nConnection: close\r\n\r\n";
    write(sock, buffer(req.c_str(), req.length()));
    char buff[512];
    boost::system::error_code ec;
    while ( !ec) {
        int bytes = read(sock, buffer(buff), ec);
        std::cout << std::string(buff, bytes);
    }
}

/*
When you connect to the remote host,
you use sock.lowest_layer() , in other words, you use the underlying socket
(since ssl::stream is just a wrapper). The next three lines perform the handshake.
Once that is done, you make the HTTP request with the Boost.Asio's write()
function, and read ( read() ) all incoming bytes.
When implementing SSL servers, things get a bit more complicated. Boost.Asio
comes with an example of an SSL server, which you'll find in boost/libs/asio/
example/ssl/server.cpp .
*/