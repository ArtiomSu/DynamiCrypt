

#include "SingleTpmNetworkHandler.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/base64.h>
#include <sstream>
#include "System_Helper.hpp"

SingleTpmNetworkHandler::SingleTpmNetworkHandler(int id){
    iteration_ = 0;
    tpm_id_ = id;
    partner_tpm = 0;
    check_for_key_counter = 0;
    
    int initK = 6;  // k is hidden neurons	
    int initN = 12; // n is input neurons
    int initL = 4;   // range of weights
    

    tpm.K = initK;
    tpm.N = initN;
    tpm.L = initL;

    tpm.Initialize();
    tpm.RandomWeight();

    max_iterations_ = (tpm.L*tpm.L*tpm.L*tpm.L)*tpm.N*tpm.K;

    tmpinputvector.xLength(tpm.K, tpm.N);
       
    std::stringstream ss;
          
    std::string term = "gnome-terminal --geometry=95x27 > /dev/null 2>&1;test=\"$(ls /dev/pts)\";inarr=(${test});max=${ar[0]};for n in \"${inarr[@]}\" ; do ((n > max)) && max=$n; done; echo $max";
    ss << "/dev/pts/";
    ss << System_helper::exec(term.c_str());

    filename = ss.str();
    filename.erase(std::remove(filename.begin(), filename.end(), '\n'), filename.end());

    key_log.open(filename, std::ios::out);
    if (key_log.is_open()){
        std::cout << "writing to file" << std::endl;
        key_log << "Tpm Created with id: " << tpm_id_ << "\n";
    }else{
        std::cout << "cant write to file:" << filename << ":spacetest" << std::endl;
        std::exit(2);
    }
    key_log.close();
}

int SingleTpmNetworkHandler::increase_key_counter(){
    check_for_key_counter = check_for_key_counter+ 1;
    return check_for_key_counter;
}

int SingleTpmNetworkHandler::iteration_increase(){
    // 0 = too high need to reset tpm
    // 1 = everything ok continue,
    // ? = perform hash and check key

    int return_type = 0;
    if(iteration_ == max_iterations_){
        return_type = 0;
    } else{
       iteration_++;
       return_type = 1;
    }

    return return_type;
}

std::string SingleTpmNetworkHandler::get_key(){
    std::stringstream ss;
    for (int i=0; i <tpm.K * tpm.N ; i++) {
        ss << tpm.W.Z[i] + tpm.L;
    }
     //getWeightValue();
    return ss.str();
}

std::string SingleTpmNetworkHandler::SHA256(std::string data){
    CryptoPP::byte const* pbData = (CryptoPP::byte*)data.data();
    unsigned int nDataLen = data.length();
    CryptoPP::byte abDigest[CryptoPP::SHA256::DIGESTSIZE];

    CryptoPP::SHA256().CalculateDigest(abDigest, pbData, nDataLen);

    std::string raw((char*)abDigest, CryptoPP::SHA256::DIGESTSIZE);

    static const char* const lut = "0123456789ABCDEF";
    size_t len = raw.length();

    std::string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i)
    {
      const unsigned char c = raw[i];
      output.push_back(lut[c >> 4]);
      output.push_back(lut[c & 15]);
    }

    //std::cout << "crypt 1 : " << output <<std::endl;
    return output;
}

std::string SingleTpmNetworkHandler::calc_test(std::string randomInput){
    std::string hashed_key = SHA256(get_key());
    std::string hashed_input = SHA256(randomInput);

    std::stringstream ss;
    for(int i=0; i < hashed_key.length(); i++){
        ss << hashed_key.at(i) << hashed_input.at(hashed_input.length() -i -1);
    }

    return SHA256(ss.str());
}

int SingleTpmNetworkHandler::getWeightValue(){
    int val = 0;
    for (int i=0; i <tpm.K * tpm.N ; i++) {
        val = val + tpm.W.Z[i];
    }
    return val;   
}

int SingleTpmNetworkHandler::set_partner(int id){
    partner_tpm = id;
    return 1;
}

void SingleTpmNetworkHandler::reset(){
    iteration_ = 0;
    check_for_key_counter = 0;
    tpm.RandomWeight();
}

void SingleTpmNetworkHandler::reset_check_for_key_counter(){
   check_for_key_counter = 0;
}

std::string SingleTpmNetworkHandler::create_random_input_vector(){
    tmpinputvector.CreateRandomVector(tpm.K, tpm.N);
    return tmpinputvector.to_string(tpm.K, tpm.N);
}

int SingleTpmNetworkHandler::set_random_input_vector_from_string(std::string input){
    tmpinputvector.set_from_string(input, tpm.K, tpm.N);
    return 1;
}
             
int SingleTpmNetworkHandler::compute_tpm_result(){
    tpm.ComputeTPMResult(tmpinputvector.X);
    return tpm.TPMOutput;
}     

void SingleTpmNetworkHandler::update_weight(){
    tpm.UpdateWeight(tmpinputvector.X);
}
        
void SingleTpmNetworkHandler::add_key_to_proper_keys(std::string key){
    std::cout << "adding key to proper keys: " << key << std::endl;
    proper_keys.push_back(key);


    key_log.open(filename, std::ios::out);
    if (key_log.is_open()){
        std::cout << "writing to file" << std::endl;
        key_log << "tpm_id:" << tpm_id_ << " key:" << key << "\n";
    }else{
        std::cout << "cant write to file" << std::endl;
    }
    key_log.close();
}        
        
        
        
        