#pragma once
#include "task.h"
#include <string>
#include <memory>
#include <map>
#include <mutex>
#include <atomic>
#include <vector>
#include <thread>

class TaskManager
{
private:
    std::mutex task_mutex;
    std::atomic<int> out;
    std::atomic<bool> running{false};
    std::thread monitor_thread;
    std::vector<std::unique_ptr<SensorTaskBase>> task_list;
    std::vector<std::unique_ptr<std::atomic<int>>> link;
    std::map<int, SensorTaskBase *> task_map;

public:
    TaskManager() : out(0), running(false) {} //
    ~TaskManager()
    {
        stop();
    }
    void add_task(int key, std::string kind);
    void pop_task();
    void out_check();
    void run();
    void monitor();
    void start();
    void stop();
};