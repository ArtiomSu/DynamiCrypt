#include "definitions.hpp"


boost::recursive_mutex read_lock;
boost::recursive_mutex write_lock;

boost::asio::io_service service;


array peers;

API_service_data_handler api_service_data_handler;

const int MAX_TPMS_PER_PEER = 1;
const int MAX_BUFF = 1024;
const int PRINT_SYNC_MESSAGES =1;
const int SLOW_DOWN = 0;

void update_peers_changed() {
    array copy;
    { boost::recursive_mutex::scoped_lock lk(read_lock);
      copy = peers;
    }
    for( array::iterator b = copy.begin(), e = copy.end(); b != e; ++b){
        (*b)->set_peers_changed();
    }
}

int begin_sync(std::string address, int port){
    peer::ptr initiating_peer = peer::new_(true, address, port);
    initiating_peer->start();
    return 1;
}

std::string test_api(){
    std::string test_string = "testing extern string";
    return test_string;
}