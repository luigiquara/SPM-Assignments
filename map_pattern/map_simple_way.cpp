/*
Simple map pattern implementation
Block scheduling is the same
Cyclic scheduling is the one the prof did
*/

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

//compute the map over the vector v.
//mode = 0 -> block scheduling
//mode = 1 -> cyclic scheduling 
vector<float> map(vector<float> v, int mode, function<float(float, int)> f, int nw){
    
    cout << "map_simple_way" << endl;

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
        for(int i=0; i<nw; i++){
            futures[i].get();
        }
    }

    //cyclic scheduling
    else if (mode==1) {
        cout << "Cyclic scheduling" << endl;
        vector<future<void>> futures(nw);
        float prob = 0.3; //probability of delay

        for(int i=0; i<nw; i++){
            futures[i] = async(launch::async,
            [&](){
                for(int j=i; j<v.size(); j+=nw){
                    v[j] = f(v[j], 1000);
                }
            }
            );
        }
        for(int i=0; i<nw; i++){
            futures[i].get();
        }
    }

    return v;
}