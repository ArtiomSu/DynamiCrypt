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
#include <boost/program_options.hpp>
#include <iterator>
#include <string>
#include <mutex>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>


#include "tpminputvector.h"
#include "dynamicarray.h"
#include "treeparitymachine.h"
#include "tpmhandler.h"

//g++ main.cpp -lboost_system -lpthread -lboost_thread -lboost_program_options -o sync

using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;

io_service service;

const int MAX_TPMS_PER_PEER = 1;
const int MAX_BUFF = 1024;
class peer;
typedef std::vector<boost::shared_ptr<peer>> array; // array of shared pointers to talk_to_client class
array peers;
boost::recursive_mutex read_lock;
boost::recursive_mutex write_lock;

int ping_count = 0;

const int PRINT_SYNC_MESSAGES =1;

#define MEM_FN(x)       boost::bind(&self_type::x, shared_from_this())
#define MEM_FN1(x,y)    boost::bind(&self_type::x, shared_from_this(),y)
#define MEM_FN2(x,y,z)  boost::bind(&self_type::x, shared_from_this(),y,z)
#define MEM_FN3(x,y,z,o)  boost::bind(&self_type::x, shared_from_this(),y,z,o)


void update_peers_changed();

class single_tpm_network_handler{
public: 
    single_tpm_network_handler(int id): iteration_(0), tpm_id_(id), partner_tpm(0), check_for_key_counter(0) {
       int initK = 8;  // k is hidden neurons	
       int initN = 10; // n is input neurons
       int initL = 6;	// range of weights	
        
       tpm.K = initK;
       tpm.N = initN;
       tpm.L = initL;
       
       tpm.Initialize();
       tpm.RandomWeight();
        
       max_iterations_ = (tpm.L*tpm.L*tpm.L*tpm.L)*tpm.N*tpm.K;
       
       tmpinputvector.xLength(tpm.K, tpm.N);
       
       
       
    }
    
   
    int iteration(){
        return iteration_;
    }
    
    int increase_key_counter(){
        check_for_key_counter = check_for_key_counter+ 1;
        return check_for_key_counter;
    }
    
    int iteration_increase(){
        // 0 = too high need to reset tpm
        // 1 = everything ok continue,
        // ? = perform hash and check key
        
        int return_type = 0;
        if(iteration_ == max_iterations_){
            return_type = 0;
        } else{
           iteration_++;
           return_type = 1;
        }
        /*if(check_for_key_counter == 1000){
            const char Dictionary [] = "0123456789_abcdefghijklmnopqrstuvwxyz";
            int key_size = (sizeof(Dictionary)-1) / (tpm.L * 2 + 1);
            int key_length = tpm.K * tpm.N / key_size;
            DynamicArray<char> key;
            key.length(key_length + 1);
	
            for(int i = 0; i < key_length; i++){
                key.Z[i] = 0;
            }
	
            std::stringstream ss;
            for (int i=1; i < key_length+1; i++) {
                int K = 1;
                for(int j=(i-1)*key_size; j<i*key_size;j++){
                    K = K + tpm.W.Z[j] + tpm.L;
                }
                key.Z[i-1]=Dictionary[K];
                ss << key.Z[i-1];
            }

            key.Z[key_length]='\0'; 
            ss << "\0";
            
            check_for_key_counter = 0;
            
            std::cout << "Key right now is: " << ss.str() << std::endl;
        }
        */
        
        return return_type;
        
    }
    
    std::string get_key(){
        const char Dictionary [] = "0123456789_abcdefghijklmnopqrstuvwxyz";
            int key_size = (sizeof(Dictionary)-1) / (tpm.L * 2 + 1);
            int key_length = tpm.K * tpm.N / key_size;
            std::cout << "key 1" << std::endl;
            DynamicArray<char> key;
            key.length(key_length + 1);
            std::cout << "key 2" << std::endl;
            for(int i = 0; i < key_length; i++){
                key.Z[i] = 0;
            }
            std::cout << "key 3" << std::endl;
            std::stringstream ss;
            for (int i=1; i < key_length+1; i++) {
                int K = 1;
                for(int j=(i-1)*key_size; j<i*key_size;j++){
                  //  std::cout << "key 4" << std::endl;
                    K = K + tpm.W.Z[j] + tpm.L;
                }
                //std::cout << "key 5" << std::endl;
                key.Z[i-1]=Dictionary[K];
                ss << key.Z[i-1];
            }
            std::cout << "key 6" << std::endl;
            key.Z[key_length]='\0'; 
            std::cout << "key 7" << std::endl;
            ss << "\0";
            
            //check_for_key_counter = 0;
            return ss.str();
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
        check_for_key_counter = 0;
        tpm.RandomWeight();
    }
    
