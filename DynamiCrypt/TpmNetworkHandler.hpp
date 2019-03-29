/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TpmNetworkHandler.hpp
 * Author: mrarchinton
 *
 * Created on 11 March 2019, 18:44
 */

#ifndef TPMNETWORKHANDLER_HPP
#define TPMNETWORKHANDLER_HPP
#include "SingleTpmNetworkHandler.hpp"
#include <string>
#include <fstream>
#include <cstdlib>
#include <vector>

class TpmNetworkHandler{
private:
    std::vector<SingleTpmNetworkHandler> tpm_networks_;
public:
    TpmNetworkHandler();
    
    int create_new_tpm(std::string service_name, std::string partner_name);
    
    int find_tpm(int id, bool find_partner);
    
    int increase_iteration(int id);
    
    int reset_tpm(int index);
    
    int get_max_iterations(int index);
    
    int getid(int index);
    
    int get_iteration(int id);
    
    int set_partner(int id, int partner_id);
    
    int get_partner(int id);
    
    SingleTpmNetworkHandler* get_tpm(int id);
    
    int increase_key_counter(int index);
    
    std::string get_key_tpm(int index);
    
    std::string calc_test(int index, std::string input);
    
    std::string sync_tpm_message_one(int tpm_id);
    
    std::string gen_random_input_for_key(int lengthOfKey);
    
    std::string sync_tpm_message_one_advanced(int tpm_id, std::vector<std::string> & parsed_msg);
    
};


#endif /* TPMNETWORKHANDLER_HPP */

