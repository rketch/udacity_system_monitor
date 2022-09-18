#include <string>
#include <vector>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
  std::vector<long> secs = {60, 3600, 86400};
  std::vector<long> cur_time = {0, 0, 0};
  long flag = 0;
  while (!flag){
    // make sure we are less than 86400 seconds into the current day
    if(seconds < secs[2]){
      // hours (auto round down)
      cur_time[0] = seconds / secs[1];
      // seconds since the hour started
      long secs_since_hour_start = seconds % secs[1];
      // minutes (auto round down)
      cur_time[1] = secs_since_hour_start / secs[0];
      // seconds since minute started
      cur_time[2] = secs_since_hour_start % secs[0];
      flag = 1;
    }
    else{
      seconds -= secs[2];
    }
  }
  string time_today; // of form HH:MM:SS
  for(int i = 0; i < 2; i++){
    if (cur_time[i] < 10){
      // if there is only one digit in our time
      time_today.append("0");
      time_today.append(std::to_string(cur_time[i]));
    }
    else{
      // if there are 2 digits in our time
      time_today.append(std::to_string(cur_time[i]));
    }
    time_today.append(":");
  }
  if (cur_time[2] < 10){
    time_today.append("0");
    time_today.append(std::to_string(cur_time[2]));
  }
  else{
    time_today.append(std::to_string(cur_time[2]));
  }
  return time_today; }