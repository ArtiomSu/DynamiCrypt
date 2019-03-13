/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SingleTpmNetworkHandler.hpp
 * Author: mrarchinton
 *
 * Created on 11 March 2019, 14:59
 */

#ifndef SINGLETPMNETWORKHANDLER_HPP
#define SINGLETPMNETWORKHANDLER_HPP

#include "TreeParityMachine.hpp"
#include "TPMInputVector.hpp"
#include <string>
#include <fstream>
#include <cstdlib>
#include <vector>

class SingleTpmNetworkHandler{
private:
    int iteration_;
    int tpm_id_;
    int max_iterations_;
    int partner_tpm; // id of partner tpm
    TreeParityMachine tpm;
    TPMInputVector tmpinputvector;
    int check_for_key_counter;
    std::vector<std::string> proper_keys;
    //auto log_file;
    std::ofstream key_log;
    std::string filename;    
    
public:  
    SingleTpmNetworkHandler(int id);
    
    int iteration(){
        return iteration_;
    }
    
    int increase_key_counter();
    
    int iteration_increase();
    
    std::string get_key();
    
    std::string SHA256(std::string data);
    
    std::string calc_test(std::string randomInput);
    
    int getWeightValue();
    
    int id(){
        return tpm_id_;
    }
    
    int set_partner(int id);
    
    int partner(){
        return partner_tpm;
    }
    
    void reset();
    
    void reset_check_for_key_counter();
    
    TPMInputVector* get_tmpinputvector(){
        return &tmpinputvector;
    }
    
    std::string create_random_input_vector();
    
    int set_random_input_vector_from_string(std::string input);
    
    int compute_tpm_result();
    
    void update_weight();
    
    int get_max_iterations(){
        return max_iterations_;
    }
    
    void add_key_to_proper_keys(std::string key);
    
    
    
};

#endif /* SINGLETPMNETWORKHANDLER_HPP */

