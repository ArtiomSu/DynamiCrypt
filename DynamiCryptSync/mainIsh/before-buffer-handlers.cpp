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

const int MAX_TPMS_PER_PEER = 2;
const int MAX_BUFF = 1024;
class peer;
typedef std::vector<boost::shared_ptr<peer>> array; // array of shared pointers to talk_to_client class
array peers;
boost::recursive_mutex read_lock;

int ping_count = 0;

#define MEM_FN(x)       boost::bind(&self_type::x, shared_from_this())
#define MEM_FN1(x,y)    boost::bind(&self_type::x, shared_from_this(),y)
#define MEM_FN2(x,y,z)  boost::bind(&self_type::x, shared_from_this(),y,z)
#define MEM_FN3(x,y,z,o)  boost::bind(&self_type::x, shared_from_this(),y,z,o)


void update_peers_changed();

class my_read_buffer{
   public:  
       my_read_buffer(){}
       
       char read_buffer_[MAX_BUFF];
};

class my_write_buffer{
   public:  
       my_write_buffer(){}
       
       char write_buffer_[MAX_BUFF];
};

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
    
    void reset(){
        iteration_ = 0;
        
    }
    
private:
    int iteration_;
    int tpm_id_;
    int max_iterations_;
    int partner_tpm; // id of partner tpm
    
};


class tpm_network_handler{
public:
    int max_iterations = 5000;
    tpm_network_handler(){
        srand(time(0)); // consistent messages
    }
    
    int create_new_tpm(){
        int randomId = rand()%5000;
        while(find_tpm(randomId, false) != -1){
            randomId = rand()%5000;
        }
        
        tpm_networks_.push_back(single_tpm_network_handler(randomId, max_iterations));
        std::cout << "created new tpm with id " << randomId << std::endl;
        return randomId;
    }
    
    int find_tpm(int id, bool find_partner){
        for(int i=0; i < tpm_networks_.size(); i++){
            
            if(find_partner){
                if(tpm_networks_[i].partner() == id){

                    return i;
                }
            } else {
                
                if(tpm_networks_[i].id() == id){

                    return i;
                }
                
            }
        }
        return -1;
    }
    
    int increase_iteration(int id){
        int index = find_tpm(id, false);
        if(tpm_networks_[index].iteration_increase()){
            
            return 1;
        }else{
            reset_tpm(index);
            //tpm_networks_.erase(tpm_networks_.begin()+index);
            return 0; 
        }
            
    }
    
    int reset_tpm(int index){
        tpm_networks_[index].reset();
        return 1;
    }
    
    int getid(int index){
        return tpm_networks_[index].id();
          
    }
    
    int get_iteration(int id){
        int index = find_tpm(id, false);
        if(index == -1 ){
            return index;
        }
        else{
            return tpm_networks_[index].iteration();
        }
    }
    
    int set_partner(int id, int partner_id){
        int index = find_tpm(id, false);
        tpm_networks_[index].set_partner(partner_id);
        return 1;
    }
    
    int get_partner(int id){
        int index = find_tpm(id, false);
        if(index == -1) {
            return index;
        }
        return tpm_networks_[index].partner();
        
    }
    
    single_tpm_network_handler* get_tpm(int id){
        int index = find_tpm(id, false);
        if(index != -1){
           return &tpm_networks_[index];
        } else{
            return NULL;
        }
        
    }
    
   
    
    
    
    
private:
    std::vector<single_tpm_network_handler> tpm_networks_;
    
    
};


class peer : public boost::enable_shared_from_this<peer>, boost::noncopyable {
    typedef peer self_type;
    peer() : sock_(service), started_(false), timer_(service), peers_changed_(false) {
        
    }
    
public:
    typedef boost::system::error_code error_code;
    typedef boost::shared_ptr<peer> ptr;
    
