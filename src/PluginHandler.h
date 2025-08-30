#include <vector>
#include <filesystem>
#include <minwindef.h>
#include <windows.h>
#include "pluginInterface.h"
#include "Util.h"

using namespace std;
using namespace crawlTask;
namespace fs = std::filesystem;

class PluginHandler {
private:
    string name;
    HINSTANCE__* dll;
    const static string Plugin_Dir;
    static const vector<string>* pluginNames;
    static vector<PluginHandler*>* plugins;
public:
    explicit PluginHandler(const string& name){
        this -> name = std::move(name);
        SetDllDirectoryA(Plugin_Dir.c_str());
        dll = LoadLibrary(TEXT((this -> name + ".dll").c_str()));
        if(dll == NULL) {
            cout << "尝试寻找插件：" << this -> name << "失败" << endl;
            throwError("未找到插件库！");
        }
    }

    ~PluginHandler(){
        FreeLibrary(dll);
    }

    PluginStatus load(){
        cout << "正在加载插件：" << *getName() << endl;
        typedef PluginStatus (__cdecl *LOAD)();
        auto createPlugin = (LOAD) GetProcAddress(dll,"load");
        if(createPlugin == nullptr){
            cout << "未找到插件方法！" << endl;
            return PluginStatus::FAIL;
        }
        PluginStatus value = createPlugin();
        if(value == PluginStatus::FAIL){
            cout << "插件注册失败！" << endl;
            return PluginStatus::FAIL;
        }
        cout << "插件加载成功" << endl;
        return value;
    }

    PluginStatus registerGroups(){
        typedef void (__cdecl *REGISTER)();
        auto plugin = (REGISTER) GetProcAddress(dll,"registerGroups");
        if(plugin == nullptr)
            return PluginStatus::PASS;
        plugin();
        return PluginStatus::SUCCESS;
    }

    VideoStatus roughJudge(){
        typedef VideoStatus (__cdecl *ROUGH_JUDGE)();
        auto plugin = (ROUGH_JUDGE) GetProcAddress(dll, "roughJudge");
        if(plugin == nullptr)
            return VideoStatus::UNKNOWN;
        return plugin();
    }

    VideoStatus judge(){
        typedef VideoStatus (__cdecl *JUDGE)();
        auto plugin = (JUDGE) GetProcAddress(dll,"judge");
        if(plugin == nullptr)
            return VideoStatus::UNKNOWN;
        return plugin();
    }

    const string* getName(){
        return &(this -> name);
    }

    static void forEachPlugin(PluginStatus function(PluginHandler)){
        if(!plugins -> empty()) {
            for(const auto & plugin : *plugins){
                switch(function(*plugin)){
                    case PluginStatus::FAIL : cout << plugin << "插件运行失败！请检查具体原因！" << endl; break;
                    case PluginStatus::SUCCESS :
                    case PluginStatus::PASS : continue;
                    default :
                        throwError("Invalid plugin return value !");
                }
            }
        }
    }

    static bool checkVideo(VideoStatus function(PluginHandler)){
        if(!plugins -> empty()) {
            for(const auto & plugin : *plugins){
                switch(function(*plugin)){
                    case VideoStatus::KEEP : return true;
                    case VideoStatus::THROW : return false;
                    case VideoStatus::UNKNOWN : continue;
                    default :
                        throwError("Invalid video status return value !");
                }
            }
        }
        return true;
    }

    static void loadAll(){
        cout << "插件加载完成，共发现" << pluginNames -> size() << "个插件" << endl;
        if(!pluginNames -> empty()) {
            for(const auto & plugin : *pluginNames){
                auto examplePlugin = new PluginHandler(plugin);
                switch(examplePlugin -> load()){
                    case PluginStatus::FAIL :
                        cout << plugin << "插件运行失败！请检查具体原因！" << endl;
                        break;
                    case PluginStatus::SUCCESS :
                        plugins -> emplace_back(examplePlugin);
                        continue;
                    case PluginStatus::PASS :
                        break;
                    default :
                        throwError("Invalid plugin return value !");
                }
                delete examplePlugin;
            }
        }
    }

private:
    static vector<string>* searchPlugin(NotNull vector<string>* back){
        string path = string();
        path.append(".\\").append(Plugin_Dir);
        if(fs::exists(fs::absolute(path))){
            string fileName;
            for(auto& fileInfo : filesystem::directory_iterator(path)){
                fileName = fileInfo.path().filename().string();
                if(endWith(fileName.c_str(),".dll") && !filesystem::is_directory(fileInfo.path())){
                    char* buffer = nullptr;
                    removeEnd(fileName.c_str(),".dll",&buffer);
                    back -> emplace_back(buffer);
                    freeOutputChar(&buffer);
                    buffer = nullptr;
                    cout << "找到插件：" << fileName << endl;
                }
            }
        }else cout << "插件寻找结果：未找到插件" << endl;
        cout << "插件加载结束，即将退出插件加载进程" << endl;
        return back;
    }
};

const string PluginHandler::Plugin_Dir = "plugins";

const vector<string>* PluginHandler::pluginNames = PluginHandler::searchPlugin(new vector<string>());

vector<PluginHandler*>* PluginHandler::plugins = new vector<PluginHandler*>();
