#include "bilibiliAPIs.h"
#include "Util.h"
#include "pluginInterface.h"

#pragma once

#define getPublishTime(json) json["ctime"].get<int>()
#define getTitle(json) json["title"].get<std::string>()

extern "C" {

namespace bilibili {
    class Video {
    private:
        int _publishTime;
        Json json;
        string _title;

        Video(const dataStore::Data &data);

        Video(const Json& json);
    public:
        API static Video fromData(const dataStore::Data &data);

        API static Video fromJson(const Json& json);

        API static dataStore::Data toData(const Video &video);

        API int publishTime();

        API dataStore::Data getData();

        API const char* title();
    };

    API void setVideo(Nullable const Video* video);

    API const Video* nowVideo() noexcept(false);

    API void clearVideo();

    API void keepVideo(Video video,const char* label = crawlTask::getGroup() -> name);
}

}

namespace bilibili{
    API map<string,vector<Video>> getVideos();
}