/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   definitions.hpp
 * Author: mrarchinton
 *
 * Created on 11 March 2019, 19:08
 */

#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include "peer.hpp"
#include <vector>
#include "API_service_data_handler.hpp"
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/base64.h>
#include <sstream>
#include <string>

class peer;

extern const int MAX_TPMS_PER_PEER;
extern const int MAX_BUFF;
extern const int PRINT_SYNC_MESSAGES;
extern const int SLOW_DOWN;
//constants used to check encryption status
extern const std::string DYNAMICRYPT_API_WAIT;
extern const std::string DYNAMICRYPT_API_FAILED_DECRYPT;
extern const std::string DYNAMICRYPT_API_ERROR;

extern const int PRINT_API_CRYPT_MESSAGES;

extern boost::recursive_mutex read_lock;
extern boost::recursive_mutex write_lock;
extern boost::asio::io_service service;

extern void update_peers_changed();

extern int begin_sync(std::string address, int port, std::string service_name, std::string partner_name);

extern std::string hash_with_sha_256(std::string data);

extern std::string test_api();

typedef std::vector<boost::shared_ptr<peer>> array; // array of shared pointers to talk_to_client class

extern array peers;

extern API_service_data_handler api_service_data_handler;

#define MEM_FN(x)       boost::bind(&self_type::x, shared_from_this())
#define MEM_FN1(x,y)    boost::bind(&self_type::x, shared_from_this(),y)
#define MEM_FN2(x,y,z)  boost::bind(&self_type::x, shared_from_this(),y,z)
#define MEM_FN3(x,y,z,o)  boost::bind(&self_type::x, shared_from_this(),y,z,o)



#endif /* DEFINITIONS_HPP */

