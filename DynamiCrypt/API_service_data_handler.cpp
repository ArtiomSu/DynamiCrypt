/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "API_service_data_handler.hpp"
#include <sstream>
#include <iostream>
#include <exception>

API_service_data_handler::API_service_data_handler(){
    
}

std::string API_service_data_handler::new_service(std::string name){
    services.push_back(API_data());
    services.back().service_name_ = gen_name(name);
    return services.back().service_name_;
}
    
std::string API_service_data_handler::new_service(std::string name, std::string name_partner){
    services.push_back(API_data());
    services.back().service_name_ = gen_name(name);
    services.back().service_name_partner_ = name_partner;
    return services.back().service_name_;
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
        ptr->keys_.push_back(key_store());
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
    
        //CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encrypt;
        //CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption decrypt;
        
        std::string string_key = api_data->keys_.back().key;
        
        std::cout << "got key like this " << string_key << std::endl;
        std::cout << "key was used " << api_data->keys_.back().uses << " times" << std::endl;
        
        api_data->keys_.back().uses ++;
        
        // need to get shorten the key to make 256 bit key or byte array of 32
        // perform some additions or something
        
        //need to shorten string
        CryptoPP::byte key[ CryptoPP::AES::MAX_KEYLENGTH ], iv[ CryptoPP::AES::BLOCKSIZE ];
        
        if(string_key.length() > CryptoPP::AES::MAX_KEYLENGTH){
            int count_first = 0;
            int count_last = string_key.length() -1;
            int number_of_operations = count_last - CryptoPP::AES::MAX_KEYLENGTH;
            for(int i = 0; i< number_of_operations; i++){
                string_key[count_first] = string_key.at(count_first) + string_key.at(count_last);
                if(count_first == CryptoPP::AES::MAX_KEYLENGTH){
                    count_first = 0;
                }
                count_first ++;
                count_last --;
            }
        }
        
        int string_count = 0;
        for(int i=0; i<CryptoPP::AES::MAX_KEYLENGTH; i++){
            if(string_count == string_key.length() -1){
                string_count = 0;
            }
            key[i] = string_key.at(string_count);
            string_count ++;
        } 


        std::cout << "key int is ";

        
        for(int i=0; i < CryptoPP::AES::MAX_KEYLENGTH; i++){
            
            std::cout << int(key[i]) << ",";
        }


        std::cout << std::endl;
        
        std::cout << "key char is " << key << std::endl;
        
        
        //memset( key, 0x00, CryptoPP::AES::MAX_KEYLENGTH );
        memset( iv, 0x00, CryptoPP::AES::BLOCKSIZE );

        //
        // String and Sink setup
        //
        std::string plaintext = message;
        std::string ciphertext;
        std::string decryptedtext;

        //
        // Dump Plain Text
        //
        std::cout << "Plain Text (" << plaintext.size() << " bytes)" << std::endl;
        std::cout << plaintext;
        std::cout << std::endl << std::endl;

        ciphertext = encrypt(plaintext, key, iv);
        
        
        decryptedtext = decrypt(ciphertext, key, iv);
        
        
        
        
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

void API_service_data_handler::set_address_and_port_of_sync(std::string address, int port){
    sync_address = address;
    sync_port = port;
}

int API_service_data_handler::remove_service(std::string service_name){
    for(int i=0; i< services.size(); i++){
           if(!services.at(i).service_name_.compare(service_name)){
               services.erase(services.begin() + i);
               return 1;
            }   
    }
    return 0;
}

int API_service_data_handler::check_service(std::string service_name){
    for(int i=0; i< services.size(); i++){
           if(!services.at(i).service_name_.compare(service_name)){
               return 1;
            }   
    }
    return 0;
}



// private
API_data* API_service_data_handler::get_API_data(std::string name, int partner){
    for(int i=0; i< services.size(); i++){
        
        if(partner){
            if(!services.at(i).service_name_partner_.compare(name)){
                return &(services.at(i));
            } 
        }
        else{
           if(!services.at(i).service_name_.compare(name)){
                return &(services.at(i));
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

std::string API_service_data_handler::encrypt(std::string message, CryptoPP::byte key[ CryptoPP::AES::MAX_KEYLENGTH ], CryptoPP::byte iv[ CryptoPP::AES::BLOCKSIZE ]){
    std::string ciphertext;
    CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::MAX_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption( aesEncryption, iv );

    CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink( ciphertext ) );
    stfEncryptor.Put( reinterpret_cast<const unsigned char*>( message.c_str() ), message.length() );
    stfEncryptor.MessageEnd();

    //
    // Dump Cipher Text
    //
    std::cout << "Cipher Text (" << ciphertext.size() << " bytes)" << std::endl;

    for( int i = 0; i < ciphertext.size(); i++ ) {

        std::cout << "0x" << std::hex << (0xFF & static_cast<CryptoPP::byte>(ciphertext[i])) << " ";
    }

    std::cout << std::endl << std::endl;
    
    return ciphertext;
    
}

std::string API_service_data_handler::decrypt(std::string message, CryptoPP::byte key[ CryptoPP::AES::MAX_KEYLENGTH ], CryptoPP::byte iv[ CryptoPP::AES::BLOCKSIZE ]){
    std::string decryptedtext;
    CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::MAX_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption( aesDecryption, iv );

    CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink( decryptedtext ) );
    stfDecryptor.Put( reinterpret_cast<const unsigned char*>( message.c_str() ), message.size() );
    stfDecryptor.MessageEnd();

    //
    // Dump Decrypted Text
    //
    std::cout << "Decrypted Text: " << std::endl;
    std::cout << decryptedtext;
    std::cout << std::endl << std::endl;
    
    return decryptedtext;
        
}