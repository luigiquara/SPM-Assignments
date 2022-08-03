#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <future>
#include <vector>
#include <chrono>
#include <numeric>
#include <cassert>

#include "utils.cpp"
//#include "map_naive.cpp"
#include "map_improved.cpp"
//#include "map_simple_way.cpp"

using namespace std;

int main(int argc, char* argv[]){
    if(argc!=5){
        cout << "Parameters: vector_size scheduling_mode nw verbose" << endl;
        return(1); 
    }

    int v_size = stoi(argv[1]);
    int mode = stoi(argv[2]);
    int nw = stoi(argv[3]);
    int verbose = stoi(argv[4]);

    vector<float> vec(v_size);
    iota(vec.begin(), vec.end(), 1.0); //initialize vector with 1.0, 2.0, 3.0...
    vector<float> squared_vec(v_size);

    for(int i=0; i<v_size; i++) squared_vec[i] = vec[i]*vec[i];

    srand(time(NULL));

    if(verbose){
        cout << "Initial vector: ";
        for(int i=0; i<vec.size(); i++) cout << vec[i] << " ";
        cout << endl;
    }

    auto start = chrono::high_resolution_clock::now();
    vec = map(vec, mode, to_square, nw);
    auto elapsed = chrono::high_resolution_clock::now() - start;
    auto time = chrono::duration_cast<chrono::microseconds>(elapsed).count();

    if(verbose){
        cout << "Squared vector: ";
        for(int i=0; i<vec.size(); i++) cout << vec[i] << " ";
        cout << endl;
    }

    assert(vec==squared_vec);
    
    cout << "Elapsed time: " << time << " usecs" << endl;

    return(0);
}