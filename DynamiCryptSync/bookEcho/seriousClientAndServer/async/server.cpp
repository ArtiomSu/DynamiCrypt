#include <cstdlib>
#include <iostream>
#include <boost/array.hpp>
#include <ctime>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
//#include <boost/thread.hpp>  // ? better one?
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <string>

using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;

io_service service;


class talk_to_client;
typedef std::vector<boost::shared_ptr<talk_to_client>> array; // array of shared pointers to talk_to_client class
array peers;
boost::recursive_mutex update_peer_list_lock;

int ping_count = 0;

#define MEM_FN(x)       boost::bind(&self_type::x, shared_from_this())
#define MEM_FN1(x,y)    boost::bind(&self_type::x, shared_from_this(),y)
#define MEM_FN2(x,y,z)  boost::bind(&self_type::x, shared_from_this(),y,z)


void update_peers_changed();

/** simple connection to server:
    - logs in just with username (no password)
    - all connections are initiated by the client: client asks, server answers
    - server disconnects any client that hasn't pinged for 5 seconds
    Possible client requests:
    - gets a list of all connected clients
    - ping: the server answers either with "ping ok" or "ping client_list_changed"
*/

class talk_to_client : public boost::enable_shared_from_this<talk_to_client>, boost::noncopyable {
    typedef talk_to_client self_type;
    talk_to_client() : sock_(service), started_(false), timer_(service), clients_changed_(false) {}
    
public:
    typedef boost::system::error_code error_code;
    typedef boost::shared_ptr<talk_to_client> ptr;
    
    void start() {
        { boost::recursive_mutex::scoped_lock lk(update_peer_list_lock);
        peers.push_back( shared_from_this());
        }
        //boost::recursive_mutex::scoped_lock lk(cs_);
        started_ = true;
        last_ping_ = boost::posix_time::microsec_clock::local_time();
        // first, we wait for client to login
        do_read();
    }
    
    static ptr new_() {
        boost::shared_ptr<talk_to_client> new_(new talk_to_client);
        return new_;
    }
    
    void stop() {
        { boost::recursive_mutex::scoped_lock lk(cs_);
        if ( !started_) return;
        started_ = false;
        sock_.close();
        }

        
        boost::shared_ptr<talk_to_client> self = shared_from_this();
        { boost::recursive_mutex::scoped_lock lk(update_peer_list_lock);
        array::iterator it = std::find(peers.begin(), peers.end(), self);
        peers.erase(it);
        }
        update_peers_changed();
    }
    
    bool started() const { 
       // boost::recursive_mutex::scoped_lock lk(cs_);
        return started_; 
    }
    
    ip::tcp::socket & sock() { 
       // boost::recursive_mutex::scoped_lock lk(cs_);
        return sock_;
    }

    std::string username() const { 
       // boost::recursive_mutex::scoped_lock lk(cs_);
        return username_; 
    }
    
    void set_clients_changed() { 
       // boost::recursive_mutex::scoped_lock lk(cs_);
        clients_changed_ = true; 
    }
    
private:
    void on_read(const error_code & err, size_t bytes) {
        if ( err) stop();
        if ( !started() ) return;
        
        boost::recursive_mutex::scoped_lock lk(cs_);
        // process the msg
        std::string msg(read_buffer_, bytes);
        if ( msg.find("login ") == 0){
            on_login(msg);
        }
        else if ( msg.find("ping") == 0){
            on_ping();
        }
        else if ( msg.find("ask_clients") == 0){
            on_clients();
        }
        else std::cerr << "invalid msg " << msg << std::endl;
    }
    
    void on_login(const std::string & msg) {
       // boost::recursive_mutex::scoped_lock lk(cs_);
        std::istringstream in(msg);
        in >> username_ >> username_;
        std::cout << username_ << " logged in" << std::endl;
        do_write("login ok\n");
        update_peers_changed();
    }
    
    void on_ping() {
       // boost::recursive_mutex::scoped_lock lk(cs_);
        std::cout << "ping gotten " << ping_count << std::endl;
        ping_count++;
        do_write(clients_changed_ ? "ping client_list_changed\n" : "ping ok\n");
        clients_changed_ = false;
    }
    
    void on_clients() {
        array copy;
        { boost::recursive_mutex::scoped_lock lk(update_peer_list_lock);
          copy = peers;
        }
        std::string msg;
        for( array::const_iterator b = copy.begin(), e = copy.end() ; b != e; ++b){
            msg += (*b)->username() + " ";
        }
        do_write("clients " + msg + "\n");
    }

