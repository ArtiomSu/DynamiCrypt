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
#include <boost/thread/recursive_mutex.hpp>
#include <string>

using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;

io_service service;

/*
Each talk_to_client instance holds a socket. The socket class is not copy-constructible, 
thus, if you want to hold it into an std::vector function, you need to
hold a shared pointer to it. There are two ways to go about this: either inside talk_to_client
hold a shared pointer to a socket and then have an array of talk_to_client instances, 
or have the talk_to_client instance hold a the socket by value,
and have an array of shared pointers to talk_to_client . I chose the latter, but you
can go either way
*/ 
struct peer;
typedef boost::shared_ptr<peer> client_ptr;
typedef std::vector<client_ptr> array;
array peers;
boost::recursive_mutex cs; // thread-safe access to clients array

void update_peers_changed();
/*
The synchronous server is quite simple as well. It needs two threads, one for
listening to new clients and one for processing existing clients. It cannot use a single
thread; waiting for a new client is a blocking operation, thus, we need an extra
thread to handle the existing clients.
 */

struct talk_to_client : boost::enable_shared_from_this<peer> {
    
    talk_to_client() : sock_(service), started_(false), already_read_(0) {
		last_ping = boost::posix_time::microsec_clock::local_time();
    }
    
    std::string username() const { return username_; }
    
    void answer_to_client() {
        try {
            read_request();
            process_request();
        } catch ( boost::system::system_error&) {
            stop();
        }
        if ( timed_out()){
            std::cout << "stopping " << username_ << " - no ping in time" << std::endl;
            stop();
        }
    }
    
    void set_clients_changed() { clients_changed_ = true; }
    
    ip::tcp::socket & sock() { return sock_; }
    
    bool timed_out() const {
        boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
        long long ms = (now - last_ping).total_milliseconds();
        return ms > 5000 ;
    }
    
    void stop() {
        boost::system::error_code err; 
        sock_.close(err);
    }
    
    void read_request() { // This will read only if there's data available, thus, the server never gets blocked:
        if ( sock_.available())
            already_read_ += sock_.read_some( buffer(buff_ + already_read_, max_msg - already_read_));
    }
  
    /*
    Take a look at process_request() . After we've read only as much as was available,
    we need to know if we read the full message (if found_enter is true). If so, we're
    protecting ourselves against maybe reading more than one message (anything after
    '\n' is saved in the buffer), and then we interpret the fully read message. The rest
    of the code is straightforward.
     */
    void process_request() {
        bool found_enter = std::find(buff_, buff_ + already_read_, '\n') < buff_ + already_read_;
        if ( !found_enter)
            return; // message is not full
        // process the msg
        last_ping = boost::posix_time::microsec_clock::local_time();
        size_t pos = std::find(buff_, buff_ + already_read_, '\n') - buff_;
        std::string msg(buff_, pos);
        std::copy(buff_ + already_read_, buff_ + max_msg, buff_);
        already_read_ -= pos + 1;
        if ( msg.find("login ") == 0) on_login(msg);
        else if ( msg.find("ping") == 0) on_ping();
        else if ( msg.find("ask_clients") == 0) on_clients();
        else std::cerr << "invalid msg " << msg << std::endl;
       
    }
    
    
    void on_login(const std::string & msg) {
        std::istringstream in(msg);
        in >> username_ >> username_;
        std::cout << username_ << " logged in" << std::endl;
        std::string s = "login ok\n";
        write(s);
        update_peers_changed();
    }
    
    void on_ping() {
        write(clients_changed_ ? "ping client_list_changed\n" : "ping ok\n");
        clients_changed_ = false;
    }
    
    void on_clients() {
        std::string msg;
        { boost::recursive_mutex::scoped_lock lk(cs);
            for( array::const_iterator b = peers.begin(), e = peers.end(); b != e; ++b)
                msg += (*b)->username() + " ";
        }
        write("clients " + msg + "\n");
    }
    
    void write(const std::string & msg) { sock_.write_some(buffer(msg)); }

    
    private:
    ip::tcp::socket sock_;
    enum { max_msg = 1024 };
    int already_read_;
    char buff_[max_msg];
    bool started_;
    std::string username_;
    bool clients_changed_;
    boost::posix_time::ptime last_ping;
};


void update_peers_changed() {
	boost::recursive_mutex::scoped_lock lk(cs);
	for (array::iterator b = peers.begin(), e = peers.end(); b != e; ++b)
		(*b)->set_peers_changed();
}



void accept_thread() {
    ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8001));
    while ( true) {
        client_ptr new_( new peer);
        acceptor.accept(new_->sock());
        boost::recursive_mutex::scoped_lock lk(cs);
        peers.push_back(new_);
    }
}


void handle_clients_thread() {
    while ( true) {
        boost::this_thread::sleep( boost::posix_time::millisec(1));
        boost::recursive_mutex::scoped_lock lk(cs);
        for(array::iterator b = peers.begin(),e = peers.end(); b != e; ++b)
            (*b)->answer_to_client();
        // erase clients that timed out
        peers.erase(std::remove_if(peers.begin(), peers.end(), boost::bind(&peer::timed_out,_1)), peers.end());
    }
}

int main(int argc, char* argv[]) {
    boost::thread_group threads;
    threads.create_thread(accept_thread);
    threads.create_thread(handle_clients_thread);
    threads.join_all();
}