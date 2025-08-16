#include <curl/curl.h>
#include <sstream>

#include "DataSaver.h"

using namespace std;

class CurlHelper{
public:
    static string filePath;
    CurlHelper(){
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
    };
    ~CurlHelper(){
        if(curl != nullptr)
            curl_easy_cleanup(curl);
    }
    void curlSetup(const string *url, const char *cookie){
        if(nullptr == curl){
            cout << "创建CURL失败" << endl;
            return;
        }
        curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,CurlHelper::saveData);
        curl_easy_setopt(curl,CURLOPT_URL,url->c_str());
        curl_easy_setopt(curl,CURLOPT_COOKIE,cookie);
        if(!filePath.empty())
            clean(filePath);
    }
    bool connect(){
        CURLcode code = curl_easy_perform(curl);
        if(CURLE_OK != code){
            cout << "连接链接失败，信息如下：" << curl_easy_strerror(code) << " 错误码：" << code << endl;
            return false;
        }
        return true;
    }
    bool connect(const string *url){
        curl_easy_setopt(curl,CURLOPT_URL,url->c_str());
        return connect();
    }
    CURL* getCurl(){
        return curl;
    }
    static void setFilePath(const ostringstream *path){
        filePath = path -> str();
    }
private:
    CURL *curl;
    static unsigned long calledTimes;
    static size_t saveData(char *data, size_t size, size_t member, void *userdata){
        calledTimes++;
        long sizes = size * member;
        string content(data,sizes);
        cout << "第" << calledTimes << "次得到数据，当前数据信息如下：" << endl;

        createAndSave(content,filePath);
        cout << "第" << calledTimes << "次回调数据保存函数saveData，当前数据：" << sizes << endl;
        return sizes;
    }
};
unsigned long CurlHelper::calledTimes = 0;
string CurlHelper::filePath;