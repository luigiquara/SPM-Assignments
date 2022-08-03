/*
Naive map pattern implementation
Block scheduling is the same
Cyclic scheduling is naive:
    + it starts nw asyncs one after the other
    + then gets the results in order
    + repeat until pos < v.size()
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

    cout << "map naive" << endl;

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
        vector<future<float>> futures(nw);
        float prob = 0.3; //probability of delay

        int i = 0;
        int pos = 0;

        //start nw asyncs
        //get their results, and repeat
        while(pos<v.size()){
            while(i<nw){
                float rng = (float) rand()/(float) RAND_MAX; //random number between 0 and 1
                int delay = (rng < prob) ? 5000:100;
                futures[i] = async(launch::async, f, v[pos], delay);

                assert(i == pos%nw); //check that we assign the right element to the right thread

                i++;
                pos++;
            }

            for (i=0; i<nw; i++){
                v[pos-i-1] = futures[i].get();
            }

            i = 0;
        }
    }

    return v;
}