#include "pluginInterface.h"

using namespace crawlTask;

Task::Task(const char *keyword, WorkingMode mode) {
    this -> keyword = keyword;
    this -> mode = mode;
}

const char* crawlTask::getName(WorkingMode mode){
    switch (mode) {
        case WorkingMode::SEARCH: {
            return "搜索模式";
        }
        case WorkingMode::SUBSCRIBE: {
            return "关注列表匹配模式";
        }
        case WorkingMode::TAG: {
            return "视频标签匹配模式";
        }
        default: {
            throwError("Unknown WorkingMode Type !");
            return "Error";
        }
    }
}

WorkingMode crawlTask::byName(const char *name) {
    string mode(name);
    if(mode == "搜索模式")
        return WorkingMode::SEARCH;
    else if(mode == "关注列表匹配模式")
        return WorkingMode::SUBSCRIBE;
    else if(mode == "视频标签匹配模式")
        return WorkingMode::TAG;
    warn("未匹配的模式名称");
    return WorkingMode::SEARCH;
}

Nullable Group* crawlTask::getGroup(const char* groupName){
    if(groupName != nullptr) {
        string name(groupName);
        for (const auto group: groups) {
            if (group->isName(groupName))
                return group;
        }
        return nullptr;
    }else{
        if(validIndex())
            return groups[workingIndex];
        else {
            string error("Wrong working Index of ");
            error += std::to_string(workingIndex);
            error += ". But groups size is ";
            error += std::to_string(groups.size());
            throwError(error.c_str());
            return nullptr;
        }
    }
}

bool crawlTask::registerTask(const char* groupName,Task* task,bool create){
    auto group = getGroup(groupName);
    if(group == nullptr && create){
        Group temp = Group(groupName);
        group = &temp;
    }
    if(group == nullptr) {
        throwError("Register task failed due to get group failed");
        return false;
    }
    return group -> registerTask(task);
}

bool crawlTask::registerGroup(const char* groupName,Group* group){
    if(!group -> isName(groupName))
        group -> name = groupName;
    if(getGroup(groupName) != nullptr)
        return false;
    groups.emplace_back(group);
    return getGroup(groupName) != nullptr;
}

Task* Group::nextTask(bool move) {
    if(validIndex(workingIndex + 1)){
        workingIndex += move;
        return tasks[workingIndex];
    }else {
        workingIndex += move;
        return nullptr;
    }
}

Task *Group::nowTask() {
    if(validIndex())
        return tasks[workingIndex];
    else {
        string error("Wrong working Index of ");
        error += std::to_string(workingIndex);
        error += ". But tasks size is ";
        error += std::to_string(tasks.size());
        throwError(error.c_str());
        return nullptr;
    }
}

bool Group::validIndex() const {
    return validIndex(workingIndex);
}

bool Group::validIndex(unsigned int index) const {
    return index < tasks.size();
}

bool Group::registerTask(Task *task) {
    if(task == nullptr)
        return false;
    tasks.emplace_back(task);
    return true;
}

Group::Group(const char *name,bool regi) {
    this -> name = name;
    if(regi && !registerGroup(name,this)){
        string error = "Register group failed ! Group name: ";
        error += name;
        throwError(error.c_str());
    }
}

bool Group::isName(const char *compare) const {
    string a(name);
    string b(compare);
    return a == b;
}

Task *crawlTask::nextTask(bool move) {
    auto group = getGroup();
    if(group != nullptr){
        return group -> nextTask(move);
    }
    return nullptr;
}

Task *crawlTask::nowTask() {
    auto group = getGroup();
    if(group != nullptr){
        return group -> nowTask();
    }else {
        throwError("Wrong working status !");
        return nullptr;
    }
}

bool crawlTask::validIndex(unsigned int index){
    return  index < groups.size();
}

void crawlTask::task_from_data(dataStore::Data &data, crawlTask::Task &task) {
    data.get("keyword",task.keyword);
    const char* mode = nullptr;
    data.get("working_mode",mode);
    task.mode = byName(mode);
}

void crawlTask::task_to_data(dataStore::Data& data,const Task& task){
    data.put("keyword",task.keyword);
    data.put("working_mode", getName(task.mode));
}

void crawlTask::group_from_data(dataStore::Data& data, Group& group){
    data.get("name",group.name);
    vector<dataStore::Data> datas;
    data.get("tasks",&datas);
    for(auto task : datas){
        Task t("");
        task_from_data(task,t);
        if(!string(t.keyword).empty())
            group.tasks.emplace_back(&t);
        else throwError("Wrong data format for tasks");
    }
}

void crawlTask::group_to_data(dataStore::Data& data, const Group& group){
    data.put("name",group.name);
    for(auto task : group.tasks){
        dataStore::Data a{};
        task_to_data(a,*task);
        data.put("tasks",&a,true);
    }
}
