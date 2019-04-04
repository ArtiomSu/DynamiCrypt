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
        .route(desc.post("/init_config"), "Initiate Config")
        .bind(&API_service::initial_config, this)
        .produces(MIME(Application, Json))
        .consumes(MIME(Application, Json))
        .response(Pistache::Http::Code::Ok, "Initial Config")
        .response(backendErrorResponse);
    
    path
        .route(desc.post("/sync"), "Initiate Sync")
        .bind(&API_service::sync, this)
        .produces(MIME(Application, Json))
        .consumes(MIME(Application, Json))
        .response(Pistache::Http::Code::Ok, "Begin Sync")
        .response(backendErrorResponse);
    
    path 
        .route(desc.post("/status"), "check if connected to tpm ok")
        .bind(&API_service::status, this)
        .produces(MIME(Application, Json))
        .consumes(MIME(Application, Json))
        .response(Pistache::Http::Code::Ok, "status")
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
     * 
     {
	"service_name": "service1",
	"partner_name": "partner1",
	"address_of_other_tpm": "127.0.0.1",
	"port_of_other_tpm": 8006
     }
     
     
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
    
    
    /*
     called seperately from apps to create service_name
     then apps must communicate with each other to share this name
     */
    
    rapidjson::Document document;
    
   
    // make into json object
    char * jsonBody = new char [request.body().length()+1];
    strcpy (jsonBody, request.body().c_str());
    document.Parse(jsonBody);
    
    std::string service_name;
    int data_ok = 1;

    if(document.HasMember("service_name")){
        if(document["service_name"].IsString()){
            service_name = document["service_name"].GetString();
        }
        else{
            data_ok = 0;
        }
    }
    else{
        data_ok = 0;
    }
    
   
    std::string respond_service_name;
    rapidjson::StringBuffer buffera;
    rapidjson::Writer<rapidjson::StringBuffer> writera(buffera);
    
    
    if(data_ok){
        respond_service_name = api_service_data_handler.new_service(service_name);
        writera.StartObject(); 
        writera.Key("service_name");                
        writera.String(respond_service_name.c_str(), respond_service_name.length());
        writera.Key("address_of_this_tpm");                
        writera.String(api_service_data_handler.get_sync_address().c_str(), api_service_data_handler.get_sync_address().length());
        writera.Key("port_of_this_tpm");
        writera.Uint(api_service_data_handler.get_sync_port());
        writera.EndObject();
        
    }
    
    else{//error with request
        writera.StartObject(); 
        writera.Key("error");                
        writera.String("invalid request");
        writera.EndObject();
    }
    
    response.send(Pistache::Http::Code::Ok, buffera.GetString());
}

void API_service::initial_config(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    /*
     called when apps have gotten each other service name this is used to update said name in api_service_data_handler
     */
    rapidjson::Document document;
    char * jsonBody = new char [request.body().length()+1];
    strcpy (jsonBody, request.body().c_str());
    document.Parse(jsonBody);
    
    std::string service_name;
    std::string partner_name;
    int data_ok = 1;
    if(document.HasMember("service_name")){
        if(document["service_name"].IsString()){
            service_name = document["service_name"].GetString();
        }
        else{
            data_ok = 0;  
        }
    }
    else{
        data_ok = 0;
    }

    if(document.HasMember("partner_name")){
        if(document["partner_name"].IsString()){
            partner_name = document["partner_name"].GetString();
        }else{
            data_ok = 0;  
        }
    }
    else{
        data_ok = 0;
    }
    
    rapidjson::StringBuffer buffera;
    rapidjson::Writer<rapidjson::StringBuffer> writera(buffera);
    
    if(data_ok){
        if(api_service_data_handler.check_service(service_name)){
            api_service_data_handler.update_partner(service_name, partner_name);
            writera.StartObject(); 
            writera.Key("status");                
            writera.Uint(1);
            writera.EndObject();
        }else{
            writera.StartObject(); 
            writera.Key("status");                
            writera.Uint(0);
            writera.EndObject();
        }
    }
    else{//error with request
        writera.StartObject(); 
        writera.Key("error");                
        writera.String("invalid request");
        writera.EndObject();
    }
    
    response.send(Pistache::Http::Code::Ok, buffera.GetString());
}

void API_service::sync(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    /*
     one app essentially calls this to begin sync. 
     however app must check if sync servers connected ok by calling the status route.
     
     if sync servers not connected then whole process must begin again by generating new name.
     Its prob ok to leave the partner name as is. 
     
      
    */
    rapidjson::Document document;
    char * jsonBody = new char [request.body().length()+1];
    strcpy (jsonBody, request.body().c_str());
    document.Parse(jsonBody);
    
    std::string service_name;
    std::string partner_name;
    std::string address_of_other_tpm;
    int port_of_other_tpm;
    
    int data_ok = 1;
    
    if(document.HasMember("service_name")){
        if(document["service_name"].IsString()){
            service_name = document["service_name"].GetString();
        }
        else{
            data_ok = 0;
        }
    }
    else{
        data_ok = 0;
    }
    
   
    if(document.HasMember("partner_name")){
        if(document["partner_name"].IsString()){
            partner_name = document["partner_name"].GetString();
        }
        else{
            data_ok = 0;
        }
    }
    else{
        data_ok = 0;
    }
    
    if(document.HasMember("address_of_other_tpm")){
        if(document["address_of_other_tpm"].IsString()){
            address_of_other_tpm = document["address_of_other_tpm"].GetString();
        }
        else{
            data_ok = 0;
        }
    }
    else{
        data_ok = 0;
    }
    
    
    if(document.HasMember("port_of_other_tpm")){
        if(document["port_of_other_tpm"].IsNumber() && document["port_of_other_tpm"].IsInt()){
            port_of_other_tpm = document["port_of_other_tpm"].GetInt();
        }
        else{
            data_ok = 0;
        }
    }
    else{
        data_ok = 0;
    }
    
    rapidjson::StringBuffer buffera;
    rapidjson::Writer<rapidjson::StringBuffer> writera(buffera);
    
    if(data_ok){
        begin_sync(address_of_other_tpm, port_of_other_tpm, service_name, partner_name);
        writera.StartObject(); 
        writera.Key("status");                
        writera.Uint(1);
        writera.EndObject();
    }else{
        writera.StartObject(); 
        writera.Key("error");                
        writera.String("invalid request");
        writera.EndObject();
    }
    
    response.send(Pistache::Http::Code::Ok, buffera.GetString());
    
}


void API_service::status(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {
    /*
     Due to callback pattern it is necessary for the client to see if the connection between the tpms was successful
     if not then address or port could be wrong. 
     
     this will be done by checking if the server_name exists. 
     
     */
    rapidjson::StringBuffer buffera;
    rapidjson::Writer<rapidjson::StringBuffer> writera(buffera);
    rapidjson::Document document;
    char * jsonBody = new char [request.body().length()+1];
    strcpy (jsonBody, request.body().c_str());
    document.Parse(jsonBody);
    
    int ok = 0;
    
    if(document.HasMember("service_name")){
        if(document["service_name"].IsString()){
            if(api_service_data_handler.check_service(document["service_name"].GetString())){
                ok = 1;
            }
        }
    }

    writera.StartObject();                
    writera.Key("status");
    writera.Uint(ok);
    writera.EndObject();

    response.send(Pistache::Http::Code::Ok, buffera.GetString());
}

void API_service::encrypt(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response) {  
    /*
     this will probably be the same thing as decrypt
     get
     nameOfService/id
     message 
     mode
     operation
     hash need hash if operation is decrypt 
      
      
     keep checking for a key probably best if sleep wasn't used but a well
     when key found use that key to encrypt and move key to a different variable or something, 
     maybe use same key again or something while new key is not received
     depending on the mode.
     
     send
     message
     mode
       
     */
    rapidjson::Document document;
    char * jsonBody = new char [request.body().length()+1];
    strcpy (jsonBody, request.body().c_str());
    document.Parse(jsonBody);
    
    std::string service_name;
    std::string message;
    std::string hash = "";
    int mode;
    int operation; // encrypt 0, decrypt 1
    
    int data_ok = 1;
    
    if(document.HasMember("service_name")){
        if(document["service_name"].IsString()){
            service_name = document["service_name"].GetString();
        }
        else{
            data_ok = 0;
        }
    }
    else{
        data_ok = 0;
    }
    
    
    if(document.HasMember("message")){
        if(document["message"].IsString()){
            message = document["message"].GetString();
        }
        else{
            data_ok = 0;
        }
    }
    else{
        data_ok = 0;
    }
    
    
    if(document.HasMember("mode")){
        if(document["mode"].IsNumber() && document["mode"].IsInt()){
            mode = document["mode"].GetInt();
        }
        else{
            data_ok = 0;
        }
    }
    else{
        data_ok = 0;
    }
    
    if(document.HasMember("operation")){
        if(document["operation"].IsNumber() && document["operation"].IsInt()){
            operation = document["operation"].GetInt();
        }
        else{
            data_ok = 0;
        }
    }
    else{
        data_ok = 0;
    }
    
    if(document.HasMember("hash")){
        if(document["hash"].IsString()){
            hash = document["hash"].GetString();
        }
        else{
            data_ok = 0;
        }
    }
    else if(data_ok && operation == 0){ // hash should only be here if operation is decrypt 
        data_ok = 1;
    }else{
        data_ok = 0;
    }
    
    
    
    
    rapidjson::StringBuffer buffera;
    rapidjson::Writer<rapidjson::StringBuffer> writera(buffera);
    
    if(data_ok){
        std::string result = api_service_data_handler.crypt(service_name, message, hash, mode, operation);
        if(result == DYNAMICRYPT_API_ERROR){
            writera.StartObject(); 
            writera.Key("error");                
            writera.String("service_not_found");
            writera.EndObject();
        }else if(result == DYNAMICRYPT_API_WAIT){
            writera.StartObject(); 
            writera.Key("error");                
            writera.String("wait");
            writera.EndObject();
        }
        else if(result == DYNAMICRYPT_API_FAILED_DECRYPT){
            writera.StartObject(); 
            writera.Key("error");                
            writera.String("failed_decrypt");
            writera.EndObject();
        }
        else{
            writera.StartObject();
            writera.Key("message");                
            writera.String(result.c_str(), result.length());
            if(operation == 0){ //send hash when encrypting
            writera.Key("hash");
            std::string temp = hash_with_sha_256(message);
            writera.String(temp.c_str(), temp.length());
            }
            writera.Key("mode");
            writera.Uint(mode);
            writera.EndObject();
        }
    } else{
        writera.StartObject(); 
        writera.Key("error");                
        writera.String("invalid request");
        writera.EndObject();
    }
    
    
    
    
    
    response.send(Pistache::Http::Code::Ok,  buffera.GetString());
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
    
    //std::string service_name= api_service_data_handler.new_service("Service1", "Partnerservice2");
    
    //int ok = api_service_data_handler.add_key(service_name, "gdkjghdkh395dksbskjfb39fskfhjkfjh39493457938rskjfh39");
    
    response.send(Pistache::Http::Code::Forbidden, "This is not the route you are looking for ");
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


    
    