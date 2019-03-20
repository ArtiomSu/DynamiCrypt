/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "JSON_helper.hpp"
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include <rapidjson/document.h>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "SharedData.hpp"
#include "SharedDataContainer.hpp"

JSON_helper::JSON_helper() {

}

std::vector<SharedDataContainer> JSON_helper::generateTestContainer(){
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

rapidjson::StringBuffer JSON_helper::vectorToJson(std::vector<SharedDataContainer> dataContainers){
    rapidjson::StringBuffer buffera;
    rapidjson::Writer<rapidjson::StringBuffer> writera(buffera);

    writera.StartArray();
    for (std::vector<SharedDataContainer>::const_iterator sharedDataItr = dataContainers.begin(); sharedDataItr != dataContainers.end(); ++sharedDataItr)
    sharedDataItr->Serialize(writera);
    writera.EndArray();

    return buffera;
}

std::vector<SharedDataContainer> JSON_helper::jsonToVector(rapidjson::Document* d){
    //read large set and Send back
    rapidjson::Value& a = *d;

    static const char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };

    std::vector<SharedDataContainer> dataContainers;

    for (rapidjson::Value::ConstValueIterator InitialArray = a.Begin(); InitialArray != a.End(); ++InitialArray){


            for (rapidjson::Value::ConstMemberIterator InitialObject = InitialArray->MemberBegin(); InitialObject != InitialArray->MemberEnd(); ++InitialObject) {
                    //printf("Type of member %s is %s  %d\n", InitialObject->name.GetString(), kTypeNames[InitialObject->value.GetType()], InitialObject->value.GetType());

                    //cout << itr->value.GetType();
                    std::string active = "active";
                    std::string sharedData = "sharedData";
                    std::string temp = InitialObject->name.GetString();
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

                            std::string value = "value";
                            std::string num = "num";
                            for (rapidjson::Value::ConstMemberIterator FinalObjectMembers = finalObject->MemberBegin(); FinalObjectMembers != finalObject->MemberEnd(); ++FinalObjectMembers) {
                                std::string temp = FinalObjectMembers->name.GetString();
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
