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
            if(mode == 1){
            
            }
            //security - different key each time
            else if (mode == 2){
            
            }
            std::string string_key = api_data->keys_.back().key;
               
        
            std::cout << "got key like this " << string_key << std::endl;
            std::cout << "key was used " << api_data->keys_.back().uses << " times" << std::endl;

            api_data->keys_.back().uses ++;

            // need to get shorten the key to make 256 bit key or byte array of 32
            // perform some additions or something

            //need to shorten string
            CryptoPP::byte key[ CryptoPP::AES::MAX_KEYLENGTH ];
            gen_key(string_key,key);
        
            
            
            std::string for_encode = encrypt(message, key, iv);
            
            std::string encoded = encode_base64(for_encode);
            
            /*
            CryptoPP::byte decoded[for_encode.size()];
            
            for( int i = 0; i < for_encode.size(); i++ ) {
                decoded[i] = (0xFF & static_cast<CryptoPP::byte>(for_encode[i]));
                //std::cout << "0x" << std::hex << (0xFF & static_cast<CryptoPP::byte>(ciphertext[i])) << " ";
            }
            
            std::string encoded;

            CryptoPP::Base64Encoder encoder;
            encoder.Put(decoded, sizeof(decoded));
            encoder.MessageEnd();

            CryptoPP::word64 size = encoder.MaxRetrievable();
            if(size)
            {
                encoded.resize(size);		
                encoder.Get((CryptoPP::byte*)&encoded[0], encoded.size());
            }
             */ 
            std::cout << "encoded message after encryption = " << encoded << std::endl; 
             
            
            output = encoded;
            
            
            
            /* works ok but for some reason meeses up if sent through network
            std::string decoded_message;
            
            decode_base64(output, decoded_message);
            
            std::string decryptede;
            
            
            int number_of_keys = api_data->keys_.size()-1;  // maybe change to keys_.size()-1
            for(int decrypt_loop = 0; decrypt_loop<max_attempts_to_decrypt; decrypt_loop++){
                 //try last key first
                std::string string_key = api_data->keys_.at(number_of_keys).key;
                std::cout << "trying to decrypt with key " << string_key << std::endl;
                std::cout << "key was used " << api_data->keys_.at(number_of_keys).uses << " times" << std::endl;
                CryptoPP::byte key[ CryptoPP::AES::MAX_KEYLENGTH ];
                gen_key(string_key,key);
                
                decryptede = decrypt(decoded_message, key, iv);
                if(!decryptede.compare(message)){ //decrypted successfully
                    api_data->keys_.at(number_of_keys).uses ++;
                    break;
                }
                
                if(number_of_keys == 0){
                    std::cout << "failed decrypt\n";
                    break;
                }
                
                number_of_keys --;
                
                 
                 
                
                
                
            }
            */ 
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            

        }else{ // decrypt
            //decode message 
            std::cout << "encoded message received = " << message << std::endl;
            std::string decoded_message;
            
            decode_base64(message, decoded_message);

            /*
            CryptoPP::Base64Decoder decoder;
            decoder.Put( (CryptoPP::byte*)message.data(), message.size() );
            decoder.MessageEnd();

            CryptoPP::word64 size = decoder.MaxRetrievable();
            if(size && size <= SIZE_MAX)
            {
                decoded_message.resize(size);		
                decoder.Get((CryptoPP::byte*)&decoded_message[0], decoded_message.size());
            }
             */
            
            
            
            if(mode == 1){
            
            }
            //security - different key each time
            else if (mode == 2){
            
            }
            //loop for mode 1
            int number_of_keys = api_data->keys_.size()-1;  // maybe change to keys_.size()-1
            for(int decrypt_loop = 0; decrypt_loop<max_attempts_to_decrypt; decrypt_loop++){
                 //try last key first
                std::string string_key = api_data->keys_.at(number_of_keys).key;
                std::cout << "trying to decrypt with key " << string_key << std::endl;
                std::cout << "key was used " << api_data->keys_.at(number_of_keys).uses << " times" << std::endl;
                CryptoPP::byte key[ CryptoPP::AES::MAX_KEYLENGTH ];
                gen_key(string_key,key);
                
                output = decrypt(decoded_message, key, iv);
                if(!hash_with_sha_256(output).compare(hash)){ //decrypted successfully
                    api_data->keys_.at(number_of_keys).uses ++;
                    break;
                }
                
                if(number_of_keys == 0){
                    output = "failed";
                    break;
                }
                
                number_of_keys --;
                
                 
                 
                
                
                
            }
            
            
        }
        
       
        
        
        
        return output;
    }
    else{
        return "error";
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


        std::cout << "key int is ";

        
        for(int i=0; i < CryptoPP::AES::MAX_KEYLENGTH; i++){
            
            std::cout << int(key[i]) << ",";
        }


        std::cout << std::endl;
        
        std::cout << "key char is " << key << std::endl;
        
}

std::string API_service_data_handler::encode_base64(const std::string data) {
    static constexpr char sEncodingTable[] = {
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
      'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
      'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
      'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
      'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
      'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
      'w', 'x', 'y', 'z', '0', '1', '2', '3',
      '4', '5', '6', '7', '8', '9', '+', '/'
    };

    size_t in_len = data.size();
    size_t out_len = 4 * ((in_len + 2) / 3);
    std::string ret(out_len, '\0');
    size_t i;
    char *p = const_cast<char*>(ret.c_str());

    for (i = 0; i < in_len - 2; i += 3) {
      *p++ = sEncodingTable[(data[i] >> 2) & 0x3F];
      *p++ = sEncodingTable[((data[i] & 0x3) << 4) | ((int) (data[i + 1] & 0xF0) >> 4)];
      *p++ = sEncodingTable[((data[i + 1] & 0xF) << 2) | ((int) (data[i + 2] & 0xC0) >> 6)];
      *p++ = sEncodingTable[data[i + 2] & 0x3F];
    }
    if (i < in_len) {
      *p++ = sEncodingTable[(data[i] >> 2) & 0x3F];
      if (i == (in_len - 1)) {
        *p++ = sEncodingTable[((data[i] & 0x3) << 4)];
        *p++ = '=';
      }
      else {
        *p++ = sEncodingTable[((data[i] & 0x3) << 4) | ((int) (data[i + 1] & 0xF0) >> 4)];
        *p++ = sEncodingTable[((data[i + 1] & 0xF) << 2)];
      }
      *p++ = '=';
    }

    return ret;
}

std::string API_service_data_handler::decode_base64(const std::string& input, std::string& out) {
    static constexpr unsigned char kDecodingTable[] = {
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
      52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
      64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
      15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
      64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
      41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
      64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
    };

    size_t in_len = input.size();
    if (in_len % 4 != 0) return "Input data size is not a multiple of 4";

    size_t out_len = in_len / 4 * 3;
    if (input[in_len - 1] == '=') out_len--;
    if (input[in_len - 2] == '=') out_len--;

    out.resize(out_len);

    for (size_t i = 0, j = 0; i < in_len;) {
      uint32_t a = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
      uint32_t b = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
      uint32_t c = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];
      uint32_t d = input[i] == '=' ? 0 & i++ : kDecodingTable[static_cast<int>(input[i++])];

      uint32_t triple = (a << 3 * 6) + (b << 2 * 6) + (c << 1 * 6) + (d << 0 * 6);

      if (j < out_len) out[j++] = (triple >> 2 * 8) & 0xFF;
      if (j < out_len) out[j++] = (triple >> 1 * 8) & 0xFF;
      if (j < out_len) out[j++] = (triple >> 0 * 8) & 0xFF;
    }

    return "";
}


