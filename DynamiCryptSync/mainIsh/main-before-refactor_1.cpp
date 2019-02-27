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
#include <boost/thread/mutex.hpp>
#include <boost/algorithm/string.hpp> // for string splicing
#include <string>

using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;

io_service service;

const int MAX_TPMS_PER_PEER = 2;
const int max_msg = 1024;

class peer;
typedef std::vector<boost::shared_ptr<peer>> array; // array of shared pointers to talk_to_client class
array peers;
typedef std::vector<boost::shared_ptr<tpm_connection>> array_tpm_connection; // array of shared pointers to talk_to_client class
array_tpm_connection tpm_connections;
boost::recursive_mutex update_peer_list_lock;
boost::recursive_mutex update_tpm_connections_lock;
boost::mutex mutex_lock;
const bool USE_PRINT_LOCK = false;

int ping_count = 0;

#define MEM_FN(x)       boost::bind(&self_type::x, shared_from_this())
#define MEM_FN1(x,y)    boost::bind(&self_type::x, shared_from_this(),y)
#define MEM_FN2(x,y,z)  boost::bind(&self_type::x, shared_from_this(),y,z)
#define MEM_FN3(x,y,z,o)  boost::bind(&self_type::x, shared_from_this(),y,z,o)

#define sMEM_FN(x)       boost::bind(&self_type1::x, shared_from_this())
#define sMEM_FN1(x,y)    boost::bind(&self_type1::x, shared_from_this(),y)
#define sMEM_FN2(x,y,z)  boost::bind(&self_type1::x, shared_from_this(),y,z)
#define sMEM_FN3(x,y,z,o)  boost::bind(&self_type1::x, shared_from_this(),y,z,o)

void update_peers_changed();

class buffers_read_write{
    
public:
    buffers_read_write() : buffers_used(false){}
    
        
    //boost::shared_ptr<char[max_msg]> read_buffer_;
    //boost::shared_ptr<char[max_msg]> write_buffer_;
    
    char read_buffer_[max_msg];
    char write_buffer_[max_msg];
    
    bool set_buffers_used(){
        if(buffers_used){
            return false;
        }
        else{
            buffers_used = true;
            return true;
        }
    }
    
    bool reset_buffers(){
        buffers_used = false;
        return true;
    }
    
private:
    bool buffers_used;
    

};

class buffers_handler{
public:
    buffers_handler(){}
    
    int create_buffers(){
        buffers.push_back(buffers_read_write());
        int index = buffers.size() -1;
        if(buffers.at(index).set_buffers_used()){
            return buffers.size() -1 ;
        } else{
            std::cout << " error creating buffer " << std::endl;
            return -1;
        }
    }
    
    int get_buffers(){
        
        for(int i=0; i< buffers.size(); i++){
            if(buffers.at(i).set_buffers_used()){
                return i;
            }
        }
        
        // nothing found so create a new one
        return create_buffers();
        
        
    }
    
    int get_num_buffers(){
        return buffers.size();
    }
    
    //boost::shared_ptr<char[max_msg]> get_read_buffer(int index){
    char* get_read_buffer(int index){
        return buffers.at(index).read_buffer_;
    }
    