    void reset_check_for_key_counter(){
        check_for_key_counter = 0;
    }
    
    TPMInputVector* get_tmpinputvector(){
        return &tmpinputvector;
    }
    
    std::string create_random_input_vector(){
        tmpinputvector.CreateRandomVector(tpm.K, tpm.N);
        return tmpinputvector.to_string(tpm.K, tpm.N);
    }
    
    int set_random_input_vector_from_string(std::string input){
        tmpinputvector.set_from_string(input, tpm.K, tpm.N);
        return 1;
    }
    
    int compute_tpm_result(){
        tpm.ComputeTPMResult(tmpinputvector.X);
        return tpm.TPMOutput;
    }
    
    void update_weight(){
        //std::cout << 
        tpm.UpdateWeight(tmpinputvector.X);
    }
    
    int get_max_iterations(){
        return max_iterations_;
    }
    
private:
    int iteration_;
    int tpm_id_;
    int max_iterations_;
    int partner_tpm; // id of partner tpm
    TreeParityMachine tpm;
    TPMInputVector tmpinputvector;
    int check_for_key_counter;
};


class tpm_network_handler{
public:
    tpm_network_handler(){
        //srand(time(0)); // consistent messages
    }
    
    int create_new_tpm(){
        int randomId = rand()%5000;
        while(find_tpm(randomId, false) != -1){
            randomId = rand()%5000;
        }
        
        tpm_networks_.push_back(single_tpm_network_handler(randomId));
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
    
    int get_max_iterations(int index){
        return tpm_networks_[index].get_max_iterations();
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
    
    int increase_key_counter(int index){
        return tpm_networks_[index].increase_key_counter();
          
    }
    
    std::string get_key_tpm(int index){
        return tpm_networks_[index].get_key();
    }
    
   
    // when initially synced using on_linking and on_reset
    std::string sync_tpm_message_one(int tpm_id){
        std::stringstream ss;
        int index = find_tpm(tpm_id, false);
        
        std::string random_input_vector = tpm_networks_[index].create_random_input_vector();
        int tpm_result = tpm_networks_[index].compute_tpm_result();
        
        // id, itteration, randomVector, resultOfVector
        ss << "1\t" << tpm_id << "\t" << tpm_networks_[index].id() << "\t" << tpm_networks_[index].iteration() << "\t" << random_input_vector << "\t" << tpm_result << "\t" << 1 << "\n";
        //     0         1                       2                                       3                                      4                           5                  6 messagetype to process
        // parsed message ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        
        std::cout << "sync_tpm_message_one end" << std::endl;
        return ss.str();
        
        // sends random vector and result to said random vector but doesn't update weights.
        
    }
    
    // when  synced using sync
    std::string sync_tpm_message_one_advanced(int tpm_id, std::vector<std::string> & parsed_msg){
        // receives random vector and partners result to that vector 
        // use that vector and test ouput
        // if output == this output then update weights 
        // tell the other machine to update too by using some variable
        
        // then for this itteration create random vector, compute result 
        // send the new vector and new result too.
        
        // at the end send
        // old_random_vector, tell_machine_to_update_or_not_using_the_old_vector, new_vector, new_TPM_output.
        
        // maybe dont send old vector as it is not really needed
        
        std::stringstream ss;
        int index = find_tpm(tpm_id, false);
        int tell_machine_to_update = 0;
        int tpm_result = 0;
        int message_type_to_process = 0;
        std::string random_input_vector;
        std::string old_input_vector;
        
        int processing_message_type_now = std::stoi(parsed_msg.at(6));
        
        
        std::string key;
        
        if(processing_message_type_now == 1){ // dont update ur weights straight away
            std::string random_vector_received = parsed_msg.at(4);
            old_input_vector = random_vector_received;
            int result_received = std::stoi(parsed_msg.at(5));
            tpm_networks_[index].set_random_input_vector_from_string(random_vector_received);
            int tpm_result_old = tpm_networks_[index].compute_tpm_result();
                    
            if(tpm_result_old == result_received){
                tpm_networks_[index].update_weight();
                tell_machine_to_update = 1;
            }        
        
            
            random_input_vector = tpm_networks_[index].create_random_input_vector();
            tpm_result = tpm_networks_[index].compute_tpm_result();
            
            std::cout << "sync_tpm_message_one_advanced-1 before getKey" << std::endl;
            message_type_to_process = 2;
            key = tpm_networks_[index].get_key();
            std::cout << "sync_tpm_message_one_advanced-1 end" << std::endl;
            //std::cout << "\n\nok\n"; 
        } 
        
        // update on old weight which should be still present in the tpm
                
        else if(processing_message_type_now == 2) {
            
            tell_machine_to_update = std::stoi(parsed_msg.at(7));
            //std::cout << "\n\nokkkk\n"; 
            if(tell_machine_to_update == 1){
                old_input_vector = parsed_msg.at(8);
                tpm_networks_[index].set_random_input_vector_from_string(old_input_vector);
                //std::cout << "\n\tell_machine_to_update\n";
                tpm_networks_[index].update_weight();
                 
                std::string other_key = parsed_msg.at(9);
                key = tpm_networks_[index].get_key();
                
                if (!key.compare(other_key)) {
                    // keys are the same
                    std::cout << "found same key: " << key << std::endl;
                    std::exit(1);
                }
                
            }
            
            
            
            
            tell_machine_to_update = 0;
            
            std::string random_vector_received = parsed_msg.at(4);
            old_input_vector = random_vector_received;
          
            int result_received = std::stoi(parsed_msg.at(5));
            
            tpm_networks_[index].set_random_input_vector_from_string(random_vector_received);
            int tpm_result_old = tpm_networks_[index].compute_tpm_result();
                    
            if(tpm_result_old == result_received){
                tpm_networks_[index].update_weight();
                tell_machine_to_update = 1;
            }        
            

           // then continue as normal and send back the result for new vector
            random_input_vector = tpm_networks_[index].create_random_input_vector();
            tpm_result = tpm_networks_[index].compute_tpm_result();
            key = tpm_networks_[index].get_key();
            // send id, id again? lol, iteration, reandom-vector, tpm result,   tell machine to update doesnt matter, message type should be 1 since this is basically the same as sync_tpm_message_one
            message_type_to_process = 2;
            
        }
        
        
       
        // mostly compatible with sync_tpm_message_one message.
        ss << "1\t" << tpm_id << "\t" << tpm_networks_[index].id() << "\t" << tpm_networks_[index].iteration() <<  "\t" << random_input_vector << "\t" << tpm_result << "\t" << message_type_to_process  << "\t" << tell_machine_to_update<< "\t" << old_input_vector << "\t" << key << "\n";
        //     0         1                       2                                       3                                      4                           5                                   6                          7                               8                     9
        // parsed message ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        
        
        return ss.str();
        
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
    
    
    
    void on_read(const error_code & err, size_t bytes) {
        if ( err) stop();
        if ( !started() ) return;
        
      //  { boost::recursive_mutex::scoped_lock lk(read_lock);
        // process the msg
        std::string msg(read_buffer_, bytes);
        msg.pop_back();
        
        std::vector<std::string> parsed_msg; 
        boost::split(parsed_msg, msg, [](char c){return c == '\t';});
        
        // partner id \t partners iteration
        if(std::stoi(parsed_msg.at(0)) == 1){ // message type 1;
            if(PRINT_SYNC_MESSAGES){
                std::cout << "message type 1 received" << "with partner id of " << parsed_msg.at(1) << " and iteration " << parsed_msg.at(3) <<  std::endl;
            }
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
       // }
    }
    
    void on_connect(const error_code & err) {
      //  { boost::recursive_mutex::scoped_lock lk(read_lock);
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
       // }
    }
    
    void on_init(std::vector<std::string> & parsed_msg){
        
      //  { boost::recursive_mutex::scoped_lock lk(read_lock);
        
        
        int id = tpm_handler.create_new_tpm();
        tpm_handler.set_partner(id, std::stoi(parsed_msg.at(1)));
                
        
        std::stringstream ss;
        ss << "3\t" << id << "\t" << tpm_handler.get_iteration(id) << "\t" << tpm_handler.get_partner(id) << "\n";
        std::cout << " on_init: " << ss.str() << std::endl;
        do_write(ss.str());
        // }
  
    }
    
    void on_sync(std::vector<std::string> & parsed_msg){
      //  { boost::recursive_mutex::scoped_lock lk(read_lock);
        bool tpm_found = false;
        bool tpm_reset = false;
        int tpm_index = tpm_handler.find_tpm(std::stoi(parsed_msg.at(1)), true);
        int tpm_id = -1;
        if(tpm_index != -1){
            tpm_found = true;
            tpm_id = tpm_handler.getid(tpm_index);
            
            if(!tpm_handler.increase_iteration(tpm_id)){
                //tpm_handler.reset_tpm(tpm_index);
                tpm_reset = true;
            }
        }
       
        if(tpm_found){
            std::stringstream ss;
            if(tpm_reset){                                                                     // 1 = stop 
                ss << "4\t" << tpm_id << "\t" << tpm_handler.get_iteration(tpm_id) << "\t" << 0 << "\n";    
            }else{
                
                
                
                
                ss << tpm_handler.sync_tpm_message_one_advanced(tpm_id, parsed_msg);
                //ss << "1\t" << tpm_id << "\t" << tpm_handler.get_iteration(tpm_id) << "\n";
            }
            if(PRINT_SYNC_MESSAGES){
                std::cout << "on_sync: key_couter=" << tpm_handler.increase_key_counter(tpm_index) << ss.str() << "::::max iterations=" << tpm_handler.get_max_iterations(tpm_index) << std::endl;
            }else{
                tpm_handler.increase_key_counter(tpm_index);
            }
            do_write(ss.str());
        } else{ // must be new machine? but shouldnt be
            
            std::cout << "on_sync no tpm found" << std::endl;
            
        }
        
       //  }
    }
    
    
    
    void on_linking(std::vector<std::string> & parsed_msg){
       // { boost::recursive_mutex::scoped_lock lk(read_lock);
        bool tpm_found = false;
        bool tpm_reset = false;
        int tpm_index = tpm_handler.find_tpm(std::stoi(parsed_msg.at(1)), false);
        int tpm_id = -1;
        if(tpm_index != -1){
            tpm_found = true;
            tpm_id = tpm_handler.getid(tpm_index);
            if(!tpm_handler.increase_iteration(tpm_id)){
              //tpm_handler.reset_tpm(tpm_index);
              tpm_found = true;

            }
            tpm_handler.set_partner(tpm_id, std::stoi(parsed_msg.at(1)));
        }     
        
       
        if(tpm_found){
            std::stringstream ss;
            if(tpm_reset){                                                                     // 1 = stop 
                ss << "4\t" << tpm_id << "\t" << tpm_handler.get_iteration(tpm_id) << "\t" << 0 << "\n";    
            }else{
                
                              
                ss << tpm_handler.sync_tpm_message_one(tpm_id);
            }
            std::cout << "on_linking: " << ss.str() << std::endl;
            do_write(ss.str());
        }
        else{ // something is not right
            
            std::cout << "on_linking no tpm found" << std::endl;
            
        }
         //}
        
    }
    
    void on_reset(std::vector<std::string> & parsed_msg){
        //{ boost::recursive_mutex::scoped_lock lk(read_lock);
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
            ss << tpm_handler.sync_tpm_message_one(tpm_id);
            std::cout << "on_reset: " << ss.str() << std::endl;
            do_write(ss.str());
        }
        else{ // something is not right
            
            std::cout << "on_reset no tpm found" << std::endl;
            
        }
        // }
       
    }
    
 
    
    
    

    void on_write(const error_code & err, size_t bytes) {
       // { boost::recursive_mutex::scoped_lock lk(read_lock);
        do_read();
        // }
    }
    
    void do_read() {
        { boost::recursive_mutex::scoped_lock lk(read_lock);
        async_read(sock_, buffer(read_buffer_), MEM_FN2(read_complete,_1,_2), MEM_FN2(on_read,_1,_2));
         }
        //post_check_ping();
    }
    
    void do_write(const std::string & msg) {
        if ( !started() ) return;
        { boost::recursive_mutex::scoped_lock lk(write_lock);
       // boost::recursive_mutex::scoped_lock lk(cs_);        
        if(msg.length() > MAX_BUFF){
            std::cout << "msg is too big for buffer see what the problem is, msg length is " << msg.length() << "message is:\n" << msg << std::endl;
            std::exit(2);
        }
        std::copy(msg.begin(), msg.end(), write_buffer_);
        sock_.async_write_some( buffer(write_buffer_, msg.size()), MEM_FN2(on_write,_1,_2));
         }
    }
    
    size_t read_complete(const boost::system::error_code & err, size_t bytes) {
        if ( err) return 0;
        //{ boost::recursive_mutex::scoped_lock lk(read_lock);
        bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
        // we read one-by-one until we get to enter, no buffering
        return found ? 0 : 1;
       // }
    }
    
private:
    mutable boost::recursive_mutex cs_;
    ip::tcp::socket sock_;
    boost::shared_ptr<ip::tcp::endpoint> endpoint_;
    std::string ip_address_;
    int ip_port_;
    
    char read_buffer_[MAX_BUFF];
    char write_buffer_[MAX_BUFF];
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



void handle_accept(peer::ptr peer, const boost::system::error_code & err, ip::tcp::acceptor* acceptor) {
    peer->start(); // starts current client
    
    // creates and listens for new client
    peer::ptr new_peer = peer::new_(false); // false to accepting connection
    std::cout << "handle_accept run test" << std::endl;
    acceptor->async_accept(new_peer->sock(), boost::bind(handle_accept,new_peer,_1,acceptor)); // this 
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
    //boost::shared_ptr<ip::tcp::acceptor> acceptor(new ip::tcp::acceptor(service));
   // ip::tcp::endpoint ep(ip::tcp::v4(), 8002);
   // acceptor->open(ip::tcp::v4());
   // acceptor->bind(ep);
    
    boost::random::mt19937 gen;
        boost::random::uniform_int_distribution<> dist(1, 429496729);
        int random_number = dist(gen);

        srand( random_number );
    
    int listen_port = -1;
    int connect_port = -1;
    char help_message[] = {"sync --listen-port 8001 --connect-port 8002"};
    try {

        boost::program_options::options_description desc("Allowed options");
        desc.add_options()
            ("help", help_message)
            ("listen-port", boost::program_options::value<int>(), "set port to listen on")
            ("connect-port", boost::program_options::value<int>(), "set port to connect to")
        ;

        boost::program_options::variables_map vm;        
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);    

        if (vm.count("help")) {
            cout << help_message << "\n";
            return 0;
        }

        if (vm.count("listen-port")) {
            listen_port = vm["listen-port"].as<int>();
        } 
        
        if (vm.count("connect-port")) {
            connect_port = vm["connect-port"].as<int>();
        } 
        
        
        
    }
    catch(std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        std::cerr << "Exception of unknown type!\n";
    }
    
    if(listen_port == -1){
        std::cout << help_message << std::endl;
        return 0;
    }
    
    std::cout << "started sync using port " << listen_port << " sending to port " << connect_port << std::endl;
    
    ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), listen_port));
    peer::ptr initial_peer = peer::new_(false);
    acceptor.async_accept(initial_peer->sock(), boost::bind(handle_accept,initial_peer,_1, &acceptor));
    
    
    if(connect_port != -1){
        std::cout << "sending request" << std::endl;
        peer::ptr initiating_peer = peer::new_(true, "127.0.0.1", connect_port);
        initiating_peer->start();
    }
    
    start_listen(4);
    threads.join_all();
}
