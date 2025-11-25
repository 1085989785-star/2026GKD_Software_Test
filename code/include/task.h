#pragma once
#include <atomic>
#include <vector>
#include <thread>

class TaskBase {
public:
    TaskBase(int key, std::atomic<int> *p1, std::atomic<int> *p2) : key(key), p1(p1), p2(p2) {}
    virtual void callback(int msg) = 0;
    virtual void run() = 0;
    virtual ~TaskBase() = default;
    void start();
    void stop();
    int key;
};

class Task1 : public TaskBase {
};

class Task2 : public TaskBase {
};

class Task3 : public TaskBase {
};