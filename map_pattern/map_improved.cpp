/*
Improved map pattern implementation
Block scheduling is the same
Cyclic scheduling is improved:
    + it starts nw asyncs one after the other
    + gets notified when an async is terminated, and assign new work to it
*/

#include <mutex>
#include <vector>
#include <iostream>
#include <future>
#include <cassert>

using namespace std;

//struct used to represent chunks
typedef struct {
  int start;
  int end;
} RANGE;

condition_variable cv;
mutex m;
vector<int> waiting_threads;
vector<int> work_done; //indicate the number of work done per thread


//compute the map over the vector v.
//mode = 0 -> block scheduling
//mode = 1 -> cyclic scheduling 
vector<float> map(vector<float> v, int mode, function<float(float, int)> f, int nw){

    cout << "map improved" << endl;

    //block scheduling
    if (mode==0){
        cout << "Block scheduling" << endl;
        vector<future<void>> futures(nw);
        vector<RANGE> ranges(nw);
        int delta = v.size()/nw; //size of each chunk

        //compute the ranges and
        //start nw asyncs
        //each one computes the function f over its range
        for(int i=0; i<nw; i++){
            ranges[i].start = i*delta;
            ranges[i].end = i != (nw-1) ? (i+1)*delta : v.size();

            futures[i] = async(launch::async, 
                [&v, &f](RANGE range){
                    for(int j=range.start; j < range.end; j++){
                        v[j] = f(v[j], 1000);
                    }
                    return;
                },
                ranges[i]
            );
        }
        for(int i=0; i < nw; i++){
            futures[i].get();
        }
    }

    //cyclic scheduling
    else if (mode==1) {
        cout << "Cyclic scheduling" << endl;
        vector<future<void>> futures(nw);
        float prob = 0.3; //probability of delay

        int i;
        int pos;
        int served_thread;
        int done_count;

        //initialize nw async on the first nw positions of the vector
        for(i=0; i<nw; i++){
            float rng = (float) rand()/(float) RAND_MAX; //random number between 0 and 1
            int delay = (rng < prob) ? 5000:100;

            futures[i] = async(launch::async,
                [&f, &v, i](float num, int delay, int tid){
                    v[i] = f(num, delay);
                    {
                        unique_lock<mutex> lock(m);
                        waiting_threads.push_back(tid);
                    }
                    cv.notify_all();
                    return;
                },
                v[i], delay, i);
                work_done.push_back(1); //set the work done to 1 for each thread
        }

        done_count = nw; //here, we have done the first nw elements;

        while(done_count < v.size()){
            {
                unique_lock<mutex> lock(m);
                while(waiting_threads.empty()) cv.wait(lock); //wait until a new thread has finished
            }

            //assign new work to the longest waiting thread
            float rng = (float) rand()/(float) RAND_MAX; //random number between 0 and 1
            int delay = (rng < prob) ? 5000:100;

            served_thread = waiting_threads.front();
            {
                unique_lock<mutex> lock(m);
                pos = work_done[served_thread]*nw + served_thread; //get the next position to be processed by the selected thread
            }

            //if the position exceeds, it means that the served_thread has processed all of its positions
            //we remove it from the waiting threads, so it cannot be called again
            if (pos >= v.size()){
                cout << "Thread " << served_thread << " has finished its work" << endl;
                waiting_threads.erase(waiting_threads.begin());
                continue;
            }

            futures[served_thread] = async(launch::async,
                [&f, &v, pos](float num, int delay, int tid){
                    v[pos] = f(num, delay);
                    {
                        unique_lock<mutex> lock(m);
                        waiting_threads.push_back(tid);
                    }
                    cv.notify_all();
                    return;
                },
                v[pos], delay, served_thread);

            {
                unique_lock<mutex> lock(m);
                waiting_threads.erase(waiting_threads.begin());
            }

            work_done[served_thread]++;
            done_count++; //increase the number of done elements
        }

    }

    return v;
}