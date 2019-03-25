/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "API_service_data_handler.hpp"
#include <sstream>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>

API_service_data_handler::API_service_data_handler(){
    
}

std::string API_service_data_handler::new_service(std::string name){
    services.push_back(new API_data);
    services.back()->service_name_ = gen_name(name);
}
    
std::string API_service_data_handler::new_service(std::string name, std::string name_partner){
    services.push_back(new API_data);
    services.back()->service_name_ = gen_name(name);
    services.back()->service_name_partner_ = name_partner;
}

int API_service_data_handler::update_partner(std::string name, std::string partners_name){
    API_data* ptr = get_API_data(name, 0);
    if(ptr != NULL){
        ptr->service_name_partner_ = partners_name;
        return 1;
    }
    else{
        return 0;
    }
    
}

int API_service_data_handler::add_key(std::string name, std::string key){
    API_data* ptr = get_API_data(name, 0);
    if(ptr != NULL){
        ptr->keys_.push_back(new key_store);
        ptr->keys_.back().key = key;
        ptr->keys_.back().uses = 0;
        return 1;
    }
    else{
        return 0;
    }
}


std::string API_service_data_handler::crypt(std::string name, std::string message, int mode, int operation){
    API_data* api_data = get_API_data(name, 0);
    if(api_data != NULL){
        
        CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encrypt;
        CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption decrypt;
        
        
        
        
        //speed same key until new key recieved
        if(mode == 1){
            
        }
        //security different key each time
        else if (mode == 2){
            
        }
        
        
        
        return "ok";
    }
    else{
        return NULL;
    }
}



// private
API_data* API_service_data_handler::get_API_data(std::string name, int partner){
    for(int i=0; i< services.size(); i++){
        
        if(partner){
            if(!services.at(i).service_name_partner_.compare(name)){
                return services.at(i)&;
            } 
        }
        else{
           if(!services.at(i).service_name_.compare(name)){
                return services.at(i)&;
            } 
        }
        
        
    }
    return NULL;
}

std::string API_service_data_handler::gen_name(std::string name){
    int broke = 0;
    while(1){
        broke = 0;
        for(int i=0; i < services.size(); i++){
            if(! services.at(i).service_name_.compare(name) ){
                int random = rand() % 1000000;
                std::stringstream ss;
                ss << name << random;

                name = ss.str();
                broke = 1;
                break;

            }
        }
        
        if(!broke){
            return name;
        }
        
    }
}