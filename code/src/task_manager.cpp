#include "task_manager.h"
#include <iostream>
using namespace std;
void TaskManager::run()
{
    while (true)
    {
        out_thread = thread(&TaskManager::out_check, this);
        while (running)
        {
            string cmd;
            cin >> cmd;
            if (cmd == "add")
            {
                lock_guard<mutex> lock(task_mutex);
                int key;
                string kind;
                cout << "key" << endl;
                cin >> key;
                for (const auto &task : task_list)
                {
                    if (task.key == key)
                    {
                        cout << "Key: " << task.key << "重复" << endl;
                    }
                    else
                    {
                        cout << "kind" << endl;
                        cin >> kind;
                        add_task(key, kind);
                        cout << "add finish" << endl;
                    }
                }
                task_thread = thread(&TaskManager::process, this);
            }
            else if (cmd == "callback")
            {
                lock_guard<mutex> lock(task_mutex);
                int key, msg;
                cin >> key;
                cin >> msg;
                if (task_map.count(key))
                {
                    task_map[key]->callback(msg);
                    cout << "callback" << endl;
                }
                else
                {
                    cerr << "Invalid key: " << key << endl;
                }
            }
            else if (cmd == "pop")
            {
                lock_guard<mutex> lock(task_mutex);
                if (!task_list.empty())
                {
                    pop_task();
                    printf("pop finish\n\n");
                }
                else
                {
                    printf("task list is empty!\n");
                }
            }
        }
    }
}

void TaskManager::add_task(key, kind)
{
    lock_guard<mutex> lock(task_mutex);
    auto p_out = link.empty() ? &out : link.back();
    link.push_back(new atomic<int>(0));
    auto p_in = link.back();
    unique_ptr<SensorTaskBase> task;

    if (kind == "filter")
    {
        task = new TaskFliter(key, p_in, p_out);
    }
    else if (kind == "gain")
    {
        task = new TaskGain(key, p_in, p_out);
    }
    else if (kind == "delay")
    {
        task = new TaskDelayBuffer(key, p_in, p_out);
    }
    task_list.push_back(move(task));
    task_map[key] = task_list.back().get();
    task_list.back()->start();
    cout << "add" << endl;
}

void TaskManager::out_check()
{
    while (true)
    {
        int val = out.exchange(0);
        if (val != 0)
        {
            printf("out: %d\n\n", val);
        }
        sleep_ms(1);
    }
}

void TaskManager::pop_task()
{
    lock_guard<mutex> lock(mtx);
    if (tasks.empty())
    {
        cout << "No task to pop" << endl;
        return;
    }
    auto task = task_list.back();
    task_list.pop_back();
    task_map.erase(task->key);
    delete task;
    delete link.back();
    link.pop_back();
    cout << "pop" << endl;
}

void start()
{
    running = true;
    monitor_thread = thread(&SensorPipeline::monitor, this);
    cout << "start" << endl;
}

void stop()
{
    running = false;
    lock_guard<mutex> lock(task_mutex);
    tasks.clear();
    if (monitor_thread.joinable())
        monitor_thread.join();
    cout << "stop" << endl;
}

void monitor()
{
    while (running)
    {
        lock_guard<mutex> lock(mtx);
        if (out != 0)
        {
            cout << "Output: " << out << endl;
            out = 0; // 读取后清零
        }
        sleep_ms(int val)(1);
    }
}