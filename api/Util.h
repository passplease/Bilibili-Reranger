#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "APIStatus.h"

#pragma once

using namespace std;
using Json = nlohmann::json;

const string ConfigPath = "config";
map<const char*,Json> storedJson = map<const char*,Json>();

template <class Label,class T>
bool contains(const Label& label,map<const Label,T> map){
    return map.find(label) != map.end();
}

Json getJson(const char* name, const char* path);

extern "C"{

API bool endWith(const char* target,const char* substring);

API bool startWith(const char* target,const char* substring);

/**
 * @param buffer DLL initialize, need call function: <p style="color:red">freeOutputChar</p> to free memory
 * */
API void removeEnd(const char* target,const char* substring,char** buffer);

API bool fileExists(const char* name, bool absolute = false);

API void throwError(const char* error);

API bool convertToInt(const char* str, int& num);

API void defaultOutputChar(char** output);

API void freeOutputChar(char** output);

/**
 * Create a config file (default .Json)
 * */
API bool createConfig(char* output,const char* filePath, const char* fileType = ".json");

/**
 * Get config file path, other function's parameter: path will call automatically
 * */
API bool getConfig(char* output,const char* filePath, const char* fileType = ".json");

API bool saveToFile(const char* name,const char* path);

/**
 * @deprecated
 * @param name The name of your file mark
 * @param path The path to your file
 * @param release Defined add to map or delete from map, false means adding
 * @return Success(true) or Fail(false)
 * */
API bool storeJson(const char* name,const char* path,const Json& json = nullptr,bool release = false);// TODO 还有用吗

namespace dataStore{
    class Data;

    void from_json(const Json& json,Data& data);

    void to_json(Json& json,const Data& data);

    class Data {
    private:
        bool _valid;
    public:
        map<const string,Data> data = map<const string,Data>();
        static const string DATA;
        static const string DATA_LABEL;

        map<const string,string> strings = map<const string,string>();
        static const string STRINGS;
        static const string STRINGS_LABEL;

        map<const string,int> ints = map<const string,int>();
        static const string INTS;
        static const string INTS_LABEL;

        string name;

        string path;

        API explicit Data(bool valid = true);

        API ~Data();

        [[nodiscard]] API bool valid() const;

        API void broken();

        API void setName(const char* _name, bool force = false);

        /**
         * @param _path Return value of getConfig()
         * */
        API void setPath(const char* _path, bool force = false);

        API Data& operator=(const Data* other);

        API Data& operator=(const Data& other);

        API bool operator==(const Data* other) const;

        API Data* operator+=(const Data* other);

        API Data operator+(const Data* other) const;

        /**
         * @param recover false means merge new Data to old Data, true means just recover old value
         * */
        API void put(const char* label,const Data* content,bool recover = false);

        API void put(const char* label,const Data& content,bool recover = false);

        API void put(const char* label,const char* content,bool recover = true);

        /**
         * @param recover always true
         * */
        API void put(const char* label,const int& content);

        /**
         * Save to Json, also to file
         * @param path The path to your file (path in computer or key in map)
         * @param data Data you want to write
         * @param storage Store to map or release, true means store, false means delete
         * */
        API bool writeToJson(const char* target_name,const char* target_path,bool storage = true);

        API bool writeToJson();

        API static Data readFromJson(const char* path,const char* name = nullptr,const bool _throw = false);
    };
}

}

const string dataStore::Data::DATA = "data";
const string dataStore::Data::DATA_LABEL = "data_";
const string dataStore::Data::STRINGS = "strings";
const string dataStore::Data::STRINGS_LABEL = "strings_";
const string dataStore::Data::INTS = "ints";
const string dataStore::Data::INTS_LABEL = "ints_";
