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
#include <boost/make_shared.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/algorithm/string.hpp> // for string splicing
#include <string>

using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;

io_service service;


class peer;
typedef std::vector<boost::shared_ptr<peer>> array; // array of shared pointers to talk_to_client class
array peers;
boost::recursive_mutex clients_cs;

int ping_count = 0;

#define MEM_FN(x)       boost::bind(&self_type::x, shared_from_this())
#define MEM_FN1(x,y)    boost::bind(&self_type::x, shared_from_this(),y)
#define MEM_FN2(x,y,z)  boost::bind(&self_type::x, shared_from_this(),y,z)


void update_peers_changed();

class single_tpm_network_handler{
public: 
    single_tpm_network_handler(int id, int max_iterations): iteration_(0), tpm_id_(id), max_iterations_(max_iterations), partner_tpm(0) {
    
    }
    
   
    int iteration(){
        return iteration_;
    }
    
    int iteration_increase(){
        if(iteration_ == max_iterations_){
            return 0;
        } else{
           iteration_++;
           return 1;
        }
    }
    
    int id(){
        return tpm_id_;
    }
    
    int set_partner(int id){
        partner_tpm = id;
        
        return 1;
    }
    
    int partner(){
        return partner_tpm;
    }
    
private:
    int iteration_;
    int tpm_id_;
    int max_iterations_;
    int partner_tpm; // id of partner tpm
    
};

/*
class tpm_network_handler{

    
    
    
private:
    
    
}
*/

class peer : public boost::enable_shared_from_this<peer>, boost::noncopyable {
    typedef peer self_type;
    peer() : sock_(service), started_(false), timer_(service), peers_changed_(false) {
   
    }
    
public:
    typedef boost::system::error_code error_code;
    typedef boost::shared_ptr<peer> ptr;
    
    void start() {
        { boost::recursive_mutex::scoped_lock lk(clients_cs);
        peers.push_back( shared_from_this());
        }
        
        std::cout << " type of peer " << sock_using_ep << std::endl;
        started_ = true;
        if(sock_using_ep){ // this makes the connection so write straight away
            //endpoint_(ip::address::from_string(ip_address_), ip_port_);
            endpoint_ = boost::make_shared<ip::tcp::endpoint>(ip::address::from_string(ip_address_), ip_port_);
            sock_.async_connect(*endpoint_, MEM_FN1(on_connect,_1));
            
        } else { // this will listen to connection so read.
        //boost::recursive_mutex::scoped_lock lk(cs_);
        do_read();
        }
    }
    
  
    
    static ptr new_(bool type_of_sock) {
        boost::shared_ptr<peer> new_(new peer);
        new_->sock_using_ep = type_of_sock;
        return new_;
    }
    
    static ptr new_(bool type_of_sock, std::string ip_address, int port) {
        boost::shared_ptr<peer> new_(new peer);
        new_->sock_using_ep = type_of_sock;
        new_->ip_address_ = ip_address;
        new_->ip_port_ = port;
        return new_;
    }
    
