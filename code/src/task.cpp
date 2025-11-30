#include "task.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

static std::mutex io_mutex;

void sleep_ms(int val)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(val));
}

void TaskFilter::callback(int msg)
{
    *p_in = msg;
}

void TaskFilter::run()
{
    while (!finish)
    {
        if (*p_in != 0)
        {

            *p_out = (*p_in) + 1;
            {
                std::lock_guard<std::mutex> lock(io_mutex);
                std::cout << "write Filter-{" << key << "}: " << *p_out << std::endl;
            }
            *p_in = 0;
        }
        sleep_ms(1);
    }
}

void TaskGain::callback(int msg)
{
    k = msg;
    *p_in = 1;
}

void TaskGain::run()
{
    while (!finish)
    {
        if (*p_in != 0)
        {
            *p_out = *p_in * k;
            {
                std::lock_guard<std::mutex> lock(io_mutex);
                std::cout << "write Gain-{" << key << "}: " << *p_out << std::endl;
            }
            *p_in = 0;
        }
        sleep_ms(1);
    }
}

void TaskDelayBuffer::callback(int msg)
{
    *p_in = msg;
}

void TaskDelayBuffer::run()
{
    while (!finish)
    {
        int t = p_in->exchange(0);
        if (t != 0)
        {
            *p_out = t;
            {
                std::lock_guard<std::mutex> lock(io_mutex);
                std::cout << "write Delay-{" << key << "}: " << *p_out << std::endl;
            }
            sleep_ms(1);
            *p_out = t + 1;
            {
                std::lock_guard<std::mutex> lock(io_mutex);
                std::cout << "write Delay-{" << key << "}: " << *p_out << std::endl;
            }
            sleep_ms(1);
        }
    }
}

void SensorTaskBase::stop()
{
    finish = true;
    if (handler.joinable())
    {
        handler.join();
    }
}

void SensorTaskBase::start()
{
    finish = false;
    handler = std::thread(&SensorTaskBase::run, this);
}