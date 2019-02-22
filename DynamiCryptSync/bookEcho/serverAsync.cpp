/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
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

io_service service;

class talk_to_client : public boost::enable_shared_from_this<talk_to_client>, boost::noncopyable {
    
    typedef talk_to_client self_type;
    
    talk_to_client() : sock_(service), started_(false) {}
    
public:
    
    typedef boost::system::error_code error_code;
    typedef boost::shared_ptr<talk_to_client> ptr;
    
    void start() {
        started_ = true;
        do_read();
    }
    static ptr new_() {
        ptr new_(new talk_to_client);
        return new_;
    }
    void stop() {
        if ( !started_) return;
            started_ = false;
        sock_.close();
    }
    
    ip::tcp::socket & sock() { return sock_;}
    
    
    //Since we've a very simple Echo server, there is no need for an is_started() function. For each client, just read its message, echo it back, and close it.
    
    void on_read(const error_code & err, size_t bytes) {
        if ( !err) {
            std::string msg(read_buffer_, bytes);
            do_write(msg + "\n");
        }
        stop();
    }
    
    void on_write(const error_code & err, size_t bytes) {
        do_read();
    }
    
    void do_read() { //The do_read() function will make sure that we read a line from the server, at which point on_read() is called.
        async_read(sock_, buffer(read_buffer_), MEM_FN2(read_complete,_1,_2), MEM_FN2(on_read,_1,_2));
    }
    
    size_t read_complete(const boost::system::error_code & err, size_t bytes) {
        //cout << "inside read_complete number of bytes is " << bytes << endl; // ok
        if ( err){
            
            return 0;   
        }
        bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
        // we read one-by-one until we get to enter, no buffering
        return found ? 0 : 1;
 
    }
    

    void do_write(const std::string & msg) {
        std::copy(msg.begin(), msg.end(), write_buffer_);
        //cout << "inside do_write message is " << msg << endl; // works ok
        sock_.async_write_some( buffer(write_buffer_, msg.size()), MEM_FN2(on_write,_1,_2));
    }
    
    private:
    ip::tcp::socket sock_;
    enum { max_msg = 1024 };
    char read_buffer_[max_msg];
    char write_buffer_[max_msg];
    bool started_;
};



ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8001));

void handle_accept(talk_to_client::ptr client, const boost::system::error_code & err){
    client->start();
    talk_to_client::ptr new_client = talk_to_client::new_();
    acceptor.async_accept(new_client->sock(),
    boost::bind(handle_accept,new_client,_1));
}
    
int main(int argc, char* argv[]) {
    talk_to_client::ptr client = talk_to_client::new_();
    acceptor.async_accept(client->sock(),
    boost::bind(handle_accept,client,_1));
    service.run();
}