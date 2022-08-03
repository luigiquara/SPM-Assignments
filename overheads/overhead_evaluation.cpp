#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <future>

using namespace std;

//it does some dummy computation - used to create a thread
void dummy_function(vector<int>& vec, int pos){
    vec[pos]++;
    return;
}

// compute the average over a vector - needed to compute the average time
auto average(vector<int>& v, int n){
    auto sum = 0;
    for(int i=0; i<n; i++) sum += v[i];
    auto avg = sum/n;

    return avg;
}

int main(int argc, char * argv[]){

    if (argc==1){
        cout << "Parameter: n_events"<<endl;
        return(0);
    }

    int n_events = stoi(argv[1]);

    // Thread overhead evaluation
    vector<int> t_elaps(n_events);
    vector<int> dummy_vec(n_events, 0);
    int sum = 0;

    for(int i=0; i<n_events; i++){
        auto start = chrono::high_resolution_clock::now();
        auto t = new thread(dummy_function, ref(dummy_vec), i);
        t->join();
        auto elapsed = chrono::high_resolution_clock::now() - start;
        t_elaps[i] = chrono::duration_cast<chrono::microseconds>(elapsed).count();
    }

    //usage of the values computated by dummy_function
    for(int i=0; i<n_events; i++) sum += dummy_vec[i];

    auto avg = average(t_elaps, n_events);
    cout << "Average elapsed time using threads: " << avg << "usecs" << endl;



    // Async overhead evaluation
    vector<int> a_elaps(n_events);

    for(int i=0; i<n_events; i++){
        auto start = chrono::high_resolution_clock::now();
        auto f = async(launch::async, dummy_function, ref(dummy_vec), i);
        f.get();
        auto elapsed = chrono::high_resolution_clock::now() - start;
        a_elaps[i] = chrono::duration_cast<chrono::microseconds>(elapsed).count();
    }

    //usage of the values computated by dummy_function
    sum = 0;
    for(int i=0; i<n_events; i++) sum += dummy_vec[i];

    avg = average(a_elaps, n_events);
    cout << "Average elapsed time using asyncs: " << avg << "usecs" << endl;


    return(0);
}