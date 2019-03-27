/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   API_service_data_handler.hpp
 * Author: mrarchinton
 *
 * Created on 25 March 2019, 12:43
 */

#ifndef API_SERVICE_DATA_HANDLER_HPP
#define API_SERVICE_DATA_HANDLER_HPP

#include <cstdio>
#include <string>
#include <vector>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>

struct key_store {
    std::string key;
    int uses;
};

struct API_data {
    std::string service_name_;
    std::string service_name_partner_;
    std::vector<key_store> keys_;
};

class API_service_data_handler{
public:
    API_service_data_handler();
    
    //return new id
    std::string new_service(std::string name);
    
    std::string new_service(std::string name, std::string name_partner);
    
    int update_partner(std::string name, std::string partners_name);
    
    int add_key(std::string name, std::string key);
                                                                        //encrypt/decrypt
    std::string crypt(std::string name, std::string message, int mode, int operation);
    
    
    
private:
    std::vector<API_data> services;
    
    std::string gen_name(std::string name);
    
    // get by partner if partner = 1
    API_data* get_API_data(std::string name, int partner);
    
    std::string decrypt(std::string message, CryptoPP::byte key[ CryptoPP::AES::MAX_KEYLENGTH ], CryptoPP::byte iv[ CryptoPP::AES::BLOCKSIZE ]);

    std::string encrypt(std::string message, CryptoPP::byte key[ CryptoPP::AES::MAX_KEYLENGTH ], CryptoPP::byte iv[ CryptoPP::AES::BLOCKSIZE ]);

    
    
};



#endif /* API_SERVICE_DATA_HANDLER_HPP */

