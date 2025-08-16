#include <iostream>
#include <vector>
#include "PluginInterface.h"
#include "Util.h"

using namespace std;

vector<Task> keywords = vector<Task>();

string CONFIG_PATH = "Targets",_CONFIG_PATH;
const string NAME = "Example Plugin";

dataStore::Data* CONFIG = nullptr;

int load(){
    cout << NAME << "插件开始加载" << endl;
    keywords.emplace_back("数学");
    char* _path;
    defaultOutputChar(&_path);
    if(getConfig(_path,CONFIG_PATH.c_str())) {
        _CONFIG_PATH = string(_path);
        delete _path;
        cout << NAME << "配置文件创建成功，路径：" << _CONFIG_PATH << endl;
        CONFIG = new dataStore::Data(dataStore::Data::readFromJson(CONFIG_PATH.c_str(),NAME.c_str()));
        if(CONFIG -> valid()) {
            CONFIG -> put("tags","选择公理");
            CONFIG -> writeToJson();
            return SUCCESS;
        }else{
            cout << NAME << " Open config failed !" << endl;
        }
    }else{
        cout << NAME << " Config file create failed ! Now path: " << *_path << endl;
        delete _path;
    }
    return FAIL;
}

Task* nextTask(){
    return nullptr;
}

int roughJudge(){
    return VideoStatus::UNKNOWN;
}

int judge(){
    return VideoStatus::UNKNOWN;
}