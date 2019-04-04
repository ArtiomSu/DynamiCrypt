#include "TpmNetworkHandler.hpp"
#include "SingleTpmNetworkHandler.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <sstream>
#include "definitions.hpp"
TpmNetworkHandler::TpmNetworkHandler(){
    
}

int TpmNetworkHandler::create_new_tpm(std::string service_name, std::string partner_name){
    int randomId = rand()%5000;
    while(find_tpm(randomId, false) != -1){
        randomId = rand()%5000;
    }

    tpm_networks_.push_back(SingleTpmNetworkHandler(randomId, service_name, partner_name));
    std::cout << "created new tpm with id " << randomId << std::endl;
    return randomId;
}
        
int TpmNetworkHandler::find_tpm(int id, bool find_partner){
    for(int i=0; i < tpm_networks_.size(); i++){
            
        if(find_partner){
            if(tpm_networks_[i].partner() == id){

                return i;
            }
        } else {

            if(tpm_networks_[i].id() == id){

                return i;
            }

        }
    }
    return -1;
}    
        
int TpmNetworkHandler::increase_iteration(int id){
    int index = find_tpm(id, false);
    if(tpm_networks_[index].iteration_increase()){

        return 1;
    }else{
        reset_tpm(index);
        //tpm_networks_.erase(tpm_networks_.begin()+index);
        return 0; 
    }
}       
        
int TpmNetworkHandler::reset_tpm(int index){
    tpm_networks_[index].reset();
    return 1;
} 
        
int TpmNetworkHandler::get_max_iterations(int index){
    return tpm_networks_[index].get_max_iterations();
}        
        
int TpmNetworkHandler::getid(int index){
    return tpm_networks_[index].id();    
}        
        
int TpmNetworkHandler::get_iteration(int id){
    int index = find_tpm(id, false);
    if(index == -1 ){
        return index;
    }
    else{
        return tpm_networks_[index].iteration();
    }
}        
        
int TpmNetworkHandler::set_partner(int id, int partner_id){
    int index = find_tpm(id, false);
    tpm_networks_[index].set_partner(partner_id);
    return 1;
}           
        
int TpmNetworkHandler::get_partner(int id){
    int index = find_tpm(id, false);
    if(index == -1) {
        return index;
    }
    return tpm_networks_[index].partner();
}           
        
SingleTpmNetworkHandler* TpmNetworkHandler::get_tpm(int id){
    int index = find_tpm(id, false);
    if(index != -1){
       return &tpm_networks_[index];
    } else{
        return NULL;
    }
}           
        
int TpmNetworkHandler::increase_key_counter(int index){
    return tpm_networks_[index].increase_key_counter();    
}           
        
std::string TpmNetworkHandler::get_key_tpm(int index){
    return tpm_networks_[index].get_key();    
}           
        
std::string TpmNetworkHandler::calc_test(int index, std::string input){
    return tpm_networks_[index].calc_test(input);
}        
        
std::string TpmNetworkHandler::sync_tpm_message_one(int tpm_id){
    std::stringstream ss;
    int index = find_tpm(tpm_id, false);

    std::string random_input_vector = tpm_networks_[index].create_random_input_vector();
    int tpm_result = tpm_networks_[index].compute_tpm_result();

    // id, itteration, randomVector, resultOfVector
    ss << "1\t" << tpm_id << "\t" << tpm_networks_[index].id() << "\t" << tpm_networks_[index].iteration() << "\t" << random_input_vector << "\t" << tpm_result << "\t" << 1 << "\n";
    //     0         1                       2                                       3                                      4                           5                  6 messagetype to process
    // parsed message ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    if(PRINT_SYNC_MESSAGES){
    std::cout << "sync_tpm_message_one end" << std::endl;
    }
    return ss.str();

    // sends random vector and result to said random vector but doesn't update weights.
}        
        
std::string TpmNetworkHandler::gen_random_input_for_key(int lengthOfKey){
    const char Dictionary [] = "0123456789_abcdefghijklmnopqrstuvwxyz{}_()&*!£$%^;:'@#~<>,.?/|;";
    std::stringstream ss;
    int previous = -1;
    int this_time = -1;
    for(int i=0; i< lengthOfKey; i++){
        int loop = 1;
        while(loop){

            this_time = rand() % (sizeof(Dictionary) -1) ;
            if(this_time < 0 || this_time == previous){
                loop = 1;
            }else{
                loop = 0;
            }


        }
        previous == this_time;
        //std::cout << this_time << std::endl;
        ss << Dictionary[previous];
    }
    return ss.str();
}        
        
std::string TpmNetworkHandler::sync_tpm_message_one_advanced(int tpm_id, std::vector<std::string> & parsed_msg){
    // receives random vector and partners result to that vector 
    // use that vector and test ouput
    // if output == this output then update weights 
    // tell the other machine to update too by using some variable

    // then for this itteration create random vector, compute result 
    // send the new vector and new result too.

    // at the end send
    // old_random_vector, tell_machine_to_update_or_not_using_the_old_vector, new_vector, new_TPM_output.

    // maybe dont send old vector as it is not really needed

    std::stringstream ss;
    int index = find_tpm(tpm_id, false);
    int tell_machine_to_update = 0;
    int tpm_result = 0;
    int message_type_to_process = 0;
    std::string random_input_vector;
    std::string old_input_vector;

    std::string random_input_for_key;
    std::string key_hash;

    int processing_message_type_now = std::stoi(parsed_msg.at(6));



    //std::string key;

    if(processing_message_type_now == 1){ // dont update ur weights straight away
        std::string random_vector_received = parsed_msg.at(4);
        old_input_vector = random_vector_received;
        int result_received = std::stoi(parsed_msg.at(5));
        tpm_networks_[index].set_random_input_vector_from_string(random_vector_received);
        int tpm_result_old = tpm_networks_[index].compute_tpm_result();

        if(tpm_result_old == result_received){
            tpm_networks_[index].update_weight();
            tell_machine_to_update = 1;
        }        


        random_input_vector = tpm_networks_[index].create_random_input_vector();
        tpm_result = tpm_networks_[index].compute_tpm_result();

        if(PRINT_SYNC_MESSAGES){
        std::cout << "sync_tpm_message_one_advanced-1 before getKey" << std::endl;
        }
        message_type_to_process = 2;
        //key = tpm_networks_[index].get_key();
        if(PRINT_SYNC_MESSAGES){
        std::cout << "tpm key gotten ok"<< std::endl;
        }
        random_input_for_key = gen_random_input_for_key(50);
        if(PRINT_SYNC_MESSAGES){
        std::cout << "random_input_gotten_ok"<< std::endl;
        }
        key_hash = tpm_networks_[index].calc_test(random_input_for_key);
        if(PRINT_SYNC_MESSAGES){
        std::cout << "key_hash_gotten_ok"<< std::endl;
        std::cout << "sync_tpm_message_one_advanced-1 end" << std::endl;
        }
        //std::cout << "\n\nok\n"; 
    } 

    // update on old weight which should be still present in the tpm

    else if(processing_message_type_now == 2) {

        tell_machine_to_update = std::stoi(parsed_msg.at(7));
        //std::cout << "\n\nokkkk\n"; 
        if(tell_machine_to_update == 1){
            old_input_vector = parsed_msg.at(8);
            tpm_networks_[index].set_random_input_vector_from_string(old_input_vector);
            if(PRINT_SYNC_MESSAGES){
            std::cout << "\nupdating machine with this vector \n" << old_input_vector << "\n";
            }
            tpm_networks_[index].update_weight();

            std::string other_key = parsed_msg.at(9);
            //key = tpm_networks_[index].get_key();

            random_input_for_key = parsed_msg.at(10);
            key_hash = parsed_msg.at(9);

            std::string key_hash_this = tpm_networks_[index].calc_test(random_input_for_key);



            if (!key_hash.compare(key_hash_this)) {
                // keys are the same
                if(PRINT_SYNC_MESSAGES){
                std::cout << "found same key hash : " << key_hash_this << std::endl;
                }
                tpm_networks_[index].add_key_to_proper_keys(tpm_networks_[index].get_key());
                //std::exit(1);

                // reset tpm
                tpm_networks_[index].reset();
                ss << "4\t" << tpm_id << "\t" << tpm_networks_[index].iteration() << "\t" << 0 << "\t" << 1 << "\n";
                //      0       1                   2                                         3            4 //save current key
                return ss.str();
            }

        }




        tell_machine_to_update = 0;

        std::string random_vector_received = parsed_msg.at(4);
        old_input_vector = random_vector_received;

        int result_received = std::stoi(parsed_msg.at(5));

        tpm_networks_[index].set_random_input_vector_from_string(random_vector_received);
        int tpm_result_old = tpm_networks_[index].compute_tpm_result();

        if(tpm_result_old == result_received){
            tpm_networks_[index].update_weight();
            tell_machine_to_update = 1;
        }        


       // then continue as normal and send back the result for new vector
        random_input_vector = tpm_networks_[index].create_random_input_vector();
        tpm_result = tpm_networks_[index].compute_tpm_result();
        //key = tpm_networks_[index].get_key();

        random_input_for_key = gen_random_input_for_key(50);
        key_hash = tpm_networks_[index].calc_test(random_input_for_key);
        // send id, id again? lol, iteration, reandom-vector, tpm result,   tell machine to update doesnt matter, message type should be 1 since this is basically the same as sync_tpm_message_one
        message_type_to_process = 2;

    }

    if(PRINT_SYNC_MESSAGES){
    std::cout << "\niteration " << tpm_networks_[index].iteration() << "message type: " << message_type_to_process << "tell_machine_to_update " << tell_machine_to_update << "\nkey_hashed: " << key_hash << "\nold: " << old_input_vector << "\nnew:" << random_input_vector << "\n\n";  
    }

    // mostly compatible with sync_tpm_message_one message.
    ss << "1\t" << tpm_id << "\t" << tpm_networks_[index].id() << "\t" << tpm_networks_[index].iteration() <<  "\t" << random_input_vector << "\t" << tpm_result << "\t" << message_type_to_process  << "\t" << tell_machine_to_update<< "\t" << old_input_vector << "\t" << key_hash << "\t" << random_input_for_key << "\n";
    //     0         1                       2                                       3                                      4                           5                                   6                          7                               8                        9                    10                      11
    // parsed message ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


    return ss.str();
        
}        
      
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        