    //boost::shared_ptr<char[max_msg]> get_write_buffer(int index){
    char* get_write_buffer(int index){
        return buffers.at(index).write_buffer_;
    }
    
    
    
private:
    std::vector<buffers_read_write> buffers;
    
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
        srand(123456789); // consistent messages
    }
    
    int create_new_tpm(){
        int randomId = rand()%5000;
        while(find_tpm(randomId, false) != -1){
            randomId = rand()%5000;
        }
        
        tpm_networks_.push_back(single_tpm_network_handler(randomId, max_iterations));
        if(USE_PRINT_LOCK)
            mutex_lock.lock();
        std::cout << "created new tpm with id " << randomId << std::endl;
        if(USE_PRINT_LOCK)
            mutex_lock.unlock();
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









class tpm_connection : public boost::enable_shared_from_this<tpm_connection>, boost::noncopyable {
    typedef tpm_connection self_type1;
    tpm_connection() : sock_(service), started_(false), timer_(service){
        
    }
    
public:
    typedef boost::system::error_code error_code;
    typedef boost::shared_ptr<peer> ptr;
    
    void start() {
        { boost::recursive_mutex::scoped_lock lk(update_tpm_connections_lock);
        tpm_connections.push_back( shared_from_this());
        }
        if(USE_PRINT_LOCK)
            mutex_lock.lock();
        
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
        boost::shared_ptr<tpm_connection> new_(new tpm_connection);
        new_->sock_using_ep = type_of_sock;
        return new_;
    }
    
    static ptr new_(bool type_of_sock, std::string ip_address, int port) {
        boost::shared_ptr<tpm_connection> new_(new tpm_connection);
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

        /*
        boost::shared_ptr<tpm_connection> self = shared_from_this();
        { boost::recursive_mutex::scoped_lock lk(update_peer_list_lock);
        array::iterator it = std::find(peers.begin(), peers.end(), self);
        peers.erase(it);
        }
        update_peers_changed();
        */
    }
    
    bool started() const { 
       // boost::recursive_mutex::scoped_lock lk(cs_);
        return started_; 
    }
    
    ip::tcp::socket & sock() { 
       // boost::recursive_mutex::scoped_lock lk(cs_);
        return sock_;
    }

    /*
    void set_peers_changed() { 
       // boost::recursive_mutex::scoped_lock lk(cs_);
        peers_changed_ = true; 
    }
     * */
    
private:
    

    void on_write(const error_code & err, size_t bytes) {
        do_read();
    }
    
    
    // functions using buffers bellow
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
            if(USE_PRINT_LOCK)
                mutex_lock.lock();
            std::cout << "message type 1 received" << "with partner id of " << parsed_msg.at(1) << " and iteration " << parsed_msg.at(2) <<  std::endl;
            if(USE_PRINT_LOCK)
                mutex_lock.unlock();
            on_sync(parsed_msg, buffer_index);
        } 
        // init tree parity machines
        else if(std::stoi(parsed_msg.at(0)) == 2){
            if(USE_PRINT_LOCK)
                mutex_lock.lock();
            std::cout << "message type 2 received" << "with partner id of " << parsed_msg.at(1) << " and iteration " << parsed_msg.at(2) <<  std::endl;
            if(USE_PRINT_LOCK)
                mutex_lock.unlock();
            on_init(parsed_msg, buffer_index);
        }
        // link inited tree parity machines
        else if(std::stoi(parsed_msg.at(0)) == 3){
            if(USE_PRINT_LOCK)
                mutex_lock.lock();
            std::cout << "message type 3 received" << "with partner id of " << parsed_msg.at(1) << " and iteration " << parsed_msg.at(2) << " and self id of " <<  parsed_msg.at(3) << std::endl;
            if(USE_PRINT_LOCK)
                mutex_lock.unlock();
            on_linking(parsed_msg, buffer_index);
        }
        // reset tree parity machines or stop
        else if(std::stoi(parsed_msg.at(0)) == 4){
            if(USE_PRINT_LOCK)
                mutex_lock.lock();
            std::cout << "message type 4 received" << "with partner id of " << parsed_msg.at(1) << " and iteration " << parsed_msg.at(2) << " and stop tpm " <<  parsed_msg.at(3) << std::endl;
            if(USE_PRINT_LOCK)
                mutex_lock.unlock();
            on_reset(parsed_msg, buffer_index);
        }
        
        
        else {
            if(USE_PRINT_LOCK)
                mutex_lock.lock();
            std::cerr << "invalid msg " << msg << std::endl;
            if(USE_PRINT_LOCK)
                mutex_lock.unlock();
        }
        
         
    }
    
    void do_read() {
        
        
        async_read(sock_, buffer(read_buffer_, max_msg), sMEM_FN2(read_complete,_1,_2), sMEM_FN2(on_read,_1,_2,));
        //post_check_ping();
    }
    
    void do_write(const std::string & msg) {
        if ( !started() ) return;
        
       
        std::copy(msg.begin(), msg.end(), write_buffer_);
        sock_.async_write_some( buffer(write_buffer_, msg.size()), sMEM_FN2(on_write,_1,_2));
    }
    
    size_t read_complete(const boost::system::error_code & err, size_t bytes) {
        if ( err) return 0;
        bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
        // we read one-by-one until we get to enter, no buffering
        return found ? 0 : 1;
    }
    
    
    
    
    // different message types
    void on_connect(const error_code & err) {
        
        for(int b; b< MAX_TPMS_PER_PEER; b++){ //create 10 tpms
        
            int id = tpm_handler.create_new_tpm();


            if ( !err){      
                std::stringstream ss;
                ss << "2\t" << id << "\t" << tpm_handler.get_iteration(id) << "\n";
                if(USE_PRINT_LOCK)
                    mutex_lock.lock();
                std::cout << "on_connect: " << ss.str() << std::endl;
                if(USE_PRINT_LOCK)
                    mutex_lock.unlock();
                do_write(ss.str());
            }
            else{
                if(USE_PRINT_LOCK)
                    mutex_lock.lock();
                std::cout << "Error on_connect:" << err.message() << std::endl;
                if(USE_PRINT_LOCK)
                    mutex_lock.unlock();
                stop();
            }
        }
    }
    
    void on_init(std::vector<std::string> & parsed_msg){
        
        
        
        
        //int id = tpm_handler.create_new_tpm();
        tpm_network.set_partner(std::stoi(parsed_msg.at(1)))
        //tpm_handler.set_partner(id, std::stoi(parsed_msg.at(1)));
                
        
        std::stringstream ss;
        ss << "3\t" << id << "\t" << tpm_network.iteration() << "\t" << tpm_network.partner() << "\n";
        if(USE_PRINT_LOCK)
            mutex_lock.lock();
        std::cout << std::stoi(parsed_msg.at(1)) << ";" << parsed_msg.at(1) << " on_init: " << ss.str() << std::endl;
        if(USE_PRINT_LOCK)
            mutex_lock.unlock();
        do_write(ss.str());
  
    }
    
    void on_sync(std::vector<std::string> & parsed_msg){
        
        bool tpm_reset = false;
        if(!tpm_network.iteration_increase()){
            tpm_network.reset();
            tpm_reset = true;
        }

        std::stringstream ss;
        if(tpm_reset){                                                                     // 1 = stop 
            ss << "4\t" << tpm_network.id() << "\t" << tpm_network.iteration() << "\t" << 0 << "\n";    
        }else{
            ss << "1\t" << tpm_network.id() << "\t" << tpm_network.iteration() << "\n";
        }
        if(USE_PRINT_LOCK)
            mutex_lock.lock();
        std::cout << "on_sync: " << ss.str() << std::endl;
        if(USE_PRINT_LOCK)
            mutex_lock.unlock();
        do_write(ss.str());
         
    }
    
    
    
    void on_linking(std::vector<std::string> & parsed_msg){
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
            if(USE_PRINT_LOCK)
                mutex_lock.lock();
            std::cout << "on_linking: " << ss.str() << std::endl;
            if(USE_PRINT_LOCK)
                mutex_lock.unlock();
            do_write(ss.str(), buffer_index);
        }
        else{ // something is not right
            if(USE_PRINT_LOCK)
                mutex_lock.lock();
            std::cout << "on_linking no tpm found" << std::endl;
            if(USE_PRINT_LOCK)
                mutex_lock.unlock();
        }
        
        
    }
    
    void on_reset(std::vector<std::string> & parsed_msg, int buffer_index){
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
            if(USE_PRINT_LOCK)
                mutex_lock.lock();
            std::cout << "on_reset: " << ss.str() << std::endl;
            if(USE_PRINT_LOCK)
                mutex_lock.unlock();
            do_write(ss.str(), buffer_index);
        }
        else{ // something is not right
            if(USE_PRINT_LOCK)
                mutex_lock.lock();
            std::cout << "on_reset no tpm found" << std::endl;
            if(USE_PRINT_LOCK)
                mutex_lock.unlock();
        }
       
    }
    
    
    
    
    
