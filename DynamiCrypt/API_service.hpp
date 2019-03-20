/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   API_service.hpp
 * Author: mrarchinton
 *
 * Created on 20 March 2019, 14:13
 */

#ifndef API_SERVICE_HPP
#define API_SERVICE_HPP

#include <pistache/http.h>
#include <pistache/description.h>
#include <pistache/endpoint.h>

#include <pistache/serializer/rapidjson.h>
#include <rapidjson/document.h>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include <pistache/router.h>

class API_service {
public:
    API_service(Pistache::Address addr)
        : httpEndpoint(std::make_shared<Pistache::Http::Endpoint>(addr))
        , desc("DynamiCrypt API", "0.1")
    { }

    void init(size_t thr);

    void start();

    void shutdown();

private:
    void createDescription();

    void initial(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);

    void the_rest(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    
    void encrypt(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    
    void route_test(const Pistache::Rest::Request&, Pistache::Http::ResponseWriter response);
    
    void leave(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
    
    std::shared_ptr<Pistache::Http::Endpoint> httpEndpoint;
    Pistache::Rest::Description desc;
    Pistache::Rest::Router router;
};


#endif /* API_SERVICE_HPP */

