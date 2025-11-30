#include "task_manager.h"
#include <iostream>
#include <mutex>
#include <atomic>
#include <memory>
#include <utility>

void TaskManager::run()
{
    while (true)
    {
        std::string cmd;
        {
            std::lock_guard<std::mutex> lock(task_mutex);
            std::cout << "cmd (add/callback/pop): ";
        }
        std::cin >> cmd;
        if (cmd == "add")
        {
            std::lock_guard<std::mutex> lock(task_mutex);
            int key;
            std::string kind;
            std::cout << "key: ";
            std::cin >> key;
            std::cout << "kind (filter/gain/delay): ";
            std::cin >> kind;
            add_task(key, kind);
            {
                std::lock_guard<std::mutex> lock(task_mutex);
                std::cout << "add finish" << std::endl;
            }
        }
        else if (cmd == "callback")
        {
            std::lock_guard<std::mutex> lock(task_mutex);
            int key, msg;
            std::cout << "key:";
            std::cin >> key;
            std::cout << "msg:";
            std::cin >> msg;
            if (task_map.count(key))
            {
                task_map[key]->callback(msg);
                {
                    std::lock_guard<std::mutex> lock(task_mutex);
                    std::cout << "callback" << std::endl;
                }
            }
            else
            {
                std::cerr << "Invalid key: " << key << std::endl;
            }
            sleep_ms(1);
        }
        else if (cmd == "pop")
        {
            pop_task();
        }
        // start();
        sleep_ms(1);
    }
}

void TaskManager::add_task(int key, std::string kind)
{
    std::lock_guard<std::mutex> lock(task_mutex);
    if (task_map.find(key) != task_map.end())
    {
        std::cout << "Key: " << key << "重复" << std::endl;
        return;
    }
    auto p_out = link.empty() ? &out : link.back().get();
    link.push_back(std::make_unique<std::atomic<int>>(0));
    auto p_in = link.back().get();
    std::unique_ptr<SensorTaskBase> task;
    if (kind == "filter")
    {
        task = std::make_unique<TaskFilter>(key, p_in, p_out);
    }
    else if (kind == "gain")
    {
        task = std::make_unique<TaskGain>(key, p_in, p_out);
    }
    else if (kind == "delay")
    {
        task = std::make_unique<TaskDelayBuffer>(key, p_in, p_out);
    }
    task_list.push_back(std::move(task));
    task_map[key] = task_list.back().get();
    task_list.back()->start();
    sleep_ms(1);
}

void TaskManager::out_check()
{
    while (running)
    {
        int val = out.exchange(0);
        if (val != 0)
        {
            std::cout << "out: " << val << "\n\n"
                      << std::endl;
        }
        sleep_ms(1);
    }
}

void TaskManager::monitor()
{
    while (running)
    {
        std::lock_guard<std::mutex> lock(task_mutex);
        int val = out.exchange(0);
        if (val != 0)
        {
            std::cout << "Output: " << out << std::endl;
            out = 0;
        }
        sleep_ms(1);
    }
}

void TaskManager::pop_task()
{
    std::lock_guard<std::mutex> lock(task_mutex);
    if (task_list.empty())
    {
        std::cout << "No task to pop" << std::endl;
        return;
    }
    auto &task = task_list.back();
    int key_to_remove = task->key;
    task->stop();
    task_list.pop_back();
    link.pop_back();
    task_map.erase(key_to_remove); 
    std::cout << "pop" << std::endl;
}

void TaskManager::start()
{
    running = true;
    monitor_thread = std::thread(&TaskManager::monitor, this);
    std::cout << "start" << std::endl;
}

void TaskManager::stop()
{
    running = false;
    std::lock_guard<std::mutex> lock(task_mutex);
    for (auto &task : task_list)
    {
        task->stop();
    }
    if (monitor_thread.joinable())
        monitor_thread.join();
    std::cout << "stop" << std::endl;
}