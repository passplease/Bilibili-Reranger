#include "pluginInterface.h"

#define EXPORT __declspec(dllexport)

extern "C" {
EXPORT PluginStatus load();

EXPORT void registerGroups();

EXPORT VideoStatus roughJudge();

EXPORT VideoStatus judge();
}
