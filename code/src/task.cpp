#include "task.h"
#include <iostream>
using namespace std;

void sleep_ms(int val)
{
    this_thread::sleep_for(chrono::milliseconds(val));
}

void TaskFilter::callback(int msg)
{
    *p_in = msg;
}

void TaskFilter::run()
{
    while (!finish)
    {
        *p_out = (*p_in) + 1;
        cout << "write Fliter-{" << key << "}: " << *p_out << endl;
        *p_in = 0;
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
            cout << "write Gain-{" << key << "}: " << *p_out << endl;
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
            cout << "write Delay-{" << key << "}: " << *p_out << endl;
            sleep_ms(1);
            *p_out = t + 1;
            cout << "write Delay-{" << key << "}: " << *p_out << endl;
        }
        sleep_ms(1);
    }
}

void TaskBase::start() {
    handler = thread(&TaskBase::run, this);
}

void TaskBase::stop() {
    finish = true;
    handler.join();
}