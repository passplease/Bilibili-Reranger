#include <curl/curl.h>
#include <sstream>
#include <regex>
#include "Util.h"
#include "pluginInterface.h"
#include "develop/flags.h"
#include "config.h"
#include "BilibiliInterface.h"

using namespace std;

extern bool roughCheckVideo();
extern bool finalCheckVideo();

string getURL(crawlTask::Task* task = crawlTask::nowTask());

class CurlHelper{
friend string getURL(crawlTask::Task* task);
public:
    CurlHelper(){
        curl = curl_easy_init();
    };
    ~CurlHelper(){
        if(curl != nullptr)
            curl_easy_cleanup(curl);
    }
    void clear(){
        json.clear();
        tempData.clear();
    }
    void curlSetup(const char *cookie){
        if(curl == nullptr){
            throwError("创建CURL失败");
            return;
        }
        curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,CurlHelper::saveData,this);
        curl_easy_setopt(curl,CURLOPT_WRITEDATA,this);
        curl_easy_setopt(curl,CURLOPT_COOKIE,cookie);
    }
    bool connect(bool deal = true){
        if(nextURL().empty())
            return false;

        #if MORE_DETAILS
        say("下一次的URL:", false);
        say(url.c_str(), true, GREEN);
        #endif

        curl_easy_setopt(curl,CURLOPT_URL,nextURL().c_str());
        clear();
        #if CONNECT_INTERNET
        if(crawlNext()){
            _crawlNext = false;
            CURLcode code = curl_easy_perform(curl);
            if (CURLE_OK != code) {
                clear();
                warn("连接链接失败，信息如下：", false);
                warn(curl_easy_strerror(code), false);
                warn("错误码：", false);
                warn(to_string(code).c_str());
                return false;
            }
        }
        #else
        if(fileExists(BILIBILI_DATA)) {
            ifstream data(BILIBILI_DATA);
            if(data.is_open()){
                stringstream stream;
                stream << data.rdbuf();
                tempData = stream.str();
            }
        }
        #endif
        if(deal && dealJson()) {
            clear();
            return true;
        }
        return !deal;
    }
    CURL* getCurl(){
        return curl;
    }
    bool dealJson(){// TODO 允许插件根据自己的需求单独处理爬取下来的数据

        #if MORE_DETAILS
        say("爬取中");
        #endif

        if(!tempData.empty()) {
            #ifdef DEVELOP
            try {
            #endif
                json = Json::parse(tempData);
            #ifdef DEVELOP
            }catch (exception e){
                warn("爬取数据格式错误！");
                warn("数据如下：");
                warn(tempData.c_str());
                throwError(e.what());
            }
            #endif
        }
        auto task = crawlTask::nowTask();
        switch (task -> mode) {
            case crawlTask::WorkingMode::SUBSCRIBE : {
                if(!startWith(url.c_str(),videoByUser)) {

                    #if MORE_DETAILS
                    say("当前搜索的关注用户名：", false);
                    say(task->keyword);
                    #endif

                    json = subscribers;
                    for (auto &up: getSubscribers(json, false).items()) {

                        #if MORE_DETAILS
                        say("当前比对up名：", false);
                        say(getSubscriberName(up).c_str(), true, YELLOW);
                        #endif

                        if (getSubscriberName(up) == task->keyword) {
                            cout << up << endl;
                            clearURL();
                            string url(videoByUser);
                            url += "?vmid=";
                            url += to_string(up.value().at(VMID).get<int>());
                            url += "&ps=";
                            url += std::to_string(config<int>(SUBSCRIBE_SEARCH_VIDEO_COUNT));
                            nextSearch(url);

                            #if MORE_DETAILS
                            say("关注用户爬取一次", true, BLUE);
                            #endif

                            nextMustCrawl();
                            return true;
                        }
                    }
                }else{
                    forEachVideoOfPerson(json){
                        const auto& video = bilibili::Video::fromJson(videoData);
                        bilibili::setVideo(&video);
                        if(roughCheckVideo() && finalCheckVideo())
                            bilibili::keepVideo(video);
                        bilibili::clearVideo();
                    }
                    clear();
                    clearURL();
                    return crawlTask::nextTask(true) != nullptr;
                }
                return false;
            }
            case crawlTask::WorkingMode::TAG : {
                dataStore::Data data = json.get<dataStore::Data>();
                data.setName(tempDataName);
                data.setPath(tempDataPath);
                data.writeToJson();
                return false;
            }
            default: return false;
        }
    }
    void dealJson(Json& _json){
        clear();
        this -> json = _json;
        dealJson();
        clear();
    }
