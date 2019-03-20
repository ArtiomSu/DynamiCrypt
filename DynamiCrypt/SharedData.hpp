/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SharedData.hpp
 * Author: mrarchinton
 *
 * Created on 20 March 2019, 13:22
 */

#ifndef SHAREDDATA_HPP
#define SHAREDDATA_HPP

#include <cstdio>
#include <string>
#include <rapidjson/document.h>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

class SharedData{
public:
    SharedData(int num, std::string value){
        this->num = num;
        this->value = value;
    }
    
    SharedData(){
        this->num = 0;
        this->value = "";
    }
    
    SharedData(std::string value){
        this->num = 0;
        this->value = value;
    }
    
    void increaseNum(){
        this->num++;
    }
    
    int getNum(){
        return this->num;
    }
    
    void setNum(int num){
        this->num = num;
    }
    
    void setValue(std::string value){
        this->value = value;
    }
    
    std::string getValue(){
        return this->value;
    }
    
    template <typename Writer>
    void Serialize(Writer& writer) const {
        // This base class just write out name-value pairs, without wrapping within an object.
       /* writer.String("value");
        writer.String(value.c_str(), value.length());
        writer.String("num");
        writer.Uint(num);
        */
        writer.StartObject(); 
        writer.Key("value");                
        writer.String(value.c_str(), value.length());
        writer.Key("num");
        writer.Uint(num);
        writer.EndObject();
    }
    
    
    
private:
    std::string value;
    int num;    
};

#endif /* SHAREDDATA_HPP */

