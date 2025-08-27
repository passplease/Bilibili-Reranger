#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "APIStatus.h"

#pragma once

using namespace std;
using Json = nlohmann::json;

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

API void throwError(const char* error) noexcept(false);

API void say(const char* message,const char* color = nullptr);

API void warn(const char* warning);

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

API void toConfigPath(NotNull char* back,const char* filePath,const char* fileType = ".json");

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

        bool saved = true;
    public:
        map<const string,Data> data = map<const string,Data>();
        const static string DATA;
        map<const string,vector<Data>> dataArrays = map<const string,vector<Data>>();
        const static string DATA_ARRAY;

        map<const string,string> strings = map<const string,string>();
        const static string STRING;
        map<const string,vector<string>> stringArrays = map<const string,vector<string>>();
        const static string STRING_ARRAY;

        map<const string,int> ints = map<const string,int>();
        const static string INT;
        map<const string,vector<int>> intArrays = map<const string,vector<int>>();
        const static string INT_ARRAY;

        map<const string,float> floats = map<const string,float>();
        const static string FLOAT;
        map<const string,vector<float>> floatArrays = map<const string,vector<float>>();
        const static string FLOAT_ARRAY;

        string name;

        string path;

        API explicit Data(bool valid = true);

        API ~Data();

        API void clear();

        [[nodiscard]] API bool valid() const;

        API void broken();

        API void setName(const char* _name, bool force = false);

        /**
         * @param _path Return value of getConfig()
         * */
        API void setPath(const char* _path, bool force = false);

        API Data& operator=(const Data* other);

        API Data& operator=(const Data& other);

        API bool operator==(const Data& other) const;

        API bool operator!=(const Data& other) const;

        API Data* operator+=(const Data* other);

        API Data operator+(const Data* other) const;

        API void copy(Data** a) const;

        API bool needSave() const;

        /**
         * @param recover false means merge new Data to old Data, true means just recover old value
         * @param vector put the value to vector or not, if true, the recover is false all the time
         * @param content could be nullptr, if that, means remove the value
         *
         * Parameters are the same for followings
         * */
        API void put(const char *label, const dataStore::Data *content, bool vector = false, bool recover = true);

        API void put(const char *label, const char *content, bool vector = false, bool recover = true);

        API void put(const char *label, const int *content, bool vector = false, bool recover = true);

        API void put(const char *label, const float *content, bool vector = false, bool recover = true);

        /**
         * All get function won't recurse to find label, just find in this object.
         * */
        API Nullable void get(const char* label,dataStore::Data* data,bool copy = false);
        API Nullable void get(const char* label,vector<dataStore::Data>* data,bool copy = false);

        API Nullable void get(const char* label,const char* string,bool copy = false);
        API Nullable void get(const char* label,NotNull vector<const char*>* string,bool copy = false);

        API Nullable void get(const char* label,int* ints,bool copy = false);
        API Nullable void get(const char* label,vector<int>* ints,bool copy = false);

        API Nullable void get(const char* label,float* floats,bool copy = false);
        API Nullable void get(const char* label,vector<float>* floats,bool copy = false);

        /**
         * Save to Json, also to file
         * @param path The path to your file (path in computer or key in map)
         * @param data Data you want to write
         * @param storage Store to map or release, true means store, false means delete
         * */
        API bool writeToJson(const char* target_name,const char* target_path,bool storage = true);

        API bool writeToJson();

        API static Data readFromJson(const char* path,const char* name = nullptr,bool _throw = false);
    };
}

}
