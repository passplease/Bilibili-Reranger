#include "Util.h"
#include "pluginInterface.h"

crawlTask::Group political("political");
crawlTask::Group math("math");
crawlTask::Group physical("physical");
crawlTask::Group chemistry("chemistry");
crawlTask::Group biological("biological");

const string EXAMPLE_PATH = "ExampleConfig";
const string EXAMPLE_NAME = "example_for_example_plugin";
const string GROUPS_LABEL = "groups";

void initGroups(){
    crawlTask::Task* a = nullptr;
    a = new crawlTask::Task{"波士顿圆脸",crawlTask::WorkingMode::SUBSCRIBE};
    political.registerTask(a);
    a = new crawlTask::Task("军情D7处",crawlTask::WorkingMode::SUBSCRIBE);
    political.registerTask(a);
    a = new crawlTask::Task("巴以冲突");
    political.registerTask(a);

    a = new crawlTask::Task("选择公理");
    math.registerTask(a);
    a = new crawlTask::Task("数学",crawlTask::WorkingMode::TAG);
    math.registerTask(a);

    a = new crawlTask::Task("物理");
    physical.registerTask(a);

    a = new crawlTask::Task("有机化学");
    chemistry.registerTask(a);
    a = new crawlTask::Task("化学",crawlTask::WorkingMode::TAG);
    chemistry.registerTask(a);

    a = new crawlTask::Task("生物");
    biological.registerTask(a);
}

void exampleConfig(){
    char* path;
    defaultOutputChar(&path);
    toConfigPath(path,EXAMPLE_PATH.c_str());
    if(!fileExists(path) && createConfig(path,EXAMPLE_PATH.c_str())) {
        initGroups();
        auto example = dataStore::Data::readFromJson(EXAMPLE_PATH.c_str(),EXAMPLE_NAME.c_str());
        dataStore::Data a{};
        crawlTask::group_to_data(a,political);
        dataStore::Data* b = nullptr;
        a.copy(&b);
        example.put(GROUPS_LABEL.c_str(),b,true);
        a.clear();

        crawlTask::group_to_data(a,math);
        a.copy(&b);
        example.put(GROUPS_LABEL.c_str(),b, true);
        a.clear();

        crawlTask::group_to_data(a,physical);
        a.copy(&b);
        example.put(GROUPS_LABEL.c_str(),b, true);
        a.clear();

        crawlTask::group_to_data(a,chemistry);
        a.copy(&b);
        example.put(GROUPS_LABEL.c_str(),b, true);
        a.clear();

        crawlTask::group_to_data(a,biological);
        a.copy(&b);
        example.put(GROUPS_LABEL.c_str(),b, true);
        a.clear();

        example.writeToJson();
    }
    freeOutputChar(&path);
}
