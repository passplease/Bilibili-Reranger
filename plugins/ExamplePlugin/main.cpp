#include <iostream>
#include <vector>
#include "tasks.h"
#include "interface.h"

using namespace std;
using namespace crawlTask;

string CONFIG_PATH = "Targets";
const string NAME = "Example Plugin";
const string VIDEO_TAG = "视频标签";
const string VIDEO_TITLE = "视频标题";
const string SUBSCRIBER = "关注的人";

dataStore::Data* CONFIG = nullptr;

PluginStatus load(){
    cout << NAME << "插件开始加载" << endl;
    exampleConfig();
    char* _path;
    defaultOutputChar(&_path);
    if(getConfig(_path,CONFIG_PATH.c_str())) {
        cout << NAME << "配置文件创建成功，路径：" << _path << endl;
        freeOutputChar(&_path);
        CONFIG = new dataStore::Data(dataStore::Data::readFromJson(CONFIG_PATH.c_str(),NAME.c_str()));
        if(CONFIG -> valid()) {
            return PluginStatus::SUCCESS;
        }else{
            cout << NAME << " Open config failed !" << endl;
        }
    }else{
        cout << NAME << " Config file create failed ! Now path: " << *_path << endl;
        freeOutputChar(&_path);
    }
    return PluginStatus::FAIL;
}

void registerTask(){}

VideoStatus roughJudge(){
    return VideoStatus::UNKNOWN;
}

VideoStatus judge(){
    return VideoStatus::UNKNOWN;
}