    void do_ping() {
        do_write("ping\n");
    }
    
    void do_ask_clients() {
        do_write("ask_clients\n");
    }

    void on_check_ping() {
       // boost::recursive_mutex::scoped_lock lk(cs_);
        boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
        if ( (now - last_ping_).total_milliseconds() > 5000) {
            std::cout << "stopping " << username_ << " - no ping in time" << std::endl;
            stop();
        }
        last_ping_ = boost::posix_time::microsec_clock::local_time();
    }
    
    void post_check_ping() {
        boost::recursive_mutex::scoped_lock lk(cs_);
        timer_.expires_from_now(boost::posix_time::millisec(5000));
        timer_.async_wait( MEM_FN(on_check_ping));
    }


    void on_write(const error_code & err, size_t bytes) {
        do_read();
    }
    
    void do_read() {
        async_read(sock_, buffer(read_buffer_), MEM_FN2(read_complete,_1,_2), MEM_FN2(on_read,_1,_2));
        post_check_ping();
    }
    
    void do_write(const std::string & msg) {
        if ( !started() ) return;
       // boost::recursive_mutex::scoped_lock lk(cs_);
        std::copy(msg.begin(), msg.end(), write_buffer_);
        sock_.async_write_some( buffer(write_buffer_, msg.size()), MEM_FN2(on_write,_1,_2));
    }
    
    size_t read_complete(const boost::system::error_code & err, size_t bytes) {
        if ( err) return 0;
        bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
        // we read one-by-one until we get to enter, no buffering
        return found ? 0 : 1;
    }
    
private:
    mutable boost::recursive_mutex cs_;
    ip::tcp::socket sock_;
    enum { max_msg = 1024 };
    char read_buffer_[max_msg];
    char write_buffer_[max_msg];
    bool started_;
    std::string username_;
    deadline_timer timer_;
    boost::posix_time::ptime last_ping_;
    bool clients_changed_;
};

void update_peers_changed() {
    array copy;
    { boost::recursive_mutex::scoped_lock lk(update_peer_list_lock);
      copy = peers;
    }
    for( array::iterator b = copy.begin(), e = copy.end(); b != e; ++b){
        (*b)->set_clients_changed();
    }
}

ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8001));

void handle_accept(talk_to_client::ptr client, const boost::system::error_code & err) {
    client->start(); // starts current client
    
    // creates and listens for new client
    talk_to_client::ptr new_client = talk_to_client::new_();
    std::cout << "handle_accept run test" << std::endl;
    acceptor.async_accept(new_client->sock(), boost::bind(handle_accept,new_client,_1)); // this 
}


/* single thread server
int main(int argc, char* argv[]) {
    talk_to_client::ptr client = talk_to_client::new_();
    std::cout << "test" << std::endl;
    acceptor.async_accept(client->sock(), boost::bind(handle_accept,client,_1));
    std::cout << "test2" << std::endl;
    service.run();
}
 */


// multi threaded server bellow

/*
once you go multi-threaded, you will have to think about thread safety.
Even though you call async_* in thread A, its completion routine can be called in
thread B (as long as thread B has called service.run() ). That is not a problem in
itself. As long as you follow the logical flow, that is, from async_read() to on_read(), 
from on_read() to process_request , from process_request to async_write() ,
from async_write() to on_write() , from on_write() to async_read() ,
and there are no public functions that are called on your talk_to_client class,
even though different functions can be called on different threads, they will still
be called in sequential order. Thus, no need for mutexes.
 
This, however, means that for a client, there can be only one asynchronous operation
pending. If at some point, for a client, we have two pending asynchronous functions,
you'll need mutexes. This is because the two pending operations might finish
roughly at the same time, and we could end up having their completion handlers
called simultaneously on two different threads. Therefore, there is a need for thread
safety, and thus, mutexes.
 */

boost::thread_group threads;

void listen_thread() {
    service.run();
}

void start_listen(int thread_count) {
    for ( int i = 0; i < thread_count; ++i)
        threads.create_thread( listen_thread);
}


int main(int argc, char* argv[]) {
    talk_to_client::ptr client = talk_to_client::new_();
    acceptor.async_accept(client->sock(), boost::bind(handle_accept,client,_1));
    start_listen(4);
    threads.join_all();
}

