/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Server2.cpp
 * Author: mrarchinton
 * 
 * Created on 28 January 2019, 17:53
 */

#include "APIServer.h"
#include <pistache/http.h>
#include <pistache/endpoint.h>
#include <cstdio>
#include "API_service.hpp"

APIServer::APIServer(int port_number) {
    Pistache::Port port(port_number);
    int thr = 2;
    Pistache::Address addr(Pistache::Ipv4::any(), port);
    //cout << "Cores = " << hardware_concurrency() << endl;
    std::cout << "API Using " << thr << " threads" << std::endl;
    API_service api(addr);
    api.init(thr);
    api.start();
    api.shutdown();
}

APIServer::APIServer(const APIServer& orig) {
}

APIServer::~APIServer() {
}