    void start() {
        { boost::recursive_mutex::scoped_lock lk(read_lock);
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
        { boost::recursive_mutex::scoped_lock lk(read_lock);
        if ( !started_) return;
        started_ = false;
        sock_.close();
        }

        
        boost::shared_ptr<peer> self = shared_from_this();
        { boost::recursive_mutex::scoped_lock lk(read_lock);
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
    
    
    
    void on_read(const error_code & err, size_t bytes, int buffer_index) {
        if ( err) stop();
        if ( !started() ) return;
        
        { boost::recursive_mutex::scoped_lock lk(read_lock);
        // process the msg
        std::string msg(read_buffer_.at(buffer_index).read_buffer_, bytes);
        msg.pop_back();
        
        std::cout << "index of buffer " << buffer_index << " on read message: " << msg << std::endl;
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
        // reset tree parity machines or stop
        else if(std::stoi(parsed_msg.at(0)) == 4){
            std::cout << "message type 4 received" << "with partner id of " << parsed_msg.at(1) << " and iteration " << parsed_msg.at(2) << " and stop tpm " <<  parsed_msg.at(3) << std::endl;
            on_reset(parsed_msg);
        }
        
        
        else std::cerr << "invalid msg " << msg << std::endl;
        }
    }
    
    void on_connect(const error_code & err) {
        { boost::recursive_mutex::scoped_lock lk(read_lock);
        //for(int b; b< MAX_TPMS_PER_PEER; b++){ //create 10 tpms
        for(int b=0; b< MAX_TPMS_PER_PEER; b++){ //create 10 tpms
            int id = tpm_handler.create_new_tpm();


            if ( !err){      
                std::stringstream ss;
                ss << "2\t" << id << "\t" << tpm_handler.get_iteration(id) << "\n";
                std::cout << "on_connect: " << ss.str() << std::endl;
                do_write(ss.str());
            }
            else{
                std::cout << "Error on_connect:" << err.message() << std::endl;
                stop();
            }
        }
        }
    }
    
    void on_init(std::vector<std::string> & parsed_msg){
        
        { boost::recursive_mutex::scoped_lock lk(read_lock);
        
        
        int id = tpm_handler.create_new_tpm();
        tpm_handler.set_partner(id, std::stoi(parsed_msg.at(1)));
                
        
        std::stringstream ss;
        ss << "3\t" << id << "\t" << tpm_handler.get_iteration(id) << "\t" << tpm_handler.get_partner(id) << "\n";
        std::cout << std::stoi(parsed_msg.at(1)) << ";" << parsed_msg.at(1) << " on_init: " << ss.str() << std::endl;
        do_write(ss.str());
         }
  
    }
    
    void on_sync(std::vector<std::string> & parsed_msg){
        { boost::recursive_mutex::scoped_lock lk(read_lock);
        bool tpm_found = false;
        bool tpm_reset = false;
        int tpm_index = tpm_handler.find_tpm(std::stoi(parsed_msg.at(1)), true);
        int tpm_id = -1;
        if(tpm_index != -1){
            tpm_found = true;
            tpm_id = tpm_handler.getid(tpm_index);
            
            if(!tpm_handler.increase_iteration(tpm_id)){
                tpm_handler.reset_tpm(tpm_index);
                tpm_reset = true;
            }
        }
       
        if(tpm_found){
            std::stringstream ss;
            if(tpm_reset){                                                                     // 1 = stop 
                ss << "4\t" << tpm_id << "\t" << tpm_handler.get_iteration(tpm_id) << "\t" << 0 << "\n";    
            }else{
                ss << "1\t" << tpm_id << "\t" << tpm_handler.get_iteration(tpm_id) << "\n";
            }
            std::cout << "on_sync: " << ss.str() << std::endl;
            do_write(ss.str());
        } else{ // must be new machine? but shouldnt be
            
            std::cout << "on_sync no tpm found" << std::endl;
            
        }
        
         }
    }
    
    
    
    void on_linking(std::vector<std::string> & parsed_msg){
        { boost::recursive_mutex::scoped_lock lk(read_lock);
        bool tpm_found = false;
        bool tpm_reset = false;
        int tpm_index = tpm_handler.find_tpm(std::stoi(parsed_msg.at(1)), false);
        int tpm_id = -1;
        if(tpm_index != -1){
            tpm_found = true;
            tpm_id = tpm_handler.getid(tpm_index);
            if(!tpm_handler.increase_iteration(tpm_id)){
              tpm_handler.reset_tpm(tpm_index);
              tpm_found = true;

            }
            tpm_handler.set_partner(tpm_id, std::stoi(parsed_msg.at(1)));
        }     
        
       
        if(tpm_found){
            std::stringstream ss;
            if(tpm_reset){                                                                     // 1 = stop 
                ss << "4\t" << tpm_id << "\t" << tpm_handler.get_iteration(tpm_id) << "\t" << 0 << "\n";    
            }else{
            ss << "1\t" << tpm_id << "\t" << tpm_handler.get_iteration(tpm_id) << "\n";
            }
            std::cout << "on_linking: " << ss.str() << std::endl;
            do_write(ss.str());
        }
        else{ // something is not right
            
            std::cout << "on_linking no tpm found" << std::endl;
            
        }
         }
        
    }
    
    void on_reset(std::vector<std::string> & parsed_msg){
        { boost::recursive_mutex::scoped_lock lk(read_lock);
        bool tpm_found = false;      
        int tpm_index = tpm_handler.find_tpm(std::stoi(parsed_msg.at(1)), false);
        int tpm_id = -1;
        if(tpm_index != -1){
            tpm_found = true;
            tpm_id = tpm_handler.getid(tpm_index);
            tpm_handler.reset_tpm(tpm_index);
            
            
            tpm_handler.increase_iteration(tpm_id);
            tpm_handler.set_partner(tpm_id, std::stoi(parsed_msg.at(1)));
        }     
        
       
        if(tpm_found){
            std::stringstream ss;
            ss << "1\t" << tpm_id << "\t" << tpm_handler.get_iteration(tpm_id) << "\n";
            std::cout << "on_reset: " << ss.str() << std::endl;
            do_write(ss.str());
        }
        else{ // something is not right
            
            std::cout << "on_reset no tpm found" << std::endl;
            
        }
         }
       
    }
    
 
    
    
    

    void on_write(const error_code & err, size_t bytes) {
        { boost::recursive_mutex::scoped_lock lk(read_lock);
        do_read();
         }
    }
    
    void do_read() {
        { boost::recursive_mutex::scoped_lock lk(read_lock);
        read_buffer_.push_back(my_read_buffer());
        int buffer_index = read_buffer_.size() -1;                      // add the index to these
        async_read(sock_, buffer(read_buffer_.at(buffer_index).read_buffer_), MEM_FN3(read_complete,_1,_2,buffer_index), MEM_FN3(on_read,_1,_2,buffer_index));
         }
        //post_check_ping();
    }
    
    void do_write(const std::string & msg) {
        if ( !started() ) return;
        { boost::recursive_mutex::scoped_lock lk(read_lock);
       // boost::recursive_mutex::scoped_lock lk(cs_);
        write_buffer_.push_back(my_write_buffer());
        int buffer_index = write_buffer_.size() -1;
        std::copy(msg.begin(), msg.end(), write_buffer_.at(buffer_index).write_buffer_);
        sock_.async_write_some( buffer(write_buffer_.at(buffer_index).write_buffer_, msg.size()), MEM_FN2(on_write,_1,_2));
         }
    }
    
    size_t read_complete(const boost::system::error_code & err, size_t bytes, int buffer_index) {
        if ( err) return 0;
        { boost::recursive_mutex::scoped_lock lk(read_lock);
        bool found = std::find(read_buffer_.at(buffer_index).read_buffer_, read_buffer_.at(buffer_index).read_buffer_ + bytes, '\n') < read_buffer_.at(buffer_index).read_buffer_ + bytes;
        // we read one-by-one until we get to enter, no buffering
         
        return found ? 0 : 1;
        }
    }
    
private:
    mutable boost::recursive_mutex cs_;
    ip::tcp::socket sock_;
    boost::shared_ptr<ip::tcp::endpoint> endpoint_;
    std::string ip_address_;
    int ip_port_;
    
    //char read_buffer_[max_msg];
    //char write_buffer_[max_msg];
    std::vector<my_read_buffer> read_buffer_;
    std::vector<my_write_buffer> write_buffer_;
    bool started_;
    std::string username_;
    deadline_timer timer_;
    boost::posix_time::ptime last_ping_;
    bool peers_changed_;
    bool sock_using_ep;
    tpm_network_handler tpm_handler;
};

void update_peers_changed() {
    array copy;
    { boost::recursive_mutex::scoped_lock lk(read_lock);
      copy = peers;
    }
    for( array::iterator b = copy.begin(), e = copy.end(); b != e; ++b){
        (*b)->set_peers_changed();
    }
}

ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8003));

void handle_accept(peer::ptr peer, const boost::system::error_code & err) {
    peer->start(); // starts current client
    
    // creates and listens for new client
    peer::ptr new_peer = peer::new_(false); // false to accepting connection
    std::cout << "handle_accept run test" << std::endl;
    acceptor.async_accept(new_peer->sock(), boost::bind(handle_accept,new_peer,_1)); // this 
}

/*
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
*/
int main(int argc, char* argv[]) {
     peer::ptr initial_peer = peer::new_(false);
    acceptor.async_accept(initial_peer->sock(), boost::bind(handle_accept,initial_peer,_1));
    std::cout << "sending request" << std::endl;
    
    peer::ptr initiating_peer = peer::new_(true, "127.0.0.1", 8001);
    initiating_peer->start();
    service.run();
}