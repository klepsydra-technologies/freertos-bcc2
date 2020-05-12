#include <stdio.h>
#include <stdlib.h>

//#include <iostream>
//#include <sstream>
//#include <vector>
//#include <algorithm>

#include <thread>
#include <atomic>

#include "posix_demo.h"

using namespace std;

static const int num_threads = 10;
std::atomic<int> counter;

//This function will be called from a thread

void call_from_thread(int tid) {
    printf("Launched by thread %d\n", tid);
    counter++;
    printf("Counter: %d\n", counter.load());
    //cout << "Launched by thread " << tid << endl;
    //counter++;
    //cout << "Counter: " << counter.load() << endl;
}

void vStartPOSIXDemo(void *pvParameters) {
    std::thread t[num_threads];

    //Launch a group of threads
    for (int i = 0; i < num_threads; ++i) {
        t[i] = std::thread(call_from_thread, i);
    }

    printf("printf: Launched from the main\n");
    //cout << "cout: Launched from the main" << endl;

    //Join the threads with the main thread
    for (int i = 0; i < num_threads; ++i) {
        t[i].join();
    }

    exit( 0 );
}

