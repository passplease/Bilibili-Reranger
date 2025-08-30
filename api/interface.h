#include "pluginInterface.h"

#define REQUIRED
#define OPTIONAL

/**
 * Called at program starting
 * */
REQUIRED PluginStatus load();

/**
 * Register all video group and tasks you want to crawl for.
 * */
OPTIONAL void registerGroups();

/**
 * Rough judge whether need to crawl more data , keep it in website or skip this video
 * */
REQUIRED VideoStatus roughJudge();

/**
 * Judge keep or throw this video away
 * */
REQUIRED VideoStatus judge();