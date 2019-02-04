/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: mrarchinton
 *
 * Created on 31 January 2019, 13:56
 */

#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>



using namespace std;


int main(int argc, char** argv) {
    boost::asio::io_context io;
    boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));
    t.wait();
    std::cout << "done" << std::endl;
    return 0;
}

