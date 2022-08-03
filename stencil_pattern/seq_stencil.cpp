#include <vector>
#include <iostream>
#include <chrono>

using namespace std;

int main(int argc, char* argv[]){
  
  if(argc != 4){
    cout << "Usage: vector_size n_iter verbose" << endl;
      return(1);
  }
  
  int vector_size = atoi(argv[1]);
  int n_iter = atoi(argv[2]);
  bool verbose = atoi(argv[3]);
  vector<float> vec(vector_size, 25.0);
  float new_tmp;
  float old_tmp;

  //initialize the vector
  vec[0] = 0.0;
  vec[vector_size-1] = 100.0;
  
  if(verbose){
    cout << "Initial vector:" << " ";
    for(int i=0; i<vector_size; i++) cout << vec[i] << " ";
    cout << endl;
  }

  auto start = chrono::high_resolution_clock::now();
  for(int k=0; k<n_iter; k++){
    old_tmp = 0.0;
    new_tmp = 0.0;
    for(int i=1; i<vector_size-1; i++){
      old_tmp = new_tmp;
      new_tmp = (vec[i-1]+vec[i]+vec[i+1])/3; 
      vec[i-1] = old_tmp;
    }
    vec[vector_size-2] = new_tmp;
  }
  auto end = chrono::high_resolution_clock::now();
  auto elapsed = chrono::duration_cast<chrono::microseconds>(end-start).count();

  if(verbose){
    cout << "Final vector:" << " ";
    for(int i=0; i<vector_size; i++) cout << vec[i] << " ";
    cout << endl;
  }

  cout << "Elapsed time: " << elapsed << " microseconds" << endl;
  cout << "Average time per iteration: " << elapsed/n_iter << " microseconds" << endl;

  return(0);
}

