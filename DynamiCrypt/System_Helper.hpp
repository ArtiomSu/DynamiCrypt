/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   System_Helper.hpp
 * Author: mrarchinton
 *
 * Created on 11 March 2019, 15:33
 */

#ifndef SYSTEM_HELPER_HPP
#define SYSTEM_HELPER_HPP

#include <string>

class System_helper{
public:
    System_helper();
    static std::string exec(const char* cmd);
};

#endif /* SYSTEM_HELPER_HPP */

