#include "PluginInterface.h"

extern "C" {
    API PluginStatus load();

    API void registerTask();

    API VideoStatus roughJudge();

    API VideoStatus judge();
}
