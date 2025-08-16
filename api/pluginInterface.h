#include "APIStatus.h"

#pragma once

extern "C" {

enum WorkingMode {
    SEARCH,
    SUBSCRIBE,
    TAG
};

struct API Task{
    const char* keyword;
    WorkingMode mode;
    explicit Task(const char* keyword,WorkingMode mode = SEARCH);
};

/**
 * Called at program starting
 * @Return_Class PluginStatus
 * */
API int load();

/**
 * Next keyword program need to search for, null means Pass
 * */
API Task* nextTask();

/**
 * Rough judge whether need to crawl more data , keep it in website or skip this video
 * @Return_Class VideoStatus
 * */
API int roughJudge();

/**
 * Judge keep or throw this video away
 * @Return_Class VideoStatus
 * */
API int judge();

enum PluginStatus {
    FAIL,
    SUCCESS,
    PASS
};

enum VideoStatus {
    KEEP,
    THROW,
    UNKNOWN
};

}