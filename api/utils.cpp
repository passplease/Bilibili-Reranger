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

const string ConfigPath = "config";
map<const char*,Json> storedJson = map<const char*,Json>();

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

void throwError(const char* error) noexcept(false){
    cout << RED << error << RESET << endl;
    throw runtime_error(error);
}

void say(const char* message,const char* color){
    if(color != nullptr)
        cout << string(color);
    cout << message << RESET << endl;
}

void warn(const char* warn){
    say(warn,YELLOW);
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
            char* back = nullptr;
            defaultOutputChar(&back);
            toConfigPath(back,filePath,fileType);
            path = string(back);
            freeOutputChar(&back);
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

void toConfigPath(char* back,const char* filePath,const char* fileType){
    string path{};
    path = path.append(ConfigPath).append("\\").append(filePath).append(fileType);
    strcpy_s(back, strlen(path.c_str()) + 1,path.c_str());
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

    Data::Data(bool valid) {
        this -> _valid = valid;
        saved = true;
    }

    Data::~Data(){
        if(needSave())
            writeToJson();
        clear();
        broken();
    }

    void Data::clear(){
        data.clear();
        dataArrays.clear();
        strings.clear();
        stringArrays.clear();
        ints.clear();
        intArrays.clear();
        floats.clear();
        floatArrays.clear();
        _valid = true;
        saved = false;
    }

    bool Data::valid() const {
        return _valid;
    }

    void Data::broken() {
        this -> _valid = false;
    }

    void Data::setName(const char *_name, bool force) {
        if(this -> name.empty() || force) {
            this->name = string(_name);
            saved = false;
        }
    }

    void Data::setPath(const char *_path, bool force) {
        if(this -> path.empty() || force) {
            char* o;
            defaultOutputChar(&o);
            getConfig(o, _path);
            this -> path = o;
            saved = false;
        }
    }

    Data &Data::operator=(const dataStore::Data *other) {
        if(valid() && other != nullptr && other -> valid()) {
            this->data = other->data;
            this->dataArrays = other->dataArrays;
            this->strings = other->strings;
            this->stringArrays = other->stringArrays;
            this->ints = other->ints;
            this->intArrays = other->intArrays;
            this->floats = other->floats;
            this->floatArrays = other->floatArrays;
            this->saved = false;
        }
        return *this;
    }

    Data &Data::operator=(const dataStore::Data &other) {
        if(other.valid() && &other != this) {
            this->data = other.data;
            this->dataArrays = other.dataArrays;
            this->strings = other.strings;
            this->stringArrays = other.stringArrays;
            this->ints = other.ints;
            this->intArrays = other.intArrays;
            this->floats = other.floats;
            this->floatArrays = other.floatArrays;
            this->saved = false;
        }
        return *this;
    }

    bool Data::operator==(const dataStore::Data& other) const {
        if(this == &other)
            return true;
        if(
                valid() && other.valid()
                && strings == other.strings
                && stringArrays == other.stringArrays
                && ints == other.ints
                && intArrays == other.intArrays
                && floats == other.floats
                && floatArrays == other.floatArrays
                ){
            for(const pair<const string, Data>& d : other.data){
                if(d.second == data.at(d.first))
                    continue;
                return false;
            }
            for(const pair<const string,vector<Data>>& _d : other.dataArrays){
                if(this -> dataArrays.contains(_d.first)){
                    const auto d = this -> dataArrays.at(_d.first);
                    if(d.size() == _d.second.size()){
                        for(int i = 0;i < d.size();i++){
                            if(d.at(i) == _d.second.at(i)){
                                continue;
                            }
                            return false;
                        }
                        continue;
                    }
                }
                return false;
            }
            return true;
        }else return false;
    }

    bool Data::operator!=(const dataStore::Data& other) const{
        return !(*this == other);
    }

    Data *Data::operator+=(const dataStore::Data *other) {
        if(valid() && other != nullptr && other -> valid()) {
            data.insert(other->data.begin(), other->data.end());
            dataArrays.insert(other->dataArrays.begin(), other->dataArrays.end());
            strings.insert(other->strings.begin(), other->strings.end());
            stringArrays.insert(other->stringArrays.begin(), other->stringArrays.end());
            ints.insert(other->ints.begin(), other->ints.end());
            intArrays.insert(other->intArrays.begin(), other->intArrays.end());
            floats.insert(other->floats.begin(), other->floats.end());
            floatArrays.insert(other->floatArrays.begin(), other->floatArrays.end());
            saved = false;
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

    void Data::copy(Data** a) const {
        *a = new Data{};
        **a = this;
    }

    bool Data::needSave() const{
        return !saved && valid();
    }

    void Data::put(const char *label, const dataStore::Data *content, bool vector, bool recover) {
        if(!valid())
            return;
        for(const auto& d : content -> data){
            if(&d.second == this){
                throwError("Any json file cannot contain itself !");
                return;
            }
        }
        for(const auto& dArray : content -> dataArrays){
            for(const auto& d : dArray.second){
                if(&d == this){
                    throwError("Any json file cannot contain itself !");
                    return;
                }
            }
        }
        string stringLabel(label);
        if(vector){
            if(dataArrays.contains(stringLabel)) {
                if (content != nullptr)
                    dataArrays[stringLabel].emplace_back(*content);
                else dataArrays.erase(stringLabel);
                saved = false;
            }else {
                if(content != nullptr) {
                    std::vector<Data> newVector{};
                    newVector.emplace_back(*content);
                    dataArrays[stringLabel] = newVector;
                    saved = false;
                }
            }
        }else {
            if (contains<string, Data>(stringLabel, data)) {
                if(content == nullptr){
                    data.erase(stringLabel);
                    saved = false;
                    return;
                }
                if (recover) {
                    data[stringLabel] = *content;
                    saved = false;
                }
            } else if(content != nullptr) {
                data[stringLabel] = *content;
                saved = false;
            }
        }
    }

    void Data::put(const char *label, const char *content, bool vector, bool recover) {
        if(!valid())
            return;
        string stringLabel(label);
        if(vector){
            if(stringArrays.contains(stringLabel)){
                if(content != nullptr)
                    stringArrays[stringLabel].emplace_back(content);
                else stringArrays.erase(stringLabel);
                saved = false;
            }else {
                if(content != nullptr) {
                    std::vector<string> newVector{};
                    newVector.emplace_back(content);
                    stringArrays[stringLabel] = newVector;
                    saved = false;
                }
            }
        }else {
            if(contains<string,string>(stringLabel,strings)){
                if(content == nullptr){
                    strings.erase(stringLabel);
                    saved = false;
                    return;
                }
                if(recover) {
                    strings[stringLabel] = string(content);
                    saved = false;
                }
            }else if(content != nullptr) {
                strings[stringLabel] = string(content);
                saved = false;
            }
        }
    }

    void Data::put(const char *label, const int *content, bool vector, bool recover) {
        if(!valid())
            return;
        string stringLabel(label);
        if(vector){
            if(intArrays.contains(stringLabel)){
                if(content != nullptr)
                    intArrays[stringLabel].emplace_back(*content);
                else intArrays.erase(stringLabel);
                saved = false;
            }else if(content != nullptr){
                std::vector<int> newVector{};
                newVector.emplace_back(*content);
                intArrays[stringLabel] = newVector;
                saved = false;
            }
        }else{
            if(ints.contains(stringLabel)){
                if(content == nullptr){
                    ints.erase(stringLabel);
                    saved = false;
                    return;
                }
                if(recover) {
                    ints[stringLabel] = *content;
                    saved = false;
                }
            }else if(content != nullptr) {
                ints[stringLabel] = *content;
                saved = false;
            }
        }
    }

    void Data::put(const char *label, const float *content, bool vector, bool recover){
        if(!valid())
            return;
        string stringLabel(label);
        if(vector){
            if(floatArrays.contains(stringLabel)){
                if(content != nullptr) {
                    floatArrays[stringLabel].emplace_back(*content);
                }else floatArrays.erase(stringLabel);
                saved = false;
            }else if(content != nullptr){
                std::vector<float> newVector{};
                newVector.emplace_back(*content);
                floatArrays[stringLabel] = newVector;
                saved = false;
            }
        }else {
            if(floats.contains(stringLabel)){
                if(content == nullptr){
                    floats.erase(stringLabel);
                    saved = false;
                    return;
                }
                if(recover) {
                    floats[stringLabel] = *content;
                    saved = false;
                }
            }else if(content != nullptr) {
                floats[stringLabel] = *content;
                saved = false;
            }
        }
    }

    void Data::get(const char *label, dataStore::Data *data,bool copy) {
        if(copy)
            *data = this -> data.at(label);
        else data = &(this -> data.at(label));
    }

    void Data::get(const char *label, vector<dataStore::Data> *data,bool copy) {
        if(copy)
            *data = this -> dataArrays.at(label);
        else data = &(this -> dataArrays.at(label));
    }

    void Data::get(const char *label,const char *string, bool copy) {
        auto s = this -> strings.at(label);
        if(copy) {
            s = std::string(s);
        }
        string = s.c_str();
    }

    void Data::get(const char *label, vector<const char *> *string, bool copy) {
        if(string == nullptr)
            return;
        auto s = this -> stringArrays.at(label);
        for(auto element : s){
            if(copy)
                element = std::string(element);
            string -> emplace_back(element.c_str());
        }
    }

    void Data::get(const char *label, int *ints, bool copy) {
        if(copy)
            *ints = this -> ints.at(label);
        else ints = &(this -> ints.at(label));
    }

    void Data::get(const char *label, vector<int> *ints, bool copy) {
        if(copy)
            *ints = this -> intArrays.at(label);
        else ints = &(this -> intArrays.at(label));
    }

    void Data::get(const char *label, float *floats, bool copy) {
        if(copy)
            *floats = this -> floats.at(label);
        else floats = &(this -> floats.at(label));
    }

    void Data::get(const char *label, vector<float> *floats, bool copy) {
        if(copy)
            *floats = this -> floatArrays.at(label);
        else floats = &(this -> floatArrays.at(label));
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
            bool back = !storage || saveToFile(target_name, target_path);
            saved = back;
            return back;
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

    void from_json(const Json& json,Data& data){
        try{
            data.clear();
            for(const auto& pair : json.items()){
                const Json& j = pair.value();
                const string& label = pair.key();
                if(j.is_array()){
                    switch (j[0].type()) {
                        case nlohmann::detail::value_t::object: {
                            for(int slot = 0;slot < j.size();slot++) {
                                const Data dataInSlot = j[slot].get<Data>();
                                data.put(label.c_str(),&dataInSlot,true);
                            }
                            break;
                        }
                        case nlohmann::detail::value_t::string: {
                            for(int slot = 0;slot < j.size();slot++) {
                                const string dataInSlot = j[slot].get<string>();
                                data.put(label.c_str(),dataInSlot.c_str(),true);
                            }
                            break;
                        }
                        case nlohmann::detail::value_t::number_integer: {
                            for(int slot = 0;slot < j.size();slot++) {
                                const int dataInSlot = j[slot].get<int>();
                                data.put(label.c_str(),&dataInSlot,true);
                            }
                            break;
                        }
                        case nlohmann::detail::value_t::number_float: {
                            for(int slot = 0;slot < j.size();slot++) {
                                const float dataInSlot = j[slot].get<float>();
                                data.put(label.c_str(),&dataInSlot,true);
                            }
                            break;
                        }
                        default: {
                            data.broken();
                            throwError("Unchecked Json file, not right format.");
                            return;
                        }
                    }
                }else {
                    switch (j.type()) {
                        case nlohmann::detail::value_t::object: {
                            const Data d = j.get<Data>();
                            data.put(label.c_str(),&d);
                            break;
                        }
                        case nlohmann::detail::value_t::string: {
                            const string d = j.get<string>();
                            data.put(label.c_str(),d.c_str());
                            break;
                        }
                        case nlohmann::detail::value_t::number_integer: {
                            const int d = j.get<int>();
                            data.put(label.c_str(),&d);
                            break;
                        }
                        case nlohmann::detail::value_t::number_float: {
                            const float d = j.get<float>();
                            data.put(label.c_str(),&d);
                            break;
                        }
                        default: {
                            data.broken();
                            throwError("Unchecked Json file, not right format.");
                            return;
                        }
                    }
                }
            }
        }catch(const std::exception& e) {
            data.broken();
            say("Invalid Json File !",RED);
            if(!data.path.empty())
                say(("File Path: " + data.path).c_str(), RED);
            say("Exception Info: ",BLUE);
            throwError(e.what());
        }
    }

    void to_json(Json& json,const Data& data){
        string label;
        for(const auto& i : data.data){
            label = json.contains(i.first) ? Data::DATA + i.first : i.first;
            json[label] = i.second;
        }
        for(const auto& i : data.dataArrays){
            label = json.contains(i.first) ? Data::DATA_ARRAY + i.first : i.first;
            for(int slot = 0;slot < i.second.size(); slot++)
                json[label][slot] = i.second.at(slot);
        }
        for(const auto& i : data.strings){
            label = json.contains(i.first) ? Data::STRING + i.first : i.first;
            json[label] = i.second;
        }
        for(const auto& i : data.stringArrays){
            label = json.contains(i.first) ? Data::STRING_ARRAY + i.first : i.first;
            for(int slot = 0;slot < i.second.size(); slot++)
                json[label][slot] = i.second.at(slot);
        }
        for(const auto& i : data.ints){
            label = json.contains(i.first) ? Data::INT + i.first : i.first;
            json[label] = i.second;
        }
        for(const auto& i : data.intArrays){
            label = json.contains(i.first) ? Data::INT_ARRAY + i.first : i.first;
            for(int slot = 0;slot < i.second.size(); slot++)
                json[label][slot] = i.second.at(slot);
        }
        for(const auto& i : data.floats){
            label = json.contains(i.first) ? Data::FLOAT + i.first : i.first;
            json[label] = i.second;
        }
        for(const auto& i : data.floatArrays){
            label = json.contains(i.first) ? Data::FLOAT_ARRAY + i.first : i.first;
            for(int slot = 0;slot < i.second.size(); slot++)
                json[label][slot] = i.second.at(slot);
        }
    }
}

const string dataStore::Data::DATA = "data_";
const string dataStore::Data::DATA_ARRAY = "data_array_";
const string dataStore::Data::STRING = "string_";
const string dataStore::Data::STRING_ARRAY = "string_array_";
const string dataStore::Data::INT = "int_";
const string dataStore::Data::INT_ARRAY = "int_array_";
const string dataStore::Data::FLOAT = "float_";
const string dataStore::Data::FLOAT_ARRAY = "float_array_";
