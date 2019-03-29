/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "peer.hpp"
#include "definitions.hpp"
#include "TpmHandler.hpp"
#include <boost/algorithm/string.hpp>

typedef boost::shared_ptr<peer> ptr;

/*peer::peer(){
    started_ = false;
    peers_changed_ = false;
    sock_ = service;
}*/

void peer::start(std::string service_name, std::string partner_name){
    { boost::recursive_mutex::scoped_lock lk(read_lock);
        peers.push_back( shared_from_this());
    }
        
    std::cout << " type of peer " << sock_using_ep << std::endl;
    started_ = true;
    if(sock_using_ep){ // this makes the connection so write straight away
        //endpoint_(ip::address::from_string(ip_address_), ip_port_);
        endpoint_ = boost::make_shared<boost::asio::ip::tcp::endpoint>(boost::asio::ip::address::from_string(ip_address_), ip_port_);
        sock_.async_connect(*endpoint_, MEM_FN3(on_connect,_1,service_name,partner_name));
        /*try{
            //sock_.async_connect(*endpoint_, MEM_FN(on_connect_no_error_handling));
            sock_.async_connect(*endpoint_, MEM_FN1(on_connect,_1));
        }
        catch(std::exception& e){
            stop();
            std::cout << "peer starts exception: " << e.what() << std::endl;
        }*/

    } else { // this will listen to connection so read.
    //boost::recursive_mutex::scoped_lock lk(cs_);
    do_read();
    }
}

ptr peer::new_(bool type_of_sock){
    boost::shared_ptr<peer> new_(new peer);
    new_->sock_using_ep = type_of_sock;
    return new_;
}

ptr peer::new_(bool type_of_sock, std::string ip_address, int port){
    boost::shared_ptr<peer> new_(new peer);
    new_->sock_using_ep = type_of_sock;
    new_->ip_address_ = ip_address;
    new_->ip_port_ = port;
    return new_;
}

void peer::stop(){
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

void peer::on_read(const error_code & err, size_t bytes){
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

void peer::on_connect(const error_code & err, std::string service_name, std::string partner_name){
    //  { boost::recursive_mutex::scoped_lock lk(read_lock);
    //for(int b; b< MAX_TPMS_PER_PEER; b++){ //create 10 tpms
    for(int b=0; b< MAX_TPMS_PER_PEER; b++){ //create 10 tpms
        int id = tpm_handler.create_new_tpm(service_name, partner_name);

        if ( !err){      
            std::stringstream ss;                                               // send self service name
            ss << "2\t" << id << "\t" << tpm_handler.get_iteration(id) << "\t" << service_name << "\t" << partner_name << "\n";
            std::cout << "on_connect: " << ss.str() << std::endl;
            do_write(ss.str());
        }
        else{
            std::cout << "Error on_connect:" << err.message() << std::endl;
            api_service_data_handler.remove_service(service_name);
            stop();
            //throw std::invalid_argument( "cannot connect to tpm" );
        }
    }
   // }
}


void peer::on_init(std::vector<std::string> & parsed_msg){
    //  { boost::recursive_mutex::scoped_lock lk(read_lock);
    
    
            
    //api_service_data_handler.new_service("self", parsed_msg.at(3));
    
    
    
    int id = tpm_handler.create_new_tpm(parsed_msg.at(4),parsed_msg.at(3));
    tpm_handler.set_partner(id, std::stoi(parsed_msg.at(1)));


    std::stringstream ss;
    ss << "3\t" << id << "\t" << tpm_handler.get_iteration(id) << "\t" << tpm_handler.get_partner(id) << "\n";
    std::cout << " on_init: " << ss.str() << std::endl;
    do_write(ss.str());
    // }
}

void peer::on_sync(std::vector<std::string> & parsed_msg){
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
            ss << "4\t" << tpm_id << "\t" << tpm_handler.get_iteration(tpm_id) << "\t" << 0 << "\t" << 0 << "\n";
        }else{




            ss << tpm_handler.sync_tpm_message_one_advanced(tpm_id, parsed_msg);
            //ss << "1\t" << tpm_id << "\t" << tpm_handler.get_iteration(tpm_id) << "\n";
        }
        if(PRINT_SYNC_MESSAGES){
            std::cout << "on_sync: key_couter=" << tpm_handler.increase_key_counter(tpm_index) << std::endl;
        }else{
            tpm_handler.increase_key_counter(tpm_index);
        }
        do_write(ss.str());
    } else{ // must be new machine? but shouldnt be

        std::cout << "on_sync no tpm found" << std::endl;

    }

   //  }
}

void peer::on_linking(std::vector<std::string> & parsed_msg){
    // { boost::recursive_mutex::scoped_lock lk(read_lock);
    bool tpm_found = false;
    bool tpm_reset = false;
    int tpm_index = tpm_handler.find_tpm(std::stoi(parsed_msg.at(3)), false);
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
            ss << "4\t" << tpm_id << "\t" << tpm_handler.get_iteration(tpm_id) << "\t" << 0 << "\t" << 0 << "\n";
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
        
void peer::on_reset(std::vector<std::string> & parsed_msg){
    //{ boost::recursive_mutex::scoped_lock lk(read_lock);
    bool tpm_found = false;      
    int tpm_index = tpm_handler.find_tpm(std::stoi(parsed_msg.at(1)), true);
    int tpm_id = -1;
    if(tpm_index != -1){
        tpm_found = true;
        tpm_id = tpm_handler.getid(tpm_index);

        if(std::stoi(parsed_msg.at(4)) == 1){
            SingleTpmNetworkHandler *hptr = tpm_handler.get_tpm(tpm_id);
            hptr->add_key_to_proper_keys(hptr->get_key());
            std::cout << "second tpm added key to list " << hptr->get_key();
            //std::exit(1);
        } 


        tpm_handler.reset_tpm(tpm_index);


        tpm_handler.increase_iteration(tpm_id);
        //tpm_handler.set_partner(tpm_id, std::stoi(parsed_msg.at(1)));
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

void peer::on_write(const error_code & err, size_t bytes){
    // { boost::recursive_mutex::scoped_lock lk(read_lock);
        do_read();
    // }
}

void peer::do_read(){
    { boost::recursive_mutex::scoped_lock lk(read_lock);
    async_read(sock_,  boost::asio::buffer(read_buffer_), MEM_FN2(read_complete,_1,_2), MEM_FN2(on_read,_1,_2));
     }
    //post_check_ping();
}

void peer::do_write(const std::string & msg){
    if(SLOW_DOWN){
        boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
    }
    if ( !started() ) return;
    { boost::recursive_mutex::scoped_lock lk(write_lock);
   // boost::recursive_mutex::scoped_lock lk(cs_);        
    if(msg.length() > MAX_BUFF){
        std::cout << "msg is too big for buffer see what the problem is, msg length is " << msg.length() << "message is:\n" << msg << std::endl;
        std::exit(2);
    }
    std::copy(msg.begin(), msg.end(), write_buffer_);
    sock_.async_write_some(  boost::asio::buffer(write_buffer_, msg.size()), MEM_FN2(on_write,_1,_2));
     }
}

size_t peer::read_complete(const boost::system::error_code & err, size_t bytes){
    if ( err) return 0;
    //{ boost::recursive_mutex::scoped_lock lk(read_lock);
    bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
    // we read one-by-one until we get to enter, no buffering
    return found ? 0 : 1;
   // }
}



















