/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "API_service.hpp"
#include "JSON_helper.hpp"
#include "definitions.hpp"
#include <string>

void API_service::init(size_t thr = 2) {
    auto opts = Pistache::Http::Endpoint::options()
        .threads(thr)
        .flags(Pistache::Tcp::Options::InstallSignalHandler);
    httpEndpoint->init(opts);
    createDescription();
}

void API_service::start() {
    router.initFromDescription(desc);
    httpEndpoint->setHandler(router.handler());
    httpEndpoint->serve();
}

void API_service::shutdown() {
    httpEndpoint->shutdown();
}

void API_service::createDescription() {
    desc
        .info()
        .license("Apache", "http://www.apache.org/licenses/LICENSE-2.0");

    auto backendErrorResponse =
        desc.response(Pistache::Http::Code::Internal_Server_Error, "An error occured with the backend");

    desc
        .schemes(Pistache::Rest::Scheme::Http)
        .basePath("/v1")
        .produces(MIME(Application, Json))
        .consumes(MIME(Application, Json));

    auto versionPath = desc.path("/v1");

    auto path = versionPath.path("/options");

    path
        .route(desc.get("/test-ok"))
        .bind(&API_service::route_test, this)
        .produces(MIME(Application, Json), MIME(Application, Xml))
        .response(Pistache::Http::Code::Ok, "ok");
    
    path
        .route(desc.post("/init"), "Initiate Communication")
        .bind(&API_service::initial, this)
        .produces(MIME(Application, Json))
        .consumes(MIME(Application, Json))
        .response(Pistache::Http::Code::Ok, "Initial request")
        .response(backendErrorResponse);
    
    path
        .route(desc.post("/encrypt"), "Encrypt")
        .bind(&API_service::encrypt, this)
        .produces(MIME(Application, Json))
        .consumes(MIME(Application, Json))
        .response(Pistache::Http::Code::Ok, "encrypted")
        .response(backendErrorResponse);
    
    path
        .route(desc.post("/exit"), "Initiate deletion")
        .bind(&API_service::leave, this)
        .produces(MIME(Application, Json))
        .consumes(MIME(Application, Json))
        .response(Pistache::Http::Code::Ok, "Bye")
        .response(backendErrorResponse);
    
    path
        .route(desc.post("/:rest"), "Caught unknown option")
        .bind(&API_service::the_rest, this)
        .produces(MIME(Application, Json))
        .consumes(MIME(Application, Json))
        .response(Pistache::Http::Code::Ok, "The unknown options")
        .response(backendErrorResponse);

}

void API_service::route_test(const Pistache::Rest::Request&, Pistache::Http::ResponseWriter response) {
    response.send(Pistache::Http::Code::Ok, "ok");
}

void API_service::initial(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    /*
     maybe have some modes maybe even change these modes for each encryption request?
     1: speed : same key will be used until new key is generated
     2: security : same key is never used must wait for new key
     3: fast security : same key can be used n number of times. n is specified by the client. 
                        when n keys are used must wait for new key or if less than n keys are used then just use new key straight away.
     4: super security : split message into pieces and encrypt each piece with different key. Will need to wait for some keys 
                         and will need to have a certain kind of structure so its possible to decrypt so probably wont be done in time.  
      
     need following info 
     nameOfService of self (maybe id) // better to generate this here though to avoid conflicts
     nameOfService of other API consumer?
     address and port of the other sync server
     
     
     send back
     newly generated id based on the provided service name
     try launch the peer and if its fine then send ok or something
     * 
     * 
     
     if this is the one who will be initiating he wont have info from the other dude so give him sum
     get
     nameOfservice 
      
     
     send back
     generated id  
     port of this sync server
     
     then he can send this over to the other API
       
     */
    
    response.send(Pistache::Http::Code::Ok, "ok");
}


void API_service::encrypt(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {  
    /*
     this will probably be the same thing as decrypt
     get
     nameOfService/id
     message 
     mode
      
      
      
     keep checking for a key probably best if sleep wasn't used but a well
     when key found use that key to encrypt and move key to a different variable or something, 
     maybe use same key again or something while new key is not received
     depending on the mode.
     
     send
     message
     mode
       
     */
    std::string service_name = api_service_data_handler.new_service("Service1", "Partnerservice2");
    
    int ok = api_service_data_handler.add_key(service_name, "gdkjghdkh395dksbskjfb39fskfhjkfjh39493457938rskjfh39");
    
    if(ok){
        std::cout << "key added ok" << std::endl;
    }
    
    api_service_data_handler.crypt(service_name,"Hello there Whats the story Hello there Whats the story Hello there Whats the story Hello there Whats the story Hello there Whats the story Hello there Whats the story Hello there Whats the story Hello there Whats the story Hello there Whats the story Hello there Whats the story Hello there Whats the story Hello there Whats the story Hello there Whats the story Hello there Whats the story Hello there Whats the story Hello there Whats the story Hello there Whats the story",1,1);
    
    
    response.send(Pistache::Http::Code::Ok, "ok");
}


void API_service::leave(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    /*
     get 
     nameofservice/id
     tell sync server to stop syncronising. This isn't supported yet maybe try call stop on the peer
     remove object (no class built yet to handle api service?) and keys and peer itself
     
     
      
     send
     nothing really needed so ok will do
     */
    
    response.send(Pistache::Http::Code::Ok, "ok");
}


void API_service::the_rest(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    // might add some functionality here that sends list of correct routes or something
    response.send(Pistache::Http::Code::Forbidden, "This is not the route you are looking for");
}

/*
void API_service::createAccount(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    std::cout << "inside Create account";
    rapidjson::Document d;
    const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";


    // to get the yoke after /v1/accounts/
    std::string identity = request.body().substr(request.body().find("/") + 13); 

    // make into json object
    char * jsonBody = new char [request.body().length()+1];
    strcpy (jsonBody, request.body().c_str());
    d.Parse(jsonBody);
    std::cout << jsonBody;


    /*
    // modify
    rapidjson::Value& s = d["stars"];
    s.SetInt(s.GetInt() + 1);

    rapidjson::Value& p = d["project"];
    p.SetString("ooblock");
    


    //vector<SharedDataContainer> dataContainers = generateTestContainer();

    std::vector<SharedDataContainer> dataContainers = JSON_helper::jsonToVector(&d);

    rapidjson::StringBuffer buffera = JSON_helper::vectorToJson(dataContainers); 

    response.send(Pistache::Http::Code::Ok, buffera.GetString());
    //***************************************************************************************************
    //END OF read large set and Send back


}
*/


    
    