#include <nlohmann/json.hpp>
#include <direct.h>
#include "Util.h"

#define MAX_BUFFER_SIZE 100

#define RESET "\033[0m" // 重置颜色
#define RED "\033[31m" // 红色
#define GREEN "\033[32m" // 绿色
#define YELLOW "\033[33m" // 黄色
#define BLUE "\033[34m" // 蓝色

using namespace std;
using Json = nlohmann::json;
namespace fs = std::filesystem;

bool endWith(const char* target,const char* substring){
    string t = string(target);
    string s = string(substring);
    if(t.size() < s.size())
        return false;
    return t.substr(t.size() - s.size()) == s;
}

bool startWith(const char* target,const char* substring){
    string t = string(target);
    string s = string(substring);
    if(t.size() < s.size())
        return false;
    return t.substr(0, s.size()) == s;
}

string removeEnd(const string& target,const string& substring){
    return endWith(target.c_str(),substring.c_str()) ? target.substr(0,target.size() - substring.size()) : target;
}

void removeEnd(const char* target,const char* substring,char** buffer){
    string t = string(target);
    string s = string(substring);
    string back = removeEnd(t,s);
    defaultOutputChar(buffer);
    strcpy_s(*buffer,back.size() + 1,back.c_str());
}

bool fileExists(const char* name, bool absolute){
    return name != nullptr && (absolute ? fs::exists(fs::absolute(name)) : fs::exists(name));
}

void throwError(const char* error){
    cout << RED << error << RESET << endl;
    throw runtime_error(error);
}

bool convertToInt(const char* str, int& num){
    return sscanf_s(str, "%d", &num) == 1;
}

Json getJson(const char* name, const char* path){
    if(fileExists(path)){
        Json json;
        ifstream file(path);
        file >> json;
        return json;
    }else if(name != nullptr){
        auto json = storedJson.find(name);
        if(json != storedJson.end())
            return json -> second;
    }
    return nullptr;
}

void defaultOutputChar(char** output){
    *output = new char[MAX_BUFFER_SIZE];
}

void freeOutputChar(char** output){
    delete *output;
}

bool createConfig(char* output,const char* filePath, const char* fileType){
    try{
        if(!fileExists(ConfigPath.c_str())){
            _mkdir(ConfigPath.c_str());
        }
        string path(".\\");
        path.append(ConfigPath).append("\\").append(filePath).append(fileType);
        ofstream file(path.c_str());
        if(file.is_open()){
            file << "{}";
            file.close();
            strcpy_s(output, strlen(path.c_str()) + 1,path.c_str());
            return true;
        }else {
            cout << "创建文件失败！请检查具体原因！" << endl;
            throwError("Failed at creating file !");
        }
        return false;
    }catch (...){
        return false;
    }
}

bool getConfig(char* output,const char* filePath, const char* fileType){
    try{
        string path;
        if(startWith(filePath,ConfigPath.c_str())){
            path = filePath;
        }else {
            path = string{};
            path.append(ConfigPath).append("\\").append(filePath).append(fileType);
        }
        if(fileExists(path.c_str())) {
            strcpy_s(output, strlen(path.c_str()) + 1,path.c_str());
            return true;
        }else {
            char out[MAX_BUFFER_SIZE];
            if(createConfig(out,filePath, fileType) && fileExists(path.c_str())) {
                strcpy_s(output, strlen(out) + 1,out);
                return true;
            }else {
                throwError("Failed at creating file !");
            }
        }
        return false;
    }catch (...){
        return false;
    }
}

bool saveToFile(const char* name,const char* path){
    auto json = getJson(name,nullptr);
    if(json == nullptr)
        json = getJson(name,path);
    if(json != nullptr){
        ofstream file(path);
        file << json;
        file.close();
        return true;
    }else return false;
}

bool storeJson(const char* name,const char* path,const Json& json,bool release){
    if(release){
        storedJson.erase(name);
        return !storedJson.contains(name);
    }else {
        storedJson[name] = json == nullptr ? getJson(name,path) : json;
        return storedJson.contains(name);
    }
}

namespace dataStore{
    void defaultOutputChar(char** output){
        *output = new char[MAX_BUFFER_SIZE];
    }

