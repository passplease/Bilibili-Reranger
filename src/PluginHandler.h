#include <vector>
#include <filesystem>
#include "Util.h"
#include "pluginInterface.h"

using namespace std;
namespace fs = std::filesystem;

class PluginHandler {
private:
    string name;
    HINSTANCE__* dll;
    const static string Plugin_Dir;
    static const vector<string>* plugins;
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
        typedef int (__cdecl *LOAD)();
        auto createPlugin = (LOAD) GetProcAddress(dll,"load");
        if(createPlugin == nullptr){
            cout << "未找到插件方法！" << endl;
            return FAIL;
        }
        int value = createPlugin();
        if(value == FAIL){
            cout << "插件注册失败！" << endl;
            return FAIL;
        }
        cout << "插件加载成功" << endl;
        return SUCCESS;
    }

    Task* nextTask(){
        typedef Task* (__cdecl *NEXT_KEYWORD)();
        auto plugin = (NEXT_KEYWORD) GetProcAddress(dll, "nextTask");
        if(plugin == nullptr)
            return nullptr;
        return plugin();
    }

    PluginStatus roughJudge(){
        typedef int (__cdecl *ROUGH_JUDGE)();
        auto plugin = (ROUGH_JUDGE) GetProcAddress(dll, "roughJudge");
        if(plugin == nullptr)
            return PASS;
        return PluginStatus(plugin());
    }

    VideoStatus judge(){
        typedef int (__cdecl *JUDGE)();
        auto plugin = (JUDGE) GetProcAddress(dll,"judge");
        if(plugin == nullptr)
            return UNKNOWN;
        return VideoStatus(plugin());
    }

    const string* getName(){
        return &(this -> name);
    }

    static void forEachPlugin(PluginStatus function(PluginHandler)){
        if(!plugins -> empty()) {
            for(const auto & plugin : *plugins){
                // TODO 是不是得把Plugin存下来
                PluginHandler examplePlugin = PluginHandler(plugin);
                switch(function(examplePlugin)){
                    case FAIL : cout << plugin << "插件运行失败！请检查具体原因！" << endl; break;
                    case SUCCESS : return;
                    case PASS : continue;
                    default :
                        throwError("Invalid plugin return value !");
                }
            }
        }
    }

    static bool checkVideo(VideoStatus function(PluginHandler)){
        if(!plugins -> empty()) {
            for(const auto & plugin : *plugins){
                PluginHandler examplePlugin = PluginHandler(plugin);
                switch(function(examplePlugin)){
                    case KEEP : return true;
                    case THROW : return false;
                    case UNKNOWN : continue;
                    default :
                        throwError("Invalid video status return value !");
                }
            }
        }
        return true;
    }

    static const vector<string>* getPlugins(){
        return plugins;
    }

    static void loadPlugin(){
        cout << "插件加载完成，共发现" << plugins -> size() << "个插件" << endl;
    }

private:
    static vector<string>* searchPlugin(){
        string path = string();
        path.append(".\\").append(Plugin_Dir);
        auto Plugins = new vector<string>();
        if(fs::exists(fs::absolute(path))){
            string fileName;
            for(auto& fileInfo : filesystem::directory_iterator(path)){
                fileName = fileInfo.path().filename().string();
                if(endWith(fileName.c_str(),".dll") && !filesystem::is_directory(fileInfo.path())){
                    char* buffer = nullptr;
                    removeEnd(fileName.c_str(),".dll",&buffer);
                    Plugins -> emplace_back(buffer);
                    freeOutputChar(&buffer);
                    buffer = nullptr;
                    cout << "找到插件：" << fileName << endl;
                }
            }
        }else cout << "插件寻找结果：未找到插件" << endl;
        cout << "插件加载结束，即将退出插件加载进程" << endl;
        return Plugins;
    }
};

const string PluginHandler::Plugin_Dir = "plugins";

const vector<string>* PluginHandler::plugins = PluginHandler::searchPlugin();
