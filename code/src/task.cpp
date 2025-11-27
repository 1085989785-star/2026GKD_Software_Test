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
        int val = (*p_in) + 1;
        printf("write Filter-{%d}:{%d}\n", key, val);
        *p_in = 0;
        sleep_ms(1);
    }
}
void TaskGain::callback(int msg) {
    k = msg;
    *p_in = 1;
}

void TaskGain::run() {
    while (!finish) {
        int val = p_in->exchange(0);
        if(val != 0) {
            *p_out = val * k;
            printf("write 2-%d: %d\n\n", key, val * k);
        }
        sleep_ms(1);
    }
}

void Task3::callback(int msg) {
    *p_in = msg;
}

void Task3::run() {
    while (!finish) {
        int val = p_in->exchange(0);
        if(val != 0) {
            *p_out = val;
            printf("write 3-%d: %d\n\n", key, val);
            sleep_ms(1);
            *p_out = val + 1;
            printf("write 3-%d: %d\n\n", key, val + 1);
        }
        sleep_ms(1);
    }
}