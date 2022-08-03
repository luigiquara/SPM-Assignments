#include <iostream>
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

int one_sort_iteration(vector<int>& v_read, vector<int>& v_write, int start_pos, int nw, int verbose){
    int n_changes = 0;
    int pos = start_pos;
    vector<future<void>> futures(nw);
    v_write = v_read;

    barrier sync_point(nw, [&verbose]{
        if(verbose) cout << "done" << endl;
    });

    while(pos+1 < v_read.size()){
        for(int i=0; i<nw; i++){
            futures[i] = async(launch::async,
                [&v_read, &v_write, &n_changes, &sync_point](int pos1, int pos2){
                    if(v_read[pos1] > v_read[pos2]){ //if needed, perform the swap
                        int tmp = v_read[pos1];
                        v_write[pos1] = v_read[pos2];
                        v_write[pos2] = tmp;

                        n_changes++;
                        sync_point.arrive_and_wait();
                    }
                },
                pos, pos+1);

                pos = pos + 2;
        }

        for(int i=0; i<nw; i++) futures[i].get();

        if(verbose){
            cout << "\nReading vector:" << " ";
            for(int i=0; i<v_write.size(); i++) cout << v_read[i] << " ";
            cout << endl;
        }

        if(verbose){
            cout << "Modified vector:" << " ";
            for(int i=0; i<v_write.size(); i++) cout << v_write[i] << " ";
            cout << endl;
        }
    }

    if(verbose) cout << "\nIteration ended\n" << endl;

    return n_changes;
}

int main(int argc, char* argv[]){
    if(argc != 4){
        cout << "Usage: vector_size nw verbose" << endl;
        return 1;
    }

    int v_size = stoi(argv[1]);
    int nw = stoi(argv[2]);
    int verbose = stoi(argv[3]);
    vector<vector<int>> v(2, vector<int>(v_size));

    //initialize the vector with random values
    if(verbose) cout << "Starting random generation of values for the vector" << endl;
    random_device rnd_device;
    mt19937 mersenne_engine {rnd_device()};
    uniform_int_distribution<int> dist {1, 100};
    generate(v[0].begin(), v[0].end(), [&dist, &mersenne_engine]{
        return dist(mersenne_engine);
    });
    if(verbose) cout << "Random generation ended\n" << endl;

    cout << "Vec: " << "";
    for(int i=0; i<v_size; i++) cout << v[0][i] << " ";
    cout << endl;

    if(verbose) cout << "Starting the sorting\n" << endl;
    int n_changes = 1; //dummy value, just to start
    int start_pos = 0; //starting position, changes for even and odd
    int row_read = 0; //the copy of the vector used to read in the current iteration
    int row_write = 1; //the copy of the vector used to write the changes in the current iteration
    bool first_iter = true;

    auto start = chrono::high_resolution_clock::now();
    while(n_changes != 0){
        if(verbose){
            cout << "Starting position: " << start_pos << endl;
            cout << "Reading row: " << row_read << endl;
            cout << "Writing row: " << row_write << "\n" << endl;
        }

        if(verbose){
            cout << "I'm gonna read from: " << "";
            for(int i=0; i<v_size; i++) cout << v[row_read][i] << " ";
            cout << endl;

            cout << "I'm gonna apply changes to: " << "";
            for(int i=0; i<v_size; i++) cout << v[row_write][i] << " ";
            cout << endl;
        }

        n_changes = one_sort_iteration(v[row_read], v[row_write], start_pos, nw, verbose);
        if(first_iter) n_changes = 1; // I need to force the alg to make at least one "even" and one "odd" iter
        first_iter = false;

        //update the values
        start_pos = (start_pos+1) % 2; //start_pos cycles between 0 and 1;
        row_read = (row_read+1) % 2;
        row_write = (row_write+1) % 2;
    }
    auto elapsed = chrono::high_resolution_clock::now() - start;
    auto time = chrono::duration_cast<chrono::microseconds>(elapsed).count();

    cout << "Elapsed time: " << time << " usecs" << endl;
    
    cout << "Sorted vec: " << "";
    for(int i=0; i<v_size; i++) cout << v[row_write][i] << " ";
    cout << endl;

    assert(is_sorted(v[row_read].begin(), v[row_read].end()));

    if(verbose) cout << "Sorting ended" << endl;

    return(0);
}