private:
    static dataStore::Data subscribers;
    bool _crawlNext = true;
    CURL *curl;
    Json json = Json();
    string tempData;
    static size_t saveData(char *data, size_t size, size_t member, void *userdata){
        auto* helper = static_cast<CurlHelper*>(userdata);
        long sizes = size * member;
        helper -> tempData += string(data,sizes);
        return sizes;
    }
    string url;
    string& nextURL(){
        return url;
    }
    void clearURL(){
        url = "";
    }
    static bool nextPage(){
        auto task = crawlTask::nowTask();
        switch(task -> mode){
            default : return false;
        }
    }
    static unsigned int getPages(string& url){
        regex regular(".*?pn=([0-9]*).*?");
        smatch result;
        if(regex_match(url,result,regular)){
            unsigned int back;
            sscanf_s(result[1].str().c_str(),"%d",&back);
            return back;
        }else return (unsigned int)1;
    }
    void nextPage(unsigned int nowPage){
        string pn = "pn=";
        string now = pn + std::to_string(nowPage++);
        string next = pn + std::to_string(nowPage);
        size_t pos = url.find(now);
        if(pos != string::npos){
            url.replace(pos,now.length(),next);
        }else url = url.append("&").append(next);
    }
    void nextMustCrawl(){
        _crawlNext = true;
    }
public:
    bool finishCrawl(){
        return url.empty() && crawlTask::nowTask() == nullptr;
    }
    void nextSearch(string url){
        if(CurlHelper::url.empty())
            CurlHelper::url = url;
    }
    void refreshSubscribers(bool force = false){
        if(subscribers.empty() || force) {
            clearURL();
            crawlTask::Task t("", 0, crawlTask::WorkingMode::SUBSCRIBE);
            nextSearch(getURL(&t));
            do{
                nextMustCrawl();
                connect(false);
                json = Json::parse(tempData);

                #ifdef DEVELOP
                dataStore::Data data = json.get<dataStore::Data>();
                #endif

                subscribers += getDataFromJson(json).get<dataStore::Data>();
                int count = getSubscriberCount(json);
                int pages = count / 50 + 1;
                unsigned int nowPage = getPages(url);

                #if MORE_DETAILS
                say("关注博主刷新完成第",false);
                say(to_string(nowPage).c_str(),false);
                say("页");
                #endif

                if(nowPage >= pages)
                    break;
                nextPage(nowPage);
            }while(true);
            clear();
            clearURL();
        }
    }
    [[nodiscard]] bool crawlNext() const{
        return _crawlNext || (crawlTask::nowTask() -> mode != crawlTask::WorkingMode::SUBSCRIBE);
    }
};

const string search = "https://api.bilibili.com/x/web-interface/search/all/v2?nextTask=选择公理&search_type=video&page_size=10";
const string commit = "https://api.bilibili.com/x/v2/reply/main?oid=BV18a411W7ge&type=1";
const string subscribe = "https://api.bilibili.com/x/relation/followings?vmid=3493105986702255";
const string liked = "https://api.bilibili.com/x/space/like/video?vmid=3493105986702255";
dataStore::Data CurlHelper::subscribers = dataStore::Data{};

const char* cookie = getenv("COOKIE");

bool crawl(){
    CurlHelper helper = CurlHelper();
    helper.curlSetup(cookie);
    helper.refreshSubscribers();
    do{
        say("等待中...");

        #if SLEEP_CRAWL
        Sleep(config<int>(WAIT_TIME));
        #endif

        auto task = crawlTask::nowTask();
        if(task == nullptr)
            break;
        helper.nextSearch(getURL(task));
    }while(helper.connect());
    return helper.finishCrawl();
}

string getURL(crawlTask::Task* task){// TODO 允许插件根据自己的需求返回特定的URL
    switch (task -> mode) {
        case crawlTask::WorkingMode::SUBSCRIBE: {
            string back = mySubscribers;
            back += "?vmid=";
            back += config<string>(VMID);
            return back;
        }
        case crawlTask::WorkingMode::TAG :
        case crawlTask::WorkingMode::SEARCH : {// TODO WBI签名
            string back = searchVideos;
            back += "&page_size=";
            back += to_string(config<int>(SEARCH_PAGE_SIZE));
            back += "&keyword=";
            back += task -> keyword;
            return back;
        }
        default: return "";
    }
}