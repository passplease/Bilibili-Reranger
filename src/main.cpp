#include <iostream>
#include "Crawler.h"
#include "PluginHandler.h"
#include "pluginInterface.h"

using namespace std;

const string search = "https://api.bilibili.com/x/web-interface/search/all/v2?nextTask=选择公理&search_type=video&page_size=10";
const string commit = "https://api.bilibili.com/x/v2/reply/main?oid=BV18a411W7ge&type=1";
const string subscribe = "https://api.bilibili.com/x/relation/followings?vmid=3493105986702255";
const string liked = "https://api.bilibili.com/x/space/like/video?vmid=3493105986702255";
const string url = subscribe;
const bool testDll = true;

const char* cookie = getenv("COOKIE");

void clean(){
    curl_global_cleanup();
}

bool checkEnv(){
    bool error = true;
    if(cookie == nullptr){
        cout << "未找到环境变量: COOKIE" << endl;
        error = false;
    }
    return error;
}

int main(){
    PluginHandler::loadPlugin();
    PluginHandler::forEachPlugin([](PluginHandler plugin) -> PluginStatus {
        return plugin.load();
    });

    if(!checkEnv()) {
        return 1;
    }

    if(testDll){
        cout << "当前处于测试插件状态，主程序已退出" << endl;
        return 0;
    }

    ostringstream path_stream;
    path_stream << R"(./Testing/DataFromWebsite_)" << 1 << R"(.Json)";
    CurlHelper::setFilePath(&path_stream);

    CurlHelper helper = CurlHelper();
    helper.curlSetup(&url,cookie);
    if(helper.connect()) {
        cout << "运行成功，现在将退出程序！";
        clean();
        return 0;
    }else {
        cout << "运行失败，请检查具体原因！";
        clean();
        return 1;
    }
}