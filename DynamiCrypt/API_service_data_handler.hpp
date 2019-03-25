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

struct key_store {
    std::string key;
    int uses;
};

struct API_data {
    std::string service_name_;
    std::string service_name_partner_;
    std::vector<key_store> keys_;
};



#endif /* API_SERVICE_DATA_HANDLER_HPP */

