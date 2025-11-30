#pragma once
#include <atomic>
#include <vector>
#include <thread>
#include <iostream>
#include <mutex>

void sleep_ms(int val);

class SensorTaskBase
{
public:
    SensorTaskBase(int key, std::atomic<int> *p_in, std::atomic<int> *p_out) : key(key), p_in(p_in), p_out(p_out) {}
    virtual void callback(int msg) = 0;
    virtual void run() = 0; // 不同task的任务
    void stop();
    void start();
    virtual ~SensorTaskBase() = default;
    int key;

protected:
    std::atomic<int> *p_in = nullptr, *p_out = nullptr;
    std::atomic<bool> finish{false};
    std::mutex task_mutex;

private:
    std::thread handler;
};

class TaskFilter : public SensorTaskBase
{
public:
    TaskFilter(int key, std::atomic<int> *p_in, std::atomic<int> *p_out) : SensorTaskBase(key, p_in, p_out) {}
    void run() override;
    void callback(int msg) override;
};

class TaskGain : public SensorTaskBase
{
public:
    TaskGain(int key, std::atomic<int> *p_in, std::atomic<int> *p_out) : SensorTaskBase(key, p_in, p_out) {}
    void callback(int msg) override;
    void run() override;
    int k = 1;
};

class TaskDelayBuffer : public SensorTaskBase
{
public:
    TaskDelayBuffer(int key, std::atomic<int> *p_in, std::atomic<int> *p_out) : SensorTaskBase(key, p_in, p_out) {}
    void callback(int msg) override;
    void run() override;
};