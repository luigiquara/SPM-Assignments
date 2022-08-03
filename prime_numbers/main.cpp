#include <iostream>
#include <future>
#include <vector>
#include <mutex>
#include <chrono>

using namespace std;

mutex m;

void active_delay(int msecs) {
  // read current time
  auto start = chrono::high_resolution_clock::now();
  auto end   = false;
  while(!end) {
    auto elapsed = chrono::high_resolution_clock::now() - start;
    auto msec = chrono::duration_cast<chrono::milliseconds>(elapsed).count();
    if(msec>msecs)
      end = true;
  }
  return;
}

bool is_prime(int n){
  if(n==2 || n==3) return true;

  if(n<=1 || n%2==0 || n%3==0) return false;

  for(int i = 5; i*i <= n; i +=6){
    if(n%i == 0 || n%(i+2) == 0) return false;
  }

  active_delay(100);

  return true;
}

int main(int argc, char* argv[]){
  if(argc != 4){
	cout << "Usage: start end nw" << endl;
	return(1);
  }
  
  int start = atoi(argv[1]);
  int end = atoi(argv[2]);
  int nw = atoi(argv[3]);

  if(nw == 0){ //sequential code
    
  }
  
  else{ //parallel code
    vector<future<void>> workers(nw);
    vector<int> primes;

    auto start_time = chrono::high_resolution_clock::now();

    for(int i=0; i<nw; i++){
      workers[i] = async(launch::async,
          [&, i](){
            for(int j=start+i; j<=end; j+=nw){
              if(is_prime(j)){
                unique_lock<mutex> lock(m);
                primes.push_back(j);
              } 
            }
          });
    }

    for(int i=0; i<nw; i++) workers[i].get();
    auto end_time = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::microseconds>(end_time-start_time).count();

    for(int i=0; i<primes.size(); i++) cout << primes[i] << " ";
    cout << endl;

    cout << "elapsed time: " << elapsed << " usecs" << endl;
  }

  return(0);
}
