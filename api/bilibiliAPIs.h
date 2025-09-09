
#define videoByUser "https://app.biliapi.com/x/v2/space/archive/cursor"
#define mySubscribers "https://api.bilibili.com/x/relation/followings"
#define searchVideos "https://api.bilibili.com/x/web-interface/wbi/search/type?search_type=video"

#define getDataFromJson(json) json["data"]
#define getSubscribers(json,all) (all ? getDataFromJson(json)["list"] : json["list"])
#define getSubscriberCount(json) getDataFromJson(json)["total"].get<int>()
#define getSubscriberName(json) up.value().at("uname").get<string>()
#define forEachVideoOfPerson(json) for(const auto& videoData : getDataFromJson(json)["item"])