private:
    mutable boost::recursive_mutex cs_;
    ip::tcp::socket sock_;
    boost::shared_ptr<ip::tcp::endpoint> endpoint_;
    std::string ip_address_;
    int ip_port_;
    //enum { max_msg = 1024 };
    char read_buffer_[max_msg];
    char write_buffer_[max_msg];
    bool started_;
    std::string username_;
    deadline_timer timer_;
    boost::posix_time::ptime last_ping_;
    bool peers_changed_;
    bool sock_using_ep;
    //tpm_network_handler tpm_handler;
    single_tpm_network_handler tpm_network;
    //buffers_handler buffers_handler_;
};

























/*
class peer : public boost::enable_shared_from_this<peer>, boost::noncopyable {
    typedef peer self_type;
    peer() : sock_(service), started_(false), timer_(service), peers_changed_(false) {
        
    }
    
public:
    typedef boost::system::error_code error_code;
    typedef boost::shared_ptr<peer> ptr;
    
    void start() {
        { boost::recursive_mutex::scoped_lock lk(update_peer_list_lock);
        peers.push_back( shared_from_this());
        }
        if(USE_PRINT_LOCK)
            mutex_lock.lock();
        std::cout << " type of peer " << sock_using_ep << std::endl;
        if(USE_PRINT_LOCK)
            mutex_lock.unlock();
        started_ = true;
        if(sock_using_ep){ // this makes the connection so write straight away
            //endpoint_(ip::address::from_string(ip_address_), ip_port_);
            endpoint_ = boost::make_shared<ip::tcp::endpoint>(ip::address::from_string(ip_address_), ip_port_);
            sock_.async_connect(*endpoint_, MEM_FN1(on_connect,_1));
            
        } else { // this will listen to connection so read.
        //boost::recursive_mutex::scoped_lock lk(cs_);
        do_read(-1);
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

    
    void set_peers_changed() { 
       // boost::recursive_mutex::scoped_lock lk(cs_);
        peers_changed_ = true; 
    }
    
private:
    

    void on_write(const error_code & err, size_t bytes, int buffer_index) {
        do_read(buffer_index);
    }
    
    
    // functions using buffers bellow
    void on_read(const error_code & err, size_t bytes, int buffer_index) {
        if ( err) stop();
        if ( !started() ) return;
        
        boost::recursive_mutex::scoped_lock lk(cs_);
        // process the msg
        std::string msg(buffers_handler_.get_read_buffer(buffer_index), bytes);
        msg.pop_back();
        
        
        std::vector<std::string> parsed_msg; 
        boost::split(parsed_msg, msg, [](char c){return c == '\t';});
        
        // partner id \t partners iteration
        if(std::stoi(parsed_msg.at(0)) == 1){ // message type 1;
            if(USE_PRINT_LOCK)
                mutex_lock.lock();
            std::cout << "message type 1 received" << "with partner id of " << parsed_msg.at(1) << " and iteration " << parsed_msg.at(2) <<  std::endl;
            if(USE_PRINT_LOCK)
                mutex_lock.unlock();
            on_sync(parsed_msg, buffer_index);
        } 
        // init tree parity machines
        else if(std::stoi(parsed_msg.at(0)) == 2){
            if(USE_PRINT_LOCK)
                mutex_lock.lock();
            std::cout << "message type 2 received" << "with partner id of " << parsed_msg.at(1) << " and iteration " << parsed_msg.at(2) <<  std::endl;
            if(USE_PRINT_LOCK)
                mutex_lock.unlock();
            on_init(parsed_msg, buffer_index);
        }
        // link inited tree parity machines
        else if(std::stoi(parsed_msg.at(0)) == 3){
            if(USE_PRINT_LOCK)
                mutex_lock.lock();
            std::cout << "message type 3 received" << "with partner id of " << parsed_msg.at(1) << " and iteration " << parsed_msg.at(2) << " and self id of " <<  parsed_msg.at(3) << std::endl;
            if(USE_PRINT_LOCK)
                mutex_lock.unlock();
            on_linking(parsed_msg, buffer_index);
        }
        // reset tree parity machines or stop
        else if(std::stoi(parsed_msg.at(0)) == 4){
            if(USE_PRINT_LOCK)
                mutex_lock.lock();
            std::cout << "message type 4 received" << "with partner id of " << parsed_msg.at(1) << " and iteration " << parsed_msg.at(2) << " and stop tpm " <<  parsed_msg.at(3) << std::endl;
            if(USE_PRINT_LOCK)
                mutex_lock.unlock();
            on_reset(parsed_msg, buffer_index);
        }
        
        
        else {
            if(USE_PRINT_LOCK)
                mutex_lock.lock();
            std::cerr << "invalid msg " << msg << std::endl;
            if(USE_PRINT_LOCK)
                mutex_lock.unlock();
        }
        
         
    }
    
    void do_read(int buffer_index) {
        if(sock_using_ep == false){ // create buffers here
            if((buffer_index == -1) || (buffers_handler_.get_num_buffers() <= MAX_TPMS_PER_PEER)){
                buffer_index = buffers_handler_.create_buffers();
            if(USE_PRINT_LOCK)
                mutex_lock.lock();
            std::cout << " do_read creating buffers with index " << buffer_index << std::endl;
            if(USE_PRINT_LOCK)
                mutex_lock.unlock();
            }
            
 
        }
        if(USE_PRINT_LOCK)
            mutex_lock.lock();
        std::cout << " reading with buffer index " << buffer_index << std::endl;
        if(USE_PRINT_LOCK)
            mutex_lock.unlock();
        async_read(sock_, buffer(buffers_handler_.get_read_buffer(buffer_index), max_msg), MEM_FN3(read_complete,_1,_2,buffer_index), MEM_FN3(on_read,_1,_2,buffer_index));
        //post_check_ping();
    }
    
    void do_write(const std::string & msg, int buffer_index) {
        if ( !started() ) return;
        if(sock_using_ep && buffer_index == -1){ //create new buffers
            buffer_index = buffers_handler_.create_buffers();
        }
       // boost::recursive_mutex::scoped_lock lk(cs_);
        if(USE_PRINT_LOCK)
            mutex_lock.lock();
        std::cout << " writing with buffer index " << buffer_index << std::endl; 
        if(USE_PRINT_LOCK)
            mutex_lock.unlock();
        std::copy(msg.begin(), msg.end(), buffers_handler_.get_write_buffer(buffer_index));
        sock_.async_write_some( buffer(buffers_handler_.get_write_buffer(buffer_index), msg.size()), MEM_FN3(on_write,_1,_2,buffer_index));
    }
    
    size_t read_complete(const boost::system::error_code & err, size_t bytes, int buffer_index) {
        if ( err) return 0;
        bool found = std::find(buffers_handler_.get_read_buffer(buffer_index), buffers_handler_.get_read_buffer(buffer_index) + bytes, '\n') < buffers_handler_.get_read_buffer(buffer_index) + bytes;
        // we read one-by-one until we get to enter, no buffering
        return found ? 0 : 1;
    }
    
    
    
    
    // different message types
    void on_connect(const error_code & err) {
        
        for(int b; b< MAX_TPMS_PER_PEER; b++){ //create 10 tpms
        
            int id = tpm_handler.create_new_tpm();


            if ( !err){      
                std::stringstream ss;
                ss << "2\t" << id << "\t" << tpm_handler.get_iteration(id) << "\n";
                if(USE_PRINT_LOCK)
                    mutex_lock.lock();
                std::cout << "on_connect: " << ss.str() << std::endl;
                if(USE_PRINT_LOCK)
                    mutex_lock.unlock();
                do_write(ss.str(), -1);
            }
            else{
                if(USE_PRINT_LOCK)
                    mutex_lock.lock();
                std::cout << "Error on_connect:" << err.message() << std::endl;
                if(USE_PRINT_LOCK)
                    mutex_lock.unlock();
                stop();
            }
        }
    }
    
    void on_init(std::vector<std::string> & parsed_msg, int buffer_index){
        
        
        
        
        int id = tpm_handler.create_new_tpm();
        tpm_handler.set_partner(id, std::stoi(parsed_msg.at(1)));
                
        
        std::stringstream ss;
        ss << "3\t" << id << "\t" << tpm_handler.get_iteration(id) << "\t" << tpm_handler.get_partner(id) << "\n";
        if(USE_PRINT_LOCK)
            mutex_lock.lock();
        std::cout << std::stoi(parsed_msg.at(1)) << ";" << parsed_msg.at(1) << " on_init: " << ss.str() << std::endl;
        if(USE_PRINT_LOCK)
            mutex_lock.unlock();
        do_write(ss.str(), buffer_index);
  
    }
    
    void on_sync(std::vector<std::string> & parsed_msg, int buffer_index){
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
            if(USE_PRINT_LOCK)
                mutex_lock.lock();
            std::cout << "on_sync: " << ss.str() << std::endl;
            if(USE_PRINT_LOCK)
                mutex_lock.unlock();
            do_write(ss.str(), buffer_index);
        } else{ // must be new machine? but shouldnt be
            if(USE_PRINT_LOCK)
                mutex_lock.lock();
            std::cout << "on_sync no tpm found" << std::endl;
            if(USE_PRINT_LOCK)
                mutex_lock.unlock();
        }
        
        
    }
    
    
    
    void on_linking(std::vector<std::string> & parsed_msg, int buffer_index){
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
            if(USE_PRINT_LOCK)
                mutex_lock.lock();
            std::cout << "on_linking: " << ss.str() << std::endl;
            if(USE_PRINT_LOCK)
                mutex_lock.unlock();
            do_write(ss.str(), buffer_index);
        }
        else{ // something is not right
            if(USE_PRINT_LOCK)
                mutex_lock.lock();
            std::cout << "on_linking no tpm found" << std::endl;
            if(USE_PRINT_LOCK)
                mutex_lock.unlock();
        }
        
        
    }
    
    void on_reset(std::vector<std::string> & parsed_msg, int buffer_index){
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
            if(USE_PRINT_LOCK)
                mutex_lock.lock();
            std::cout << "on_reset: " << ss.str() << std::endl;
            if(USE_PRINT_LOCK)
                mutex_lock.unlock();
            do_write(ss.str(), buffer_index);
        }
        else{ // something is not right
            if(USE_PRINT_LOCK)
                mutex_lock.lock();
            std::cout << "on_reset no tpm found" << std::endl;
            if(USE_PRINT_LOCK)
                mutex_lock.unlock();
        }
       
    }
    
    
    
    
    
private:
    mutable boost::recursive_mutex cs_;
    ip::tcp::socket sock_;
    boost::shared_ptr<ip::tcp::endpoint> endpoint_;
    std::string ip_address_;
    int ip_port_;
    //enum { max_msg = 1024 };
    //char read_buffer_[max_msg];
    //char write_buffer_[max_msg];
    bool started_;
    std::string username_;
    deadline_timer timer_;
    boost::posix_time::ptime last_ping_;
    bool peers_changed_;
    bool sock_using_ep;
    tpm_network_handler tpm_handler;
    buffers_handler buffers_handler_;
};
*/
void update_peers_changed() {
    array copy;
    { boost::recursive_mutex::scoped_lock lk(update_peer_list_lock);
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
    if(USE_PRINT_LOCK)
            mutex_lock.lock();
    std::cout << "handle_accept run test" << std::endl;
    if(USE_PRINT_LOCK)
            mutex_lock.unlock();
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
    
    peer::ptr initiating_peer = peer::new_(true, "127.0.0.1", 8001);
    initiating_peer->start();
    
    start_listen(4);
    threads.join_all();
}

