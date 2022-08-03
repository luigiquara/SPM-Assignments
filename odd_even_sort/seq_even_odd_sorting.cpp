#include <iostream>
#include <vector>
#include <cassert>
#include <random>
#include <algorithm>
#include <chrono>
#include <thread>

using namespace std;

int main(int argc, char* argv[]){
    if(argc != 2){
        cout << "Usage: vector_size" << endl;
        return(1);
    }

    int v_size = stoi(argv[1]);
    vector<int> vec(v_size);

    random_device rnd_device;
    mt19937 mersenne_engine {rnd_device()};
    uniform_int_distribution<int> dist {1, 1000000};
    generate(vec.begin(), vec.end(), [&dist, &mersenne_engine]{
        return dist(mersenne_engine);
    });

    vector<int> v_sorted = vec;
    sort(v_sorted.begin(), v_sorted.end());

    int start_pos = 0;
    int pos = start_pos;
    int n_changes = 1;
    bool first = true;

    auto start = chrono::high_resolution_clock::now();
    while(n_changes != 0){
        n_changes = 0;

        while(pos+1 < v_size){
            if(vec[pos] > vec[pos+1]){ //do the swap
                int tmp = vec[pos];
                vec[pos] = vec[pos+1];
                vec[pos+1] = tmp;

                n_changes++;

                this_thread::sleep_for(10ms);
            }

            pos = pos + 2;
        }

        start_pos = (start_pos + 1) % 2;
        pos = start_pos;
        if(first) n_changes = 1;
        first = false;
    }
    auto elapsed = chrono::high_resolution_clock::now() - start;
    auto time = chrono::duration_cast<chrono::microseconds>(elapsed).count();

    assert(is_sorted(vec.begin(), vec.end()));
    assert(vec == v_sorted);


    cout << "Elapsed time: " << time << " usecs" << endl;

    return(0);
}