    bool createConfig(char* output,const char* filePath, const char* fileType = ".json"){
        try{
            if(!fileExists(ConfigPath.c_str())){
                _mkdir(ConfigPath.c_str());
            }
            string path(".\\");
            path.append(ConfigPath).append("\\").append(filePath).append(fileType);
            ofstream file(path.c_str());
            if(file.is_open()){
                file << "{}";
                file.close();
                strcpy_s(output, strlen(path.c_str()) + 1,path.c_str());
                return true;
            }else {
                cout << "创建文件失败！请检查具体原因！" << endl;
                throwError("Failed at creating file !");
            }
            return false;
        }catch (...){
            return false;
        }
    }

    bool getConfig(char* output,const char* filePath, const char* fileType = ".json"){
        try{
            string path;
            if(startWith(filePath,ConfigPath.c_str())){
                path = filePath;
            }else {
                path = string{};
                path.append(ConfigPath).append("\\").append(filePath).append(fileType);
            }
            if(fileExists(path.c_str())) {
                strcpy_s(output, strlen(path.c_str()) + 1,path.c_str());
                return true;
            }else {
                char out[MAX_BUFFER_SIZE];
                if(createConfig(out,filePath, fileType) && fileExists(path.c_str())) {
                    strcpy_s(output, strlen(out) + 1,out);
                    return true;
                }else {
                    throwError("Failed at creating file !");
                }
            }
            return false;
        }catch (...){
            return false;
        }
    }

    bool saveToFile(const char* name,const char* path){
        auto json = getJson(name,nullptr);
        if(json == nullptr)
            json = getJson(name,path);
        if(json != nullptr){
            ofstream file(path);
            file << json;
            file.close();
            return true;
        }else return false;
    }

    bool storeJson(const char* name,const char* path,const Json& json = nullptr,bool release = false){
        if(release){
            storedJson.erase(name);
            return !storedJson.contains(name);
        }else {
            storedJson[name] = json == nullptr ? getJson(name,path) : json;
            return storedJson.contains(name);
        }
    }

    Data::Data(bool valid) {
        this -> _valid = valid;
    }

    Data::~Data(){
        writeToJson();
        data.clear();
        strings.clear();
        ints.clear();
    }

    bool Data::valid() const {
        return _valid;
    }

    void Data::broken() {
        this -> _valid = false;
    }

    void Data::setName(const char *_name, bool force) {
        if(this -> name.empty() || force)
            this -> name = string(_name);
    }

    void Data::setPath(const char *_path, bool force) {
        if(this -> path.empty() || force) {
            char* o;
            defaultOutputChar(&o);
            getConfig(o, _path);
            this -> path = o;
        }
    }

    Data &Data::operator=(const dataStore::Data *other) {
        if(valid() && other -> valid()) {
            this->data = other->data;
            this->strings = other->strings;
            this->ints = other->ints;
        }
        return *this;
    }

    Data &Data::operator=(const dataStore::Data &other) {
        if(other.valid() && &other != this) {
            this -> data = other.data;
            this -> strings = other.strings;
            this -> ints = other.ints;
        }
        return *this;
    }

    bool Data::operator==(const dataStore::Data *other) const {
        if(valid() && other -> valid() && strings == other -> strings && ints == other -> ints){
            for(const pair<const string, Data>& d : other -> data){
                if(!(d.second == &data.at(d.first)))
                    return false;
            }
            return true;
        }else return false;
    }

    Data *Data::operator+=(const dataStore::Data *other) {
        if(valid() && other -> valid()) {
            data.insert(other->data.begin(), other->data.end());
            strings.insert(other->strings.begin(), other->strings.end());
            ints.insert(other->ints.begin(), other->ints.end());
        }
        return this;
    }

    Data Data::operator+(const dataStore::Data *other) const {
        Data value{};
        if(valid() && other -> valid()) {
            value += this;
            value += other;
        }
        return value;
    }

    void Data::put(const char *label, const dataStore::Data *content, bool recover) {
        if(!valid())
            return;
        string stringLabel(label);
        if(contains<string,Data>(stringLabel,data)){
            if(recover)
                data[stringLabel] = *content;
            else data[stringLabel] += content;
        }else data[stringLabel] = *content;
    }

