#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <future>
#include <mutex>
#include <condition_variable>
#include <cassert>
#include <random>
#include <algorithm>
#include <chrono>
#include <barrier>

using namespace std;

mutex m;

int main(int argc, char* argv[]){
    if(argc != 4){
        cout << "Usage: vector_size nw verbose" << endl;
        return 1;
    }

    int v_size = stoi(argv[1]);
    int nw = stoi(argv[2]);
    int verbose = stoi(argv[3]);
    vector<vector<int>> v(2, vector<int>(v_size));
    vector<future<void>> workers(nw);
    barrier sync_point(nw+1, [&verbose]{if(verbose) cout << "everyone arrived at the barrier" << endl;});

    //initialize the vector with random values
    if(verbose) cout << "Starting random generation of values for the vector" << endl;
    random_device rnd_device;
    mt19937 mersenne_engine {rnd_device()};
    uniform_int_distribution<int> dist {1, 10};
    generate(v[0].begin(), v[0].end(), [&dist, &mersenne_engine]{
        return dist(mersenne_engine);
    });
    if(verbose) cout << "Random generation ended\n" << endl;

    if(verbose){
        cout << "Vec: " << "";
        for(int i=0; i<v_size; i++) cout << v[0][i] << " ";
        cout << "\n" << endl;
    }

    vector<int> v_sorted = v[0];
    sort(v_sorted.begin(), v_sorted.end());

    int n_changes = 1;
    int row_read = 0;
    int row_write = 1;
    int starting_pos = 0;
    int pos;
    int i;
    bool first = true;

    auto start = chrono::high_resolution_clock::now();
    while(n_changes != 0){
        v[row_write] = v[row_read];
        i = 0;
        n_changes = 0;
        if(first) {
            n_changes = 1;
            first = false;
        }

        for(i=0; i<nw && starting_pos+i<v_size-1; i++){
            workers[i] = async(launch::async,
            [&, i](){
                for(int j=starting_pos+2*i; j<v[row_read].size()-1; j+=2*nw){
                    //string str = "thread " + to_string(i) + " - current position: " + to_string(j);
                    //cout << str << endl;
                    if (v[row_read][j] > v[row_read][j+1]) { // do the swap
                        //str = "thread " + to_string(i) + " is swapping";
                        //cout << str << endl;

                        int tmp = v[row_read][j];
                        v[row_write][j] = v[row_read][j+1];
                        v[row_write][j+1] = tmp;

                        n_changes++;

                        this_thread::sleep_for(10ms);
                    }
                }
                //string str = "thread " + to_string(i) + " finished the iteration";
                //cout << str << endl;
                sync_point.arrive_and_wait();
            });
        }

        sync_point.arrive_and_wait();

        row_read = (row_read + 1) % 2;
        row_write = (row_write + 1) % 2;
        starting_pos = (starting_pos + 1) % 2;

        //cout << "ending iteration\n" << endl;

        /*
        while(pos < v_size - 1){
            for(i=0; i<nw && pos < v_size-1; i++){
                workers[i] = async(launch::async,
                [&](int pos){
                    if(v[row_read][pos] > v[row_read][pos+1]){ //do the swap
                        n_changes++;

                        int tmp = v[row_read][pos];
                        v[row_write][pos] = v[row_read][pos+1];
                        v[row_write][pos+1] = tmp;
                    }
                }, pos);
                pos += 2;
            }
        }
        */
        
        /*
        cout << "vec I read: " << "";
        for(int j=0; j<v_size; j++) cout <<v[row_read][j] << " ";
        cout << endl;

        cout << "vec just modified: " << "";
        for(int j=0; j<v_size; j++) cout << v[row_write][j] << " ";
        cout << "\n" << endl;
        */
    }

    auto elapsed = chrono::high_resolution_clock::now() - start;
    auto time = chrono::duration_cast<chrono::microseconds>(elapsed).count();

    cout << "Elapsed time: " << time << " usecs" << endl;

    if(verbose){
        cout << "Sorted vec: " << "";
        for(int i=0; i<v_size; i++) cout << v[row_write][i] << " ";
        cout << endl;
    }

    assert(is_sorted(v[row_read].begin(), v[row_read].end()));
    assert(v[row_read] == v_sorted);

    return(0);
}