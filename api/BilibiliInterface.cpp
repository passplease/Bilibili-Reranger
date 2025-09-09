#include "BilibiliInterface.h"
#include "pluginInterface.h"

namespace bilibili{
    const Video* _nowVideo;

    Video::Video(const dataStore::Data &data) {
        Json j;
        j = data;
        Video{j};
    }

    Video::Video(const Json &json) {
        this -> json = json;
        _publishTime = getPublishTime(json);
        _title = getTitle(json);
    }

    Video Video::fromData(const dataStore::Data &data) {
        return Video{data};
    }

    Video Video::fromJson(const Json &json) {
        return Video{json};
    }

    dataStore::Data Video::toData(const bilibili::Video &video) {
        return video.json.get<dataStore::Data>();
    }

    int Video::publishTime(){
        int back = _publishTime;
        return back;
    }

    dataStore::Data Video::getData() {
        return toData(*this);
    }

    const char* Video::title(){
        return _title.c_str();
    }

    void setVideo(Nullable const Video* video){
        _nowVideo = video;
    }

    const Video* nowVideo(){
        return _nowVideo;
    }

    void clearVideo(){
        setVideo(nullptr);
    }

    map<string,vector<Video>> videos = map<string,vector<Video>>();

    void keepVideo(Video video,const char* label){
        string name(label);
        videos[name].emplace_back(video);
    }

    map<string,vector<Video>> getVideos(){
        return videos;
    }
}