    void stop() {
        { boost::recursive_mutex::scoped_lock lk(cs_);
        if ( !started_) return;
        started_ = false;
        sock_.close();
        }

        
        boost::shared_ptr<peer> self = shared_from_this();
        { boost::recursive_mutex::scoped_lock lk(clients_cs);
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

    
    void set_peers_changed() { 
       // boost::recursive_mutex::scoped_lock lk(cs_);
        peers_changed_ = true; 
    }
    
private:
    
    
    
    void on_read(const error_code & err, size_t bytes) {
        if ( err) stop();
        if ( !started() ) return;
        
        boost::recursive_mutex::scoped_lock lk(cs_);
        // process the msg
        std::string msg(read_buffer_, bytes);
        msg.pop_back();
        
        
        std::vector<std::string> parsed_msg; 
        boost::split(parsed_msg, msg, [](char c){return c == '\t';});
        
        // partner id \t partners iteration
        if(std::stoi(parsed_msg.at(0)) == 1){ // message type 1;
            std::cout << "message type 1 received" << "with partner id of " << parsed_msg.at(1) << " and iteration " << parsed_msg.at(2) <<  std::endl;
            on_sync(parsed_msg);
        } 
        // init tree parity machines
        else if(std::stoi(parsed_msg.at(0)) == 2){
            std::cout << "message type 2 received" << "with partner id of " << parsed_msg.at(1) << " and iteration " << parsed_msg.at(2) <<  std::endl;
            on_init(parsed_msg);
        }
        // link inited tree parity machines
        else if(std::stoi(parsed_msg.at(0)) == 3){
            std::cout << "message type 3 received" << "with partner id of " << parsed_msg.at(1) << " and iteration " << parsed_msg.at(2) << " and self id of " <<  parsed_msg.at(3) << std::endl;
            on_linking(parsed_msg);
        }
        
        
        else std::cerr << "invalid msg " << msg << std::endl;
         
    }
    
    void on_connect(const error_code & err) {
        
        tpms.push_back(single_tpm_network_handler(1234, 5000));
        
        
        if ( !err){      
            std::stringstream ss;
            ss << "2\t" << tpms.back().id() << "\t" << tpms.back().iteration() << "\n";
            std::cout << "on_connect: " << ss.str() << std::endl;
            do_write(ss.str());
        }
        else{
            std::cout << "Error on_connect:" << err.message() << std::endl;
            stop();
        }
    }
    
    void on_init(std::vector<std::string> & parsed_msg){
        
        srand(123456789); // consistend messages
        
        
        
        tpms.push_back(single_tpm_network_handler(rand()%5000, 5000));
        int index = tpms.size()-1;
        // NB should make sure this id is unique -- will do later
        
        
        tpms[index].set_partner(std::stoi(parsed_msg.at(1)));
        
        
        std::stringstream ss;
        ss << "3\t" << tpms[index].id() << "\t" << tpms[index].iteration() << "\t" << tpms[index].partner() << "\n";
        std::cout << std::stoi(parsed_msg.at(1)) << ";" << parsed_msg.at(1) << " on_init: " << ss.str() << std::endl;
        do_write(ss.str());
  
    }
    
    void on_sync(std::vector<std::string> & parsed_msg){
        bool tpm_found = false;
        int tpm_index = -1;
        for(int i=0; i < tpms.size(); i++){
            if(tpms.at(i).partner() == std::stoi(parsed_msg.at(1))){
                
                tpms.at(i).iteration_increase();
                tpm_found = true;
                tpm_index = i;
                break;
            }
        }
        if(tpm_found){
            std::stringstream ss;
            ss << "1\t" << tpms.at(tpm_index).id() << "\t" << tpms.at(tpm_index).iteration() << "\n";
            std::cout << "on_sync: " << ss.str() << std::endl;
            do_write(ss.str());
        } else{ // must be new machine? but shouldnt be
            
            std::cout << "on_sync no tpm found" << std::endl;
            
        }
        
        
    }
    
    
    
    void on_linking(std::vector<std::string> & parsed_msg){
        bool tpm_found = false;
        int tpm_index = -1;
        
             
        
        for(int i=0; i < tpms.size(); i++){
            if(tpms.at(i).id() == std::stoi(parsed_msg.at(3))){
                tpms.at(i).set_partner(std::stoi(parsed_msg.at(1)));
                tpms.at(i).iteration_increase();
                tpm_found = true;
                tpm_index = i;
                break;
            }
        }
        if(tpm_found){
            std::stringstream ss;
            ss << "1\t" << tpms.at(tpm_index).id() << "\t" << tpms.at(tpm_index).iteration() << "\n";
            std::cout << "on_linking: " << ss.str() << std::endl;
            do_write(ss.str());
        }
        else{ // something is not right
            
            std::cout << "on_linking no tpm found" << std::endl;
            
        }
        
        
    }
    
    
    

    void on_write(const error_code & err, size_t bytes) {
        do_read();
    }
    
    void do_read() {
        async_read(sock_, buffer(read_buffer_), MEM_FN2(read_complete,_1,_2), MEM_FN2(on_read,_1,_2));
        //post_check_ping();
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
    boost::shared_ptr<ip::tcp::endpoint> endpoint_;
    std::string ip_address_;
    int ip_port_;
    enum { max_msg = 1024 };
    char read_buffer_[max_msg];
    char write_buffer_[max_msg];
    bool started_;
    std::string username_;
    deadline_timer timer_;
    boost::posix_time::ptime last_ping_;
    bool peers_changed_;
    bool sock_using_ep;
    std::vector<single_tpm_network_handler> tpms;
};

void update_peers_changed() {
    array copy;
    { boost::recursive_mutex::scoped_lock lk(clients_cs);
      copy = peers;
    }
    for( array::iterator b = copy.begin(), e = copy.end(); b != e; ++b){
        (*b)->set_peers_changed();
    }
}

ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8001));

void handle_accept(peer::ptr peer, const boost::system::error_code & err) {
    peer->start(); // starts current client
    
    // creates and listens for new client
    peer::ptr new_peer = peer::new_(false); // false to accepting connection
    std::cout << "handle_accept run test" << std::endl;
    acceptor.async_accept(new_peer->sock(), boost::bind(handle_accept,new_peer,_1)); // this 
}


boost::thread_group threads;

void listen_thread() {
    service.run();
}

void start_listen(int thread_count) {
    for ( int i = 0; i < thread_count; ++i)
        threads.create_thread( listen_thread);
}


int main(int argc, char* argv[]) {
    peer::ptr initial_peer = peer::new_(false);
    acceptor.async_accept(initial_peer->sock(), boost::bind(handle_accept,initial_peer,_1));
    std::cout << "sending request" << std::endl;
    
    peer::ptr initiating_peer = peer::new_(true, "127.0.0.1", 8002);
    initiating_peer->start();
    
    start_listen(4);
    threads.join_all();
}


