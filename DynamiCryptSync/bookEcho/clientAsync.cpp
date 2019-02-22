

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
#include <vector>


using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;


//MEM_FN* macros are convenience macros, and they enforce always using a shared pointer to *this , via the shared_ptr_from_this() function.
#define MEM_FN(x) boost::bind(&self_type::x, shared_from_this())
#define MEM_FN1(x,y) boost::bind(&self_type::x, shared_from_this(),y)
#define MEM_FN2(x,y,z) boost::bind(&self_type::x, shared_from_this(),y,z)

// every asynchronous operation starts a new asynchronous operation, keeping the service. run() busy.


/*
 We want to always use shared pointers to talk_to_svr , so that as long as there are
 asynchronous operations on an instance of talk_to_svr , that instance is alive. In
 order to avoid mistakes, such as constructing an instance of the talk_to_svr object
 on the stack, I've made the constructor private and disallowed copy construction
 (derived from boost::noncopyable ).
 */

io_service service;

class talk_to_svr : public boost::enable_shared_from_this<talk_to_svr>, boost::noncopyable {
  
public:
    typedef boost::system::error_code error_code;
    typedef boost::shared_ptr<talk_to_svr> ptr;
    typedef talk_to_svr self_type;
    
    talk_to_svr(const std::string & message) : sock_(service), started_(true), message_(message) {
    
    }
    
    void start(ip::tcp::endpoint ep) {
        sock_.async_connect(ep, MEM_FN1(on_connect,_1));
    }
    
    static ptr start(ip::tcp::endpoint ep, const std::string &message) { //To construct a connection, just call talk_to_svr::start(endpoint, message) .
        ptr new_(new talk_to_svr(message));
        new_->start(ep);
        return new_;
    }
    
    void stop() {
        if ( !started_) return;
        started_ = false;
        sock_.close();
    }
    
    bool started() { return started_; }
    
    void do_read() { //The do_read() function will make sure that we read a line from the server, at which point on_read() is called.
        async_read(sock_, buffer(read_buffer_), MEM_FN2(read_complete,_1,_2), MEM_FN2(on_read,_1,_2));
    }
    
    /*
     The do_write() function will first copy the message into the buffer 
     (since msg will probably go out of scope and be destroyed by the time the async_write actually takes place), 
     and then make sure on_write() is called after the actual write takes place.
     */
    void do_write(const std::string & msg) {
        if ( !started() ) return;
        std::copy(msg.begin(), msg.end(), write_buffer_);
        //cout << "inside do_write message is " << msg << endl; // works ok
        sock_.async_write_some( buffer(write_buffer_, msg.size()), MEM_FN2(on_write,_1,_2));
    }
    
    /*
    ok lol so apparently read_complete is executed pretty much all the time since out put looks like this
    inside read_complete number of bytes is 0
    inside read_complete number of bytes is 0
    inside read_complete number of bytes is 0
    inside read_complete number of bytes is 1
    inside read_complete number of bytes is 2
    inside read_complete number of bytes is 3
    inside read_complete number of bytes is 4
    inside read_complete number of bytes is 5
    inside read_complete number of bytes is 6
    inside read_complete number of bytes is 7
    inside read_complete number of bytes is 8
    inside read_complete number of bytes is 9
    inside read_complete number of bytes is 10
    inside read_complete number of bytes is 11
    inside read_complete number of bytes is 12
    inside read_complete number of bytes is 1
    inside read_complete number of bytes is 13
    inside on_read number of bytes is 13
    server echoed our John says hi: OK

     so this yoke is executed until in this case \n is received only then on_read is called since all of the crap has arrived
     *  
     */
    
    size_t read_complete(const boost::system::error_code & err, size_t bytes) {
        //cout << "inside read_complete number of bytes is " << bytes << endl; // ok
        if ( err){
            
            return 0;   
        }
        bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
        // we read one-by-one until we get to enter, no buffering
        return found ? 0 : 1;
 
    }
    
    void on_connect(const error_code & err) {
        if ( !err)
            do_write(message_ + "\n");
        else
            stop();
    }
    
    void on_read(const error_code & err, size_t bytes) {
        //cout << "inside on_read number of bytes is " << bytes << endl; // ok
        if ( !err) {
            std::string copy(read_buffer_, bytes - 1);
            std::cout << "server echoed our " << message_ << ": " << (copy == message_ ? "OK" : "FAIL") << std::endl;
        }
        else{
            cout << "inside on_read error occured:  " << err.message() << endl;
        }
        stop();
    }
    
    void on_write(const error_code & err, size_t bytes) {
        if(err){
            cout << " error occured in on_write " << endl;
        }
        //cout << "inside on_write number of bytes is " << bytes << endl; // ok
        do_read();
    }
    
    /*
     After we're connected, we send the message to the server, do_write() . When the
     write operation is finished, on_write() gets called, which initiates a do_read()
     function. When do_read() is complete, on_read() gets called; here, we simply
     check that the message from the server is simply an echo, and exit from it.
     */
    
    private:
    ip::tcp::socket sock_;
    enum { max_msg = 1024 };
    char read_buffer_[max_msg];
    char write_buffer_[max_msg];
    bool started_;
    std::string message_;
};

int main(int argc, char* argv[]) {
    ip::tcp::endpoint ep( ip::address::from_string("127.0.0.1"), 8001);
    
    /*
    char* messages[] = { "John says hi", "so does James", "Lucy got home", 0};
    for ( char ** message = messages; *message; ++message) {
        talk_to_svr::start( ep, *message);
        boost::this_thread::sleep( boost::posix_time::millisec(100));
    }
    */
    
    std::vector<std::string> messages;
    messages.push_back("John says hi");
    messages.push_back("so does James");
    messages.push_back("Lucy got home");
            
    for ( int i = 0; i < messages.size(); i++) {
        talk_to_svr::start( ep, messages[i]);
        boost::this_thread::sleep( boost::posix_time::millisec(100));
    }
    
    
    service.run();
}