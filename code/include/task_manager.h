#pragma once
#include "task.h"
#include <string>
using namespace std;

class TaskManager
{
private:
    mutex task_mutex;
    vector<int> taskgroup;
    atomic<int> out;
    thread out_thread;
    vector<TaskBase> task_list;
    vector<atomic<int> * link>;
    map<int, TaskBase *> task_map;

public:
    void add_task(int key, string kind);
    void pop_task();
    void out_check();
    void run();
};