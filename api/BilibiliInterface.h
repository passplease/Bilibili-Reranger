#include "bilibiliAPIs.h"
#include "Util.h"
#include "pluginInterface.h"
#include "bilibili_wbi/wbi.h"

#pragma once

#define getPublishTime(json) (json.contains("ctime") ? json["ctime"].get<int>() : json["pubdate"].get<int>())
#define getTitle(json) json["title"].get<std::string>()
#define getAuthor(json) json["author"].get<std::string>()
#define getDescription(json) (json.contains("description") ? json["description"].get<std::string>() : "")
#define WRONG_MID (-1)
#define getMid(json) (json.contains("mid") ? json["mid"].get<int>() : WRONG_MID)
#define getVideoURL(json) Video::getURLFromJson(json)

extern "C" {

#define OUTPUT_PATH "output\\crawl_output.json"
#define OUTPUT_NAME "crawl_output"

namespace bilibili {
    class Video {
    private:
        int _publishTime{};
        Json json;
        string _title;
        string _author;
        string _description;
        int _mid{};
        string _url;

        explicit Video(const dataStore::Data &data);

        explicit Video(const Json& json);
    public:
        API static Video fromData(const dataStore::Data &data);

        API static Video fromJson(const Json& json);

        API static dataStore::Data toData(const Video &video);

        API static string getURLFromJson(const Json& json);

        [[nodiscard]] API int const& publishTime() const;

        [[nodiscard]] API dataStore::Data getData() const;

        [[nodiscard]] API Json const& getJson() const;

        [[nodiscard]] API const char* title() const;

        [[nodiscard]] API const char* author() const;

        [[nodiscard]] API const char* description() const;

        [[nodiscard]] API int const& mid() const;

        [[nodiscard]] API const char* url() const;

        API void write_necessary(Json& json) const;

        API void write_all(Json& json) const;

        API void reset();
    };

    API void setVideo(Nullable const Video* video);

    API const Video* nowVideo() noexcept(false);

    API void clearVideo();

    API void keepVideo(const Video& video,const char* label = crawlTask::getGroup() -> name);

    API bool enoughVideo(const char* label = crawlTask::getGroup() -> name);
}

}

namespace bilibili{
    API map<string,vector<Video>> getVideos();

    API void saveVideos();
}