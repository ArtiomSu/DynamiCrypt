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
#include <fstream>

#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/base64.h>

#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>

#include "TPMInputVector.hpp"
#include "DynamicArray.hpp"
#include "TreeParityMachine.hpp"
#include "TpmHandler.hpp"
#include "SingleTpmNetworkHandler.hpp"
#include "TpmNetworkHandler.hpp"
#include "definitions.hpp"
#include "peer.hpp"

#include "APIServer.hpp"

//g++ *.cpp -lboost_system -lpthread -lboost_thread -lboost_program_options -lcryptopp -o sync
//g++ *.cpp -lboost_system -lpthread -lboost_thread -lboost_program_options -lcryptopp -lpistache -o sync-test
//./sync-test --listen-port 8003 --api-port 9200


using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;


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

void seed_random(){
    unsigned long long int random_value = 0; //Declare value to store data into
        size_t size = sizeof(random_value); //Declare size of data
        std::ifstream urandom("/dev/urandom", std::ios::in|std::ios::binary); //Open stream
        if(urandom) //Check if stream is open
        {
            urandom.read(reinterpret_cast<char*>(&random_value), size); //Read from urandom
            if(!urandom) //Check if stream is ok, read succeeded
            {
                std::cerr << "Failed to read from /dev/urandom" << std::endl;
            }
            urandom.close(); //close stream
        }
        else //Open failed
            {
                std::cerr << "Failed to open /dev/urandom" << std::endl;
        }
        
        srand( random_value );
}


int main(int argc, char* argv[]) {
    //boost::shared_ptr<ip::tcp::acceptor> acceptor(new ip::tcp::acceptor(service));
   // ip::tcp::endpoint ep(ip::tcp::v4(), 8002);
   // acceptor->open(ip::tcp::v4());
   // acceptor->bind(ep);
    seed_random();
    //test_term();
    
    int listen_port = -1;
    int connect_port = -1;
    int api_port = -1;
    char help_message[] = {"sync --listen-port 8001 --api-port 9081 --connect-port 8002"};
    try {

        boost::program_options::options_description desc("Allowed options");
        desc.add_options()
            ("help", help_message)
            ("listen-port", boost::program_options::value<int>(), "set port to listen on")
            ("connect-port", boost::program_options::value<int>(), "set port to connect to")
            ("api-port", boost::program_options::value<int>(), "set port for API to listen to")
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
        
        if (vm.count("api-port")) {
            api_port = vm["api-port"].as<int>();
        } 
             
    }
    catch(std::exception& e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        std::cerr << "Exception of unknown type!\n";
    }
    
    if(listen_port == -1 || api_port == -1){
        std::cout << help_message << std::endl;
        return 0;
    }
    
    std::cout << "started sync using port " << listen_port << " sending to port " << connect_port << std::endl;
    
    ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), listen_port));
    peer::ptr initial_peer = peer::new_(false);
    acceptor.async_accept(initial_peer->sock(), boost::bind(handle_accept,initial_peer,_1, &acceptor));
    
    
    if(connect_port != -1){
        std::cout << "sending request" << std::endl;
        begin_sync("127.0.0.1", connect_port);
    }
    
    start_listen(4);
    APIServer api_server(api_port);
    threads.join_all();
}