    void Data::put(const char *label, const dataStore::Data &content, bool recover) {
        if(!valid())
            return;
        put(label,&content,recover);
    }

    void Data::put(const char *label, const char *content, bool recover) {
        if(!valid())
            return;
        string stringLabel(label);
        if(contains<string,string>(stringLabel,strings)){
            if(recover)
                strings[stringLabel] = content;
            else {
                string old(strings[stringLabel]);
                old.append(content);
                strings[stringLabel] = old;
            }
        }else strings[stringLabel] = content;
    }

    void Data::put(const char *label, const int &content) {
        ints[string(label)] = content;
    }

    bool Data::writeToJson(const char *target_name, const char *target_path, bool storage) {
        if(!valid())
            return false;
        setName(target_name);
        setPath(target_path);
        Json json = getJson(name.c_str(), path.c_str());
        if (json == nullptr) {
            string error = "File not exists ! Cannot write Json ! File path: ";
            error.append(path);
            throwError(error.c_str());
            return false;
        }else {
            dataStore::to_json(json,*this);
            storeJson(name.c_str(),path.c_str(),json);
            return !storage || saveToFile(target_name, target_path);
        }
    }

    bool Data::writeToJson() {
        return !this -> name.empty() && !this -> path.empty() && writeToJson(this -> name.c_str(),this -> path.c_str());
    }

    Data Data::readFromJson(const char *path, const char *name, const bool _throw) {
        Data d;
        d.setName(name);
        ifstream file;
        char* filePath;
        if(fileExists(path)){
            d.setPath(path);
            file = ifstream(path);
        }else {
            defaultOutputChar(&filePath);
            d = Data{};
            if(!getConfig(filePath,path)) {
                if(_throw) {
                    string error("Path not exists ! Current path: ");
                    error.append(path);
                    throwError(error.c_str());
                }
                d.broken();
                return d;
            }
            d.setPath(filePath);
            file = ifstream(filePath);
        }
        if(file.is_open()) {
            Json json;
            file >> json;
            file.close();
            d = json.get<dataStore::Data>();
        }else d.broken();
        return d;
    }

    // TODO 检查读取格式等，需逐步检查
    void from_json(const Json& json,Data& data){
        Json j,j_label;

        try{
            data = Data{};
            if (json.contains(Data::DATA)) {
                j = json[Data::DATA];
                j_label = json[Data::DATA_LABEL];
                for (size_t i = 0; i < j.size(); i++) {
                    data.put(j_label.at(i).dump().c_str(), j.at(i).get<dataStore::Data>());
                }
            }

            if (json.contains(Data::STRINGS)) {
                j = json[Data::STRINGS];
                j_label = json[Data::STRINGS_LABEL];
                for (size_t i = 0; i < j.size(); i++) {
                    data.put(j_label.at(i).dump().c_str(), j.at(i).get<string>().c_str());
                }
            }

            if (json.contains(Data::INTS)) {
                j = json[Data::INTS];
                j_label = json[Data::INTS_LABEL];
                for (size_t i = 0; i < j.size(); i++) {
                    data.put(j_label.at(i).dump().c_str(), j.at(i).get<int>());
                }
            }
        }catch(const std::exception& e) {
            cout << "Invalid Json File !" << endl;
            if(!data.path.empty())
                cout << "File Path: " << data.path << endl;
            cout << "Exception Info: " << e.what() << endl;
            data = Data();
        }
    }

    // TODO 检查写入格式等，需逐步检查
    void to_json(Json& json,const Data& data){
        if (!data.data.empty()) {
            for (auto &&d: data.data) {
                json[Data::DATA_LABEL] += string(d.first);
                json[Data::DATA] += d.second;
            }
        }

        if (!data.strings.empty()) {
            for (auto &&s: data.strings) {
                json[Data::STRINGS_LABEL] += string(s.first);
                json[Data::STRINGS] += string(s.second);
            }
        }

        if (!data.ints.empty()) {
            for (auto &&i: data.ints) {
                json[Data::INTS_LABEL] += string(i.first);
                json[Data::INTS] += i.second;
            }
        }
    }
}
