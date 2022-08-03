#include <condition_variable>
#include <cstdlib>
#include <iostream>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <queue>
#include <vector>

using namespace std;

mutex m;
condition_variable cv;

class ThreadPool{
    public:
        ThreadPool(int nw){
            for(int i=0; i<nw; i++) tids.push_back(thread(lifecycle, i));
        }

        void assign_work(function<void(int)> w){
            works.push(w);
            cv.notify_one();
        }

        void quit(){
            cout << "quitting all" << endl;
            termination = true;
            cv.notify_all();

            for(thread& t: tids) t.join();
        }

    private:
        queue<function<void(int)>> works;
        vector<thread> tids;
        bool termination = false;

        function<void(int)> lifecycle = [this](int tid){
            int my_id = tid;

            while(true){
                unique_lock<mutex> lock(m);

                //wait until there is some work to do
                cv.wait(lock, [this]{
                    return !works.empty() || termination;
                });

                if(termination && works.empty()) break;

                //there is at least one work, so I pick it
                auto f = works.front();
                works.pop();

                lock.unlock();

                //do the computation
                f(tid);
            }
        };
};

void func(string message, int tid){
    string text = message + to_string(tid);

    if((float) rand()/RAND_MAX < 0.3){
        cout << text << endl;
    }
    else{
        text += "...";
        cout << text << endl;
        this_thread::sleep_for(3s);
        cout << "... done" << endl;
    }
}

int main(int argc, char* argv[]){
    if(argc != 3){
        cout << "Usage: nw seed" << endl;
        return 1;
    }

    int nw = atoi(argv[1]);
    int seed = atoi(argv[2]);
    ThreadPool tp(nw);
    srand(seed);
    int i;

    for(i=0; i<10; i+=5){
        tp.assign_work(bind(func, to_string(i+1) +  " work by thread ", placeholders::_1));
        tp.assign_work(bind(func, to_string(i+2) +  " work by thread ", placeholders::_1));        
        tp.assign_work(bind(func, to_string(i+3) +  " work by thread ", placeholders::_1));        
        tp.assign_work(bind(func, to_string(i+4) +  " work by thread ", placeholders::_1));        
        tp.assign_work(bind(func, to_string(i+5) +  " work by thread ", placeholders::_1));          
    }

    this_thread::sleep_for(3s);
    tp.quit();

    return 0;
}
