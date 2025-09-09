#include "pluginInterface.h"

#pragma once

#define REQUIRED
#define OPTIONAL

/**
 * Called at program starting
 * */
REQUIRED PluginStatus load();
API REQUIRED typedef PluginStatus (__cdecl *LOAD)();

/**
 * Register all video group and tasks you want to crawl for.
 * */
OPTIONAL void registerGroups();
API OPTIONAL typedef void (__cdecl *REGISTER)();

/**
 * Rough judge whether need to crawl more data , keep it in website or skip this video
 * */
OPTIONAL VideoStatus roughJudge();
API typedef VideoStatus (__cdecl *ROUGH_JUDGE)();

/**
 * Judge keep or throw this video away
 * */
OPTIONAL VideoStatus judge();
API typedef VideoStatus (__cdecl *JUDGE)();