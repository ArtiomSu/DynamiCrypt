/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   JSON_helper.hpp
 * Author: mrarchinton
 *
 * Created on 20 March 2019, 13:08
 */

#ifndef JSON_HELPER_HPP
#define JSON_HELPER_HPP

#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include <rapidjson/document.h>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "SharedDataContainer.hpp"

class JSON_helper{
public:
    JSON_helper();
    
    static std::vector<SharedDataContainer> generateTestContainer();
    
    static rapidjson::StringBuffer vectorToJson(std::vector<SharedDataContainer> dataContainers);
    
    static std::vector<SharedDataContainer> jsonToVector(rapidjson::Document *d);
    
};



#endif /* JSON_HELPER_HPP */

