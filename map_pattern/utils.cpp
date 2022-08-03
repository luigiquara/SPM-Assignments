#include <chrono>

void active_delay(int usecs) {
  // read current time
  auto start = std::chrono::high_resolution_clock::now();
  auto end   = false;
  while(!end) {
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    auto msec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    if(msec>usecs)
      end = true;
  }
  return;
}

float to_square(float num, int delay){
    active_delay(delay);
    return num*num;
}