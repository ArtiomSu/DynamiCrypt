#include "definitions.hpp"


boost::recursive_mutex read_lock;
boost::recursive_mutex write_lock;

boost::asio::io_service service;


array peers;

API_service_data_handler api_service_data_handler;

const int MAX_TPMS_PER_PEER = 1;
const int MAX_BUFF = 1024;
int PRINT_SYNC_MESSAGES =0; // 0 to disable sync messages as they can be quite spammy
const int SLOW_DOWN = 0; // slows down syncronisation by a lot use only when debugging

const std::string DYNAMICRYPT_API_WAIT = "DYNAMICRYPT_API_WAIT0b064b133c7d17706b05ca8076a2910be7a47b07feafe23655739bfb3a7302f5";
const std::string DYNAMICRYPT_API_FAILED_DECRYPT = "DYNAMICRYPT_API_FAILED_DECRYPTae8481c771da94b1274b177107ead49bbf7a116b61b8faecd1525b83e71b833b";
const std::string DYNAMICRYPT_API_ERROR = "DYNAMICRYPT_API_ERROR8074bae01e05967ef783344437a68677ba1fd4eaf7371199f4bb289d80fbf04e";

int PRINT_API_CRYPT_MESSAGES = 1;
int PRINT_KEYS_TO_EXTERNAL_GNOME_TERMINAL = 1;

void update_peers_changed() {
    array copy;
    { boost::recursive_mutex::scoped_lock lk(read_lock);
      copy = peers;
    }
    for( array::iterator b = copy.begin(), e = copy.end(); b != e; ++b){
        (*b)->set_peers_changed();
    }
}

int begin_sync(std::string address, int port, std::string service_name, std::string partner_name){
    try{
        peer::ptr initiating_peer = peer::new_(true, address, port);
        initiating_peer->start(service_name, partner_name);
    }
    catch(std::exception& e){
        return 0;
    }
    return 1;
}

int stop_sync(std::string service_name){
    array copy;
    { boost::recursive_mutex::scoped_lock lk(read_lock);
      copy = peers;
    }
    int found_service_in_peers = 0;
    for( array::iterator b = copy.begin(), e = copy.end(); b != e; ++b){
        if(!service_name.compare((*b)->get_service_name())){
            (*b)->stop();
            found_service_in_peers = 1;
        }
        //(*b)->set_peers_changed();
    }
    /*
    int found_service_in_api_service_data_handler = api_service_data_handler.remove_service(service_name);
    if(found_service_in_peers && found_service_in_api_service_data_handler){
        return 1;
    }else if(found_service_in_peers){
        return 2;
    }else if(found_service_in_api_service_data_handler){
        return 3;
    }else{
        return 0;
    }
     */
    if(found_service_in_peers){
        return 1;
    }else{
        return 0;
    }
    
    
}

/*
std::string test_api(){
    std::string test_string = "testing extern string";
    return test_string;
}*/

std::string hash_with_sha_256(std::string data){
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