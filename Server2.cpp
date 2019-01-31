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

#include "Server2.h"
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

using namespace std;
using namespace Pistache;

namespace Generic {

void handleReady(const Rest::Request&, Http::ResponseWriter response) {
    response.send(Http::Code::Ok, "1");
}

}

class SharedData {
public:
    SharedData(int num, string value){
        this->num = num;
        this->value = value;
    }
    
    SharedData(){
        this->num = 0;
        this->value = "";
    }
    
    SharedData(string value){
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
    
    void setValue(string value){
        this->value = value;
    }
    
    string getValue(){
        return this->value;
    }
    
//protected:
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
    string value;
    int num;

   
};


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
    
    vector<SharedData> getSharedDatas(){
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
    vector<SharedData> sharedDatas;
};

class BankerService {
public:
    BankerService(Address addr)
        : httpEndpoint(std::make_shared<Http::Endpoint>(addr))
        , desc("Banking API", "0.1")
    { }

    void init(size_t thr = 2) {
        auto opts = Http::Endpoint::options()
            .threads(thr)
            .flags(Tcp::Options::InstallSignalHandler);
        httpEndpoint->init(opts);
        createDescription();
    }

    void start() {
        router.initFromDescription(desc);

        Rest::Swagger swagger(desc);
        swagger
            .uiPath("/doc")
            .uiDirectory("/home/octal/code/web/swagger-ui-2.1.4/dist")
            .apiPath("/banker-api.json")
            .serializer(&Rest::Serializer::rapidJson)
            .install(router);

        httpEndpoint->setHandler(router.handler());
        httpEndpoint->serve();
    }

    void shutdown() {
        httpEndpoint->shutdown();
    }

private:
    void createDescription() {
        desc
            .info()
            .license("Apache", "http://www.apache.org/licenses/LICENSE-2.0");

        auto backendErrorResponse =
            desc.response(Http::Code::Internal_Server_Error, "An error occured with the backend");

        desc
            .schemes(Rest::Scheme::Http)
            .basePath("/v1")
            .produces(MIME(Application, Json))
            .consumes(MIME(Application, Json));

        desc
            .route(desc.get("/ready"))
            .bind(&Generic::handleReady)
            .response(Http::Code::Ok, "Response to the /ready call")
            .hide();

        auto versionPath = desc.path("/v1");

        auto accountsPath = versionPath.path("/accounts");

        accountsPath
            .route(desc.get("/all"))
            .bind(&BankerService::retrieveAllAccounts, this)
            .produces(MIME(Application, Json), MIME(Application, Xml))
            .response(Http::Code::Ok, "The list of all account");

        accountsPath
            .route(desc.get("/:name"), "Retrieve an account")
            .bind(&BankerService::retrieveAccount, this)
            .produces(MIME(Application, Json))
            .parameter<Rest::Type::String>("name", "The name of the account to retrieve")
            .response(Http::Code::Ok, "The requested account")
            .response(backendErrorResponse);

        accountsPath
            .route(desc.post("/:name"), "Create an account")
            .bind(&BankerService::createAccount, this)
            .produces(MIME(Application, Json))
            .consumes(MIME(Application, Json))
            .parameter<Rest::Type::String>("name", "The name of the account to create")
            .response(Http::Code::Ok, "The initial state of the account")
            .response(backendErrorResponse);

        auto accountPath = accountsPath.path("/:name");
        accountPath.parameter<Rest::Type::String>("name", "The name of the account to operate on");

        accountPath
            .route(desc.post("/budget"), "Add budget to the account")
            .bind(&BankerService::creditAccount, this)
            .produces(MIME(Application, Json))
            .response(Http::Code::Ok, "Budget has been added to the account")
            .response(backendErrorResponse);

    }

    void retrieveAllAccounts(const Rest::Request&, Http::ResponseWriter response) {
        response.send(Http::Code::Ok, "No Account");
    }

    void retrieveAccount(const Rest::Request&, Http::ResponseWriter response) {
        response.send(Http::Code::Ok, "The bank is closed, come back later");
    }

    void createAccount(const Rest::Request& request, Http::ResponseWriter response) {
        cout << "inside Create account";
        rapidjson::Document d;
        const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
        
        
        // to get the yoke after /v1/accounts/
        string identity = request.body().substr(request.body().find("/") + 13); 
        
        // make into json object
        char * jsonBody = new char [request.body().length()+1];
        strcpy (jsonBody, request.body().c_str());
        d.Parse(jsonBody);
        cout << jsonBody;
        
        
        /*
        // modify
        rapidjson::Value& s = d["stars"];
        s.SetInt(s.GetInt() + 1);
        
        rapidjson::Value& p = d["project"];
        p.SetString("ooblock");
        */
        
        
        vector<SharedDataContainer> dataContainers = generateTestContainer();
        
        rapidjson::StringBuffer buffera = vectorToJson(dataContainers); 
        
        
           
        
    
        
        response.send(Http::Code::Ok, buffera.GetString());
        //***************************************************************************************************
        //END OF read large set and Send back

   
    }

    void creditAccount(const Rest::Request&, Http::ResponseWriter response) {
        response.send(Http::Code::Ok, "The bank is closed, come back later");
    }
    
    vector<SharedDataContainer> generateTestContainer(){
        //test with larger set
        std::vector<SharedDataContainer> dataContainers;
        
        dataContainers.push_back(SharedDataContainer(1));
        dataContainers.back().AddSharedData(SharedData(200, "count"));
        dataContainers.back().AddSharedData(SharedData(0, "initial"));
        dataContainers.back().AddSharedData(SharedData(9000, "max"));
        dataContainers.back().AddSharedData(SharedData(1, "iteration"));
        
        dataContainers.push_back(SharedDataContainer(0));
        dataContainers.back().AddSharedData(SharedData(10000, "count"));
        dataContainers.back().AddSharedData(SharedData(0, "initial"));
        dataContainers.back().AddSharedData(SharedData(10000, "max"));
        dataContainers.back().AddSharedData(SharedData(1, "iteration"));
        
        return dataContainers;
    
    }
    
    rapidjson::StringBuffer vectorToJson(vector<SharedDataContainer> dataContainers){
        rapidjson::StringBuffer buffera;
        rapidjson::Writer<rapidjson::StringBuffer> writera(buffera);
        
        writera.StartArray();
        for (std::vector<SharedDataContainer>::const_iterator sharedDataItr = dataContainers.begin(); sharedDataItr != dataContainers.end(); ++sharedDataItr)
        sharedDataItr->Serialize(writera);
        writera.EndArray();
        
        return buffera;
    
    }
    
    vector<SharedDataContainer> jsonToVector(rapidjson::Document d){
    //***************************************************************************************************
        //read large set and Send back
        rapidjson::Value& a = d;

	static const char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };

	vector<SharedDataContainer> dataContainers;

	for (rapidjson::Value::ConstValueIterator InitialArray = a.Begin(); InitialArray != a.End(); ++InitialArray){


		for (rapidjson::Value::ConstMemberIterator InitialObject = InitialArray->MemberBegin(); InitialObject != InitialArray->MemberEnd(); ++InitialObject) {
    			//printf("Type of member %s is %s  %d\n", InitialObject->name.GetString(), kTypeNames[InitialObject->value.GetType()], InitialObject->value.GetType());

    			//cout << itr->value.GetType();
    			string active = "active";
    			string sharedData = "sharedData";
    			string temp = InitialObject->name.GetString();
    			if (InitialObject->value.GetType() == 6 && !temp.compare(active)){
    				//cout << itr->value.GetInt() << endl;
    				dataContainers.push_back(SharedDataContainer(InitialObject->value.GetInt()));
    			}

    			if (InitialObject->value.GetType() == 4 && !temp.compare(sharedData)){
    				//cout << itr->value.GetInt() << endl;
                            //cout << "got array";
                            
                            for (rapidjson::Value::ConstValueIterator finalObject = InitialObject->value.Begin(); finalObject != InitialObject->value.End(); ++finalObject) {
                            //printf("Type of member %s is %s  %d\n", finalObject->, kTypeNames[finalObject->value.GetType()], finalObject->value.GetType());
                                
                                SharedData tempSharedData;
                                
                                string value = "value";
                                string num = "num";
                                for (rapidjson::Value::ConstMemberIterator FinalObjectMembers = finalObject->MemberBegin(); FinalObjectMembers != finalObject->MemberEnd(); ++FinalObjectMembers) {
                                    string temp = FinalObjectMembers->name.GetString();
                                    //printf("Type of member %s is %s  %d\n", FinalObjectMembers->name.GetString(), kTypeNames[FinalObjectMembers->value.GetType()], FinalObjectMembers->value.GetType());
                                    
                                    if (FinalObjectMembers->value.GetType() == 5 && !temp.compare(value)){
                                        tempSharedData.setValue(FinalObjectMembers->value.GetString());
                                        //cout << "got value" << endl;
                                    }  
                                    
                                    
                                    if (FinalObjectMembers->value.GetType() == 6 && !temp.compare(num)){
                                        tempSharedData.setNum(FinalObjectMembers->value.GetInt());
                                        //cout << "got num" << endl;
                                    }  
                                                                    
                                }
                                
                                dataContainers.back().AddSharedData(tempSharedData);
                            }
                        }                      	
    		}
	}
        return dataContainers; 
    }
    

    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Description desc;
    Rest::Router router;
};






Server2::Server2() {
    Port port(9080);

    int thr = 2;

    Address addr(Ipv4::any(), port);

    cout << "Cores = " << hardware_concurrency() << endl;
    cout << "Using " << thr << " threads" << endl;

    BankerService banker(addr);

    banker.init(thr);
    banker.start();

    banker.shutdown();
}

Server2::Server2(const Server2& orig) {
}

Server2::~Server2() {
}

