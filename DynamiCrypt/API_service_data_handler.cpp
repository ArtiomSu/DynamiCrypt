/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "API_service_data_handler.hpp"
#include <sstream>
#include <iostream>
#include <exception>
#include "definitions.hpp"

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


std::string API_service_data_handler::crypt(std::string name, std::string message, std::string hash, int mode, int operation){
    API_data* api_data = get_API_data(name, 0);
    int max_attempts_to_decrypt = 10; //try 10 times to decrypt if not decrypted by then, then give up and avoid being stuck in loop
    if(api_data != NULL){
    
        //CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encrypt;
        //CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption decrypt;
        
        
        
        
        
        

        CryptoPP::byte iv[ CryptoPP::AES::BLOCKSIZE ];
        memset( iv, 0x00, CryptoPP::AES::BLOCKSIZE );
        
        std::string output;
        
        if(operation == 0){ // encrypt
             //speed - same key until new key recieved
            
            std::string string_key;
            if(mode == 1){
                //check for any latest key 
                if(api_data->keys_.size() == 0){
                    return DYNAMICRYPT_API_WAIT;
                }else{
                    string_key = api_data->keys_.back().key;
                    api_data->keys_.back().uses ++;
                    if(PRINT_API_CRYPT_MESSAGES){
                        std::cout << "got key like this " << string_key << std::endl;
                        std::cout << "key was used " << api_data->keys_.back().uses << " times" << std::endl;
                    }
                }
            
            }
            //security - different key each time
            else if (mode == 2){
                if(api_data->keys_.size() == 0){
                    return DYNAMICRYPT_API_WAIT;
                }
                else{
                    int attempts_to_find_key = 0; //search max_attempts_to_decrypt times for the key since decrypt will only search for max_attempts_to_decrypt times too
                    int found_key = 0;
                    for(int number_of_keys = api_data->keys_.size()-1; number_of_keys >= 0; number_of_keys --){
                        if(attempts_to_find_key == max_attempts_to_decrypt){
                            break;
                        }
                        attempts_to_find_key ++;
                        if(api_data->keys_.at(number_of_keys).uses == 0){
                            string_key = api_data->keys_.at(number_of_keys).key;
                            api_data->keys_.at(number_of_keys).uses++;
                            if(PRINT_API_CRYPT_MESSAGES){
                                std::cout << "got key like this " << string_key << std::endl;
                                std::cout << "key was used " << api_data->keys_.at(number_of_keys).uses << " times" << std::endl;
                            }
                            found_key = 1;
                            break;
                        }
                    }
                    if(!found_key){
                        return DYNAMICRYPT_API_WAIT;
                    }
                }
            }
            if(PRINT_API_CRYPT_MESSAGES){
                std::cout << "keyring size for service " << name << " = " << api_data->keys_.size() << std::endl;
            } 
            // need to get shorten the key to make 256 bit key or byte array of 32
            // perform some additions or something

            //need to shorten string
            CryptoPP::byte key[ CryptoPP::AES::MAX_KEYLENGTH ];
            gen_key(string_key,key);
        
            
            
            std::string for_encode = encrypt(message, key, iv);
            
            std::string encoded = encode_base64(for_encode);
            
            output = encoded;
            


        }else{ // decrypt
            //decode message
            if(PRINT_API_CRYPT_MESSAGES){
                std::cout << "encoded message received = " << message << std::endl;
            }
            std::string decoded_message = decode_base64(message);

            //try to use whichever key is last
            if(mode == 1){
                if(api_data->keys_.size() == 0){
                    return DYNAMICRYPT_API_WAIT; // no keys in key ring shouldn't happen with decrypt if used properly
                }
                int number_of_keys = api_data->keys_.size()-1;  // maybe change to keys_.size()-1
                for(int decrypt_loop = 0; decrypt_loop<max_attempts_to_decrypt; decrypt_loop++){
                    //try last key first
                    std::string string_key = api_data->keys_.at(number_of_keys).key;
                    if(PRINT_API_CRYPT_MESSAGES){
                        std::cout << "trying to decrypt with key " << string_key << std::endl;
                        std::cout << "key was used " << api_data->keys_.at(number_of_keys).uses << " times" << std::endl;
                    }
                    CryptoPP::byte key[ CryptoPP::AES::MAX_KEYLENGTH ];
                    gen_key(string_key,key);

                    output = decrypt(decoded_message, key, iv);
                    if(!hash_with_sha_256(output).compare(hash)){ //decrypted successfully
                        api_data->keys_.at(number_of_keys).uses ++;
                        break;
                    }

                    if(number_of_keys == 0){
                        //output = "failed";
                        return DYNAMICRYPT_API_FAILED_DECRYPT;
                        break;
                    }

                    number_of_keys --;
                
                }
            }
            //security - different key each time
            else if (mode == 2){ // try keys with 0 uses first
                std::string string_key;
                if(api_data->keys_.size() == 0){
                    return DYNAMICRYPT_API_WAIT; // no keys in key ring shouldn't happen with decrypt if used properly
                }
                int has_skipped_keys = 0;
                int number_of_keys = api_data->keys_.size()-1;  // maybe change to keys_.size()-1
                
                //std::cout << "number_of_keys at start " << number_of_keys << std::endl;
                std::vector<int> skipped_keys;
                for(int decrypt_loop = 0; decrypt_loop<max_attempts_to_decrypt; decrypt_loop++){
                    //std::cout << "decrypt_loop at start " << decrypt_loop << std::endl;
                    if(number_of_keys == -1){ // needed because of the continue which could cause number_of_keys to be -1
                        break;
                    }
                    
                    if(api_data->keys_.at(number_of_keys).uses == 0){
                        
                        string_key = api_data->keys_.at(number_of_keys).key;
                        //std::cout << "key with 0 uses found " << string_key << std::endl;
                    }else{
                        skipped_keys.push_back(number_of_keys);
                        //std::cout << "skipping key at index " << number_of_keys << std::endl;
                        number_of_keys --;
                        has_skipped_keys = 1;
                        
                        continue;
                    }
                    
                    if(PRINT_API_CRYPT_MESSAGES){
                        std::cout << "trying to decrypt with key " << string_key << std::endl;
                        std::cout << "key was used " << api_data->keys_.at(number_of_keys).uses << " times" << std::endl;
                    }
                    CryptoPP::byte key[ CryptoPP::AES::MAX_KEYLENGTH ];
                    gen_key(string_key,key);

                    output = decrypt(decoded_message, key, iv);
                    if(!hash_with_sha_256(output).compare(hash)){ //decrypted successfully
                        api_data->keys_.at(number_of_keys).uses ++;
                        return output;
                    }

                    if(number_of_keys == 0){
                        break;
                    }

                    number_of_keys --;
                
                }
                //code runs here only if decryption was unsuccessful
                if(has_skipped_keys){ //check for skipped keys
                    for(int i = 0; i < skipped_keys.size(); i ++ ){
                        string_key = api_data->keys_.at(i).key;
                        if(PRINT_API_CRYPT_MESSAGES){
                            std::cout << "trying to decrypt with key " << string_key << std::endl;
                            std::cout << "key was used " << api_data->keys_.at(i).uses << " times" << std::endl;
                        }
                        CryptoPP::byte key[ CryptoPP::AES::MAX_KEYLENGTH ];
                        gen_key(string_key,key);

                        output = decrypt(decoded_message, key, iv);
                        if(!hash_with_sha_256(output).compare(hash)){ //decrypted successfully
                            api_data->keys_.at(i).uses ++;
                            return output;
                        }
                    }
                    // if key not decrypted then code is continued here therefore
                    //output = "failed";
                    return DYNAMICRYPT_API_FAILED_DECRYPT;
                }else{
                    //output = "failed";
                    return DYNAMICRYPT_API_FAILED_DECRYPT;
                }
                
                
                
            }
            
            
            
        }

        return output;
    }
    else{
        return DYNAMICRYPT_API_ERROR;
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

    if(PRINT_API_CRYPT_MESSAGES){
        std::cout << "Cipher Text (" << ciphertext.size() << " bytes)" << std::endl;

        for( int i = 0; i < ciphertext.size(); i++ ) {

            std::cout << "0x" << std::hex << (0xFF & static_cast<CryptoPP::byte>(ciphertext[i])) << " ";
        }

        std::cout << std::endl << std::endl;
    }
    
    return ciphertext;
    
}

std::string API_service_data_handler::decrypt(std::string message, CryptoPP::byte key[ CryptoPP::AES::MAX_KEYLENGTH ], CryptoPP::byte iv[ CryptoPP::AES::BLOCKSIZE ]){
    std::string decryptedtext;
    CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::MAX_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption( aesDecryption, iv );

    CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink( decryptedtext ) );
    stfDecryptor.Put( reinterpret_cast<const unsigned char*>( message.c_str() ), message.size() );
    stfDecryptor.MessageEnd();

    if(PRINT_API_CRYPT_MESSAGES){
        std::cout << "Decrypted Text: " << std::endl;
        std::cout << decryptedtext;
        std::cout << std::endl << std::endl;
    }
    
    return decryptedtext;
        
}

void API_service_data_handler::gen_key(std::string string_key, CryptoPP::byte* key){
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

        if(PRINT_API_CRYPT_MESSAGES){
            std::cout << "key int is ";


            for(int i=0; i < CryptoPP::AES::MAX_KEYLENGTH; i++){

                std::cout << int(key[i]) << ",";
            }


            std::cout << std::endl;

            std::cout << "key char is " << key << std::endl;
        }
        
}

std::string API_service_data_handler::encode_base64(std::string for_encode) {
    std::string encoded;
            
            
    CryptoPP::byte decoded[for_encode.size()];

    for( int i = 0; i < for_encode.size(); i++ ) {
        decoded[i] = (0xFF & static_cast<CryptoPP::byte>(for_encode[i]));
        //std::cout << "0x" << std::hex << (0xFF & static_cast<CryptoPP::byte>(ciphertext[i])) << " ";
    }

    CryptoPP::Base64Encoder encoder;
    encoder.Put(decoded, sizeof(decoded));
    encoder.MessageEnd();

    CryptoPP::word64 size = encoder.MaxRetrievable();
    if(size)
    {
        encoded.resize(size);		
        encoder.Get((CryptoPP::byte*)&encoded[0], encoded.size());
    }

    if(PRINT_API_CRYPT_MESSAGES){
        std::cout << "encoded message after encryption = " << encoded << std::endl; 
    }
    
    return encoded;
}

std::string API_service_data_handler::decode_base64(std::string message) {  
    std::string decoded_message;
            
    CryptoPP::Base64Decoder decoder;
    decoder.Put( (CryptoPP::byte*)message.data(), message.size() );
    decoder.MessageEnd();

    CryptoPP::word64 size = decoder.MaxRetrievable();
    if(size && size <= SIZE_MAX)
    {
        decoded_message.resize(size);		
        decoder.Get((CryptoPP::byte*)&decoded_message[0], decoded_message.size());
    }
    return decoded_message;
}


