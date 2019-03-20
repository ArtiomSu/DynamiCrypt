/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "API_service.hpp"
#include "JSON_helper.hpp"
#include "definitions.hpp"

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

    auto accountsPath = versionPath.path("/accounts");

    accountsPath
        .route(desc.get("/all"))
        .bind(&API_service::retrieveAllAccounts, this)
        .produces(MIME(Application, Json), MIME(Application, Xml))
        .response(Pistache::Http::Code::Ok, "The list of all account");

    accountsPath
        .route(desc.get("/:name"), "Retrieve an account")
        .bind(&API_service::retrieveAccount, this)
        .produces(MIME(Application, Json))
        .parameter<Pistache::Rest::Type::String>("name", "The name of the account to retrieve")
        .response(Pistache::Http::Code::Ok, "The requested account")
        .response(backendErrorResponse);

    accountsPath
        .route(desc.post("/:name"), "Create an account")
        .bind(&API_service::createAccount, this)
        .produces(MIME(Application, Json))
        .consumes(MIME(Application, Json))
        .parameter<Pistache::Rest::Type::String>("name", "The name of the account to create")
        .response(Pistache::Http::Code::Ok, "The initial state of the account")
        .response(backendErrorResponse);

    auto accountPath = accountsPath.path("/:name");
    accountPath.parameter<Pistache::Rest::Type::String>("name", "The name of the account to operate on");

    accountPath
        .route(desc.post("/budget"), "Add budget to the account")
        .bind(&API_service::creditAccount, this)
        .produces(MIME(Application, Json))
        .response(Pistache::Http::Code::Ok, "Budget has been added to the account")
        .response(backendErrorResponse);

}

void API_service::retrieveAllAccounts(const Pistache::Rest::Request&, Pistache::Http::ResponseWriter response) {

    std::string test = test_api();

    response.send(Pistache::Http::Code::Ok, "No Account " + test);
}

void API_service::retrieveAccount(const Pistache::Rest::Request&, Pistache::Http::ResponseWriter response) {
    response.send(Pistache::Http::Code::Ok, "The bank is closed, come back later");
}

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
    */


    //vector<SharedDataContainer> dataContainers = generateTestContainer();

    std::vector<SharedDataContainer> dataContainers = JSON_helper::jsonToVector(&d);

    rapidjson::StringBuffer buffera = JSON_helper::vectorToJson(dataContainers); 

    response.send(Pistache::Http::Code::Ok, buffera.GetString());
    //***************************************************************************************************
    //END OF read large set and Send back


}

void API_service::creditAccount(const Pistache::Rest::Request&, Pistache::Http::ResponseWriter response) {
    response.send(Pistache::Http::Code::Ok, "The bank is closed, come back later");
}
    
    