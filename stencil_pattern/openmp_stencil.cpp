#include <vector>
#include <iostream>
#include <chrono>

using namespace std;

int main(int argc, char* argv[]){
  
  if(argc != 5){
    cout << "Usage: vector_size n_iter n_workers verbose" << endl;
      return(1);
  }
  
  int vector_size = atoi(argv[1]);
  int n_iter = atoi(argv[2]);
  int n_workers = atoi(argv[3]);
  bool verbose = atoi(argv[4]);
  vector<float> reading_vec(vector_size, 25.0);
  vector<float> writing_vec(vector_size);

  //initialize the vector
  reading_vec[0] = 0.0;
  reading_vec[vector_size-1] = 100.0;
  writing_vec = reading_vec;
  
  if(verbose){
    cout << "Initial vector:" << " ";
    for(int i=0; i<vector_size; i++) cout << reading_vec[i] << " ";
    cout << endl;
  }

  auto start = chrono::high_resolution_clock::now();
  for(int k=0; k<n_iter; k++){
#pragma omp parallel for num_threads(n_workers)
    for(int i=1; i<vector_size-1; i++){
      writing_vec[i] = (reading_vec[i-1]+reading_vec[i]+reading_vec[i+1])/3;
    }
    reading_vec.swap(writing_vec);
  }
  auto end = chrono::high_resolution_clock::now();
  auto elapsed = chrono::duration_cast<chrono::microseconds>(end-start).count();

  if(verbose){
    cout << "Final vector:" << " ";
    for(int i=0; i<vector_size; i++) cout << reading_vec[i] << " ";
    cout << endl;
  }

  cout << "Elapsed time: " << elapsed << " microseconds" << endl;

  return(0);
}

