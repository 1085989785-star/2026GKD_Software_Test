#include "task_manager.h"
#include <iostream>
using namespace std;
void TaskManager::run()
{
    out_thread = thread(&TaskManager::out_check, this);
    while (true)
    {
        string cmd;
        cin >> cmd;
        if (cmd == "add")
        {
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
        }
        else if (cmd == "callback")
        {
            int key, msg;
            cin >> key;
            cin >> msg;
            if (task_map.count(key))
            {
                if (task_map.count(key))
                {
                    task_map[key]->callback(msg);
                }
            }
            else
            {
                cerr << "Invalid key: " << key << endl;
            }
        }
        else if (cmd == "pop")
        {
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

void TaskManager::add_task(key, kind)
{
    TaskBase *task_pointer = nullptr;
    lock_guard<mutex> lock(task_mutex);
    auto p_out = link.empty() ? &out : link.back();
    link.push_back(new atomic<int>(0));
    auto p_in = link.back();
    if (kind == "filter")
    {
        task_pointer = new TaskFliter(key, p_in, p_out);
    }
    else if (kind == "gain")
    {
        task_pointer = new TaskGain(key, p_in, p_out);
    }
    else if (kind == "delay")
    {
        task_pointer = new TaskDelayBuffer(key, p_in, p_out);
    }
    else
    {
        cerr << "kind error " << endl;
        delete link.back(); 
        link.pop_back();
        return;
    }
    task_map[key] = task_pointer;
    task_list.push_back(task_pointer);
    task_pointer->start();
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
        this_thread::sleep_ms(1);
        ;
    }
}

void TaskManager::pop_task()
{
    auto task_pointer = task_list.back();
    task_list.pop_back();
    task_map.erase(task_pointer->key);
    task_pointer->stop();
    delete task_pointer;
    delete link.back();
    link.pop_back();
}