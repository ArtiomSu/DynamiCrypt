#include "definitions.hpp"


boost::recursive_mutex read_lock;
boost::recursive_mutex write_lock;

boost::asio::io_service service;


array peers;

API_service_data_handler api_service_data_handler;

const int MAX_TPMS_PER_PEER = 1;
const int MAX_BUFF = 1024;
const int PRINT_SYNC_MESSAGES =0; // 0 to disable sync messages as they can be quite spammy
const int SLOW_DOWN = 0;

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

std::string test_api(){
    std::string test_string = "testing extern string";
    return test_string;
}

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