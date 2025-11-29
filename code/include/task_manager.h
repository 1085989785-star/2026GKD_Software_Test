#pragma once
#include "task.h"
#include <string>
#include <memory>
using namespace std;

class TaskManager
{
private:
    mutex task_mutex;

    atomic<int> out;
    atomic<bool> running;
    thread monitor_thread;
    vector<unique_ptr<SensorTaskBase>> tasks_list;
    vector<atomic<int> link>;
    map<int, TaskBase *> task_map;

public:
    SensorTaskBase(atomic<bool> running, atomic<int> out) : running(false), out(0) {}
    void add_task(int key, string kind);
    void pop_task();
    void out_check();
    void run();
    void monitor()
    ~TaskManager()
    {
        stop();
    }
};