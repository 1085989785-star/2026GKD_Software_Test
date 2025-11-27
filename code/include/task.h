#pragma once
#include <atomic>
#include <vector>
#include <thread>
using namespace std;

void sleep_ms(int val);

class SensorTaskBase
{
public:
    SensorTaskBase(int key, atomic<int> *p_in, atomic<int> *p_out) : key(key), p_in(p_in), p_out(p_out) {}
    virtual void callback(int msg) = 0;
    virtual void run() = 0; // 不同task的任务
    void stop() = 0;
    virtual ~SensorTaskBase() = default;
    int key;

protected:
    atomic<int> *p_in = nullptr, *p_out = nullptr;
    atomic<bool> finish{false};

private:
    thread handler;
};
calc class TaskFilter : public SensorTaskBase
{
public:
    TaskFliter(int key, atomic<int> *p_in, atomic<int> *p_out) : SensorTaskBase(key, p_in, p_out) {}
    void run() override;
    void callback(int msg) override;
};

class TaskGain : public SensorTaskBase
{
};

class TaskDelayBuffer : public SensorTaskBase
{
};