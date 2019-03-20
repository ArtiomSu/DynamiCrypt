/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SharedDataContainer.hpp
 * Author: mrarchinton
 *
 * Created on 20 March 2019, 13:32
 */

#ifndef SHAREDDATACONTAINER_HPP
#define SHAREDDATACONTAINER_HPP


#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include <rapidjson/document.h>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "SharedData.hpp"

class SharedDataContainer{
public:
    SharedDataContainer(int active){
        this->active = active;
    }
    
    void ToggleActive(){
        if(active){
            active = 0;
        }else{
            active = 1;
        }  
    }

    int getActive(){
        return active;
    }
    
    void AddSharedData(SharedData data){
        sharedDatas.push_back(data);
    }
    
    std::vector<SharedData> getSharedDatas(){
        return sharedDatas;
    }
    
    template <typename Writer>
    void Serialize(Writer& writer) const {
        /*
        writer.StartObject();

//        sharedDatas.back().Serialize(writer);
       // SharedData::Serialize(writer);

        writer.String("active");
        writer.Uint(active);

        writer.String(("sharedData"));
        writer.StartArray();
        for (std::vector<SharedData>::const_iterator sharedDataItr = sharedDatas.begin(); sharedDataItr != sharedDatas.end(); ++sharedDataItr)
            sharedDataItr->Serialize(writer);
        writer.EndArray();

        writer.EndObject();
         * 
         * */
        
        writer.StartObject();

//        sharedDatas.back().Serialize(writer);
       // SharedData::Serialize(writer);

        writer.Key("active");
        writer.Uint(active);

        writer.Key("sharedData");
        writer.StartArray();
        for (std::vector<SharedData>::const_iterator sharedDataItr = sharedDatas.begin(); sharedDataItr != sharedDatas.end(); ++sharedDataItr)
            sharedDataItr->Serialize(writer);
        writer.EndArray();

        writer.EndObject();
        
        
        
    }
private:
    int active;
    std::vector<SharedData> sharedDatas;
};

#endif /* SHAREDDATACONTAINER_HPP */

