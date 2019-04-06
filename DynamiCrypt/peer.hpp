/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   peer.hpp
 * Author: mrarchinton
 *
 * Created on 11 March 2019, 19:26
 */

#ifndef PEER_HPP
#define PEER_HPP

#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <cstdlib>
#include <iostream>
#include <boost/array.hpp>
#include <iterator>
#include <string>
#include <mutex>
#include <fstream>

#include "definitions.hpp"
#include "TpmNetworkHandler.hpp"

extern boost::asio::io_service service;


class peer : public boost::enable_shared_from_this<peer>, boost::noncopyable {
    typedef peer self_type;
    
    peer() : sock_(service), started_(false), peers_changed_(false){
        
    }
    
public:
    typedef boost::system::error_code error_code;
    typedef boost::shared_ptr<peer> ptr;

    void start(std::string service_name, std::string partner_name);
    
    static ptr new_(bool type_of_sock);
    
    static ptr new_(bool type_of_sock, std::string ip_address, int port);
    
    void stop();
    
    bool started() const { 
       // boost::recursive_mutex::scoped_lock lk(cs_);
        return started_; 
    }
    
    boost::asio::ip::tcp::socket & sock() { 
       // boost::recursive_mutex::scoped_lock lk(cs_);
        return sock_;
    }
    
    void set_peers_changed() { 
       // boost::recursive_mutex::scoped_lock lk(cs_);
        peers_changed_ = true; 
    }
    
    std::string get_service_name(){
        return service_name_;
    }
    
private:
    void on_read(const error_code & err, size_t bytes);
    
    void on_connect(const error_code & err, std::string service_name, std::string partner_name);
    
    void on_init(std::vector<std::string> & parsed_msg);
    
    void on_sync(std::vector<std::string> & parsed_msg);
    
    void on_linking(std::vector<std::string> & parsed_msg);
    
    void on_reset(std::vector<std::string> & parsed_msg);
    
    void on_write(const error_code & err, size_t bytes);
    
    void do_read();
    
    void do_write(const std::string & msg);
    
    size_t read_complete(const boost::system::error_code & err, size_t bytes);

private:
    mutable boost::recursive_mutex cs_;
    boost::asio::ip::tcp::socket sock_;
    boost::shared_ptr<boost::asio::ip::tcp::endpoint> endpoint_;
    std::string ip_address_;
    int ip_port_;
    
    char read_buffer_[1024];
    char write_buffer_[1024];
    bool started_;
    std::string service_name_;
    boost::posix_time::ptime last_ping_;
    bool peers_changed_;
    bool sock_using_ep;
    TpmNetworkHandler tpm_handler;
};

#endif /* PEER_HPP */

