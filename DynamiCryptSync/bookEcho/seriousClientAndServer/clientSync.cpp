/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


/*
 
• The client logs in to the server with a username (no password)
• All connections are initiated by the client, where client asks and server answers
• All requests and answers are finished with a line feed ( '\n' )
• Server disconnects any client that hasn't pinged for 5 seconds

  The client can make the following requests:
• Get a list of all connected clients
• The client can ping, and when it pings, the server answers either with ping
  ok or ping client_list_chaned (in the latter case, the client re-requests the
  list of connected clients)
 
  To keep things interesting, we'll add a few twists:
• Each client application logs in six with user connections, such as John, James,
  Lucy, Tracy, Frank, Abby
• Each client connection pings the server at random times (random of 7
  seconds; thus, every now and then, the server will drop a connection)
 
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


struct talk_to_svr {
    
    talk_to_svr(const std::string & username) : sock_(service), started_(true), username_(username) {}

    void connect(ip::tcp::endpoint ep) {
        sock_.connect(ep);
    }
    /*
     we just put a bit and a ping to sleep, and read the server's answer.
     We put it to sleep at random (sometimes over 5 seconds), so that the server will
     disconnect us at some point
     */
    void loop() {
        write("login " + username_ + "\n");
        read_answer();
        while ( started_) {
            write_request();
            read_answer();
            int millis = rand() * 1425 % 7000;
            std::cout << username_ << " postpone ping " << millis << " ms" << std::endl;
            boost::this_thread::sleep(boost::posix_time::millisec(millis));
        }
    }
    
    std::string username() const { return username_; }
    
    void write_request() {
        write("ping\n");
    }
    
    void read_answer() {
        already_read_ = 0;
        read(sock_, buffer(buff_),
        boost::bind(&talk_to_svr::read_complete, this, _1, _2));
        process_msg();
    }
    
    void process_msg() {
        std::string msg(buff_, already_read_);
        //cout << msg << endl;
        if ( msg.find("login ") == 0){ 
            cout << " process_msg login " << endl;
            on_login();
        }
        else if ( msg.find("ping") == 0){
            cout << " process_msg ping " << endl;
            on_ping(msg);
        }
        else if ( msg.find("clients ") == 0){
            cout << " process_msg clients " << endl;
            on_clients(msg);
        }
        else std::cerr << "invalid msg " << msg << std::endl;
    }
    
    /*
     For reading the answer, we use read_complete , explained extensively in the previous
     chapter, to make sure that we read up to the line feed ( '\n' ). The logic is in process_msg()
     where we read the client's answer, and dispatch to the right function:
     */
    
    void on_login() { 
        std::cout << username_ << " logged in" << std::endl;

        do_ask_clients(); 
    }
    
    // When reading the server's answer to our ping, if we get client_list_changed , we ask again for the list of clients.
    void on_ping(const std::string & msg) {
        std::istringstream in(msg);
        std::string answer;
        in >> answer >> answer;
        if ( answer == "client_list_changed")
            do_ask_clients();
    }
    
    void on_clients(const std::string & msg) {
        std::string clients = msg.substr(8);
        std::cout << username_ << ", new client list:" << clients;
    }
    
    void do_ask_clients() {
        write("ask_clients\n");
        read_answer();
    }
    
    void write(const std::string & msg){ 
        sock_.write_some(buffer(msg)); 
    }
    
    size_t read_complete(const boost::system::error_code & err, size_t bytes) {
        //cout << "inside read_complete number of bytes is " << bytes << endl; // ok
        if ( err){
            
            return 0;   
        }
        bool found = std::find(buff_, buff_ + bytes, '\n') < buff_ + bytes;
        // we read one-by-one until we get to enter, no buffering
        return found ? 0 : 1;
    }
    
    
    private:
    ip::tcp::socket sock_;
    enum { max_msg = 1024 };
    int already_read_;
    char buff_[max_msg];
    bool started_;
    std::string username_;
};






void run_client(const std::string & client_name) {
    talk_to_svr client(client_name);
    try {
        client.connect(ep);
        client.loop();
    } catch(boost::system::system_error & err) {
        std::cout << "client terminated " << client.username() << ": " << err.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    boost::thread_group threads;
    
    std::vector<std::string> names;
    names.push_back("John");
    names.push_back("James");
    names.push_back("Lucy");
    names.push_back("Tracy");
    names.push_back("Frank");
    names.push_back("Abby");
    
    for ( int i = 0; i < names.size(); i++) {
        threads.create_thread( boost::bind(run_client, names[i]));
        boost::this_thread::sleep( boost::posix_time::millisec(100));
    }
    threads.join_all();
}