#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <math.h>
#include <algorithm>
#include <unistd.h>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DTODO: Read and return the system memory utilization currently as percentage
float LinuxParser::MemoryUtilization() { 
  int i = 0;
  int mem_total;
  int mem_free;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
    string line;
    while(std::getline(stream, line)){
      if(0 == i){
        // thanks https://stackoverflow.com/questions/61144980/extract-all-numbers-from-stringstream
        line.erase(std::remove_if(line.begin(), line.end(), [](unsigned char c) { return !std::isdigit(c); }), line.end());
        mem_total = std::stoi(line);
      }
      if(1 == i){
        // thanks https://stackoverflow.com/questions/61144980/extract-all-numbers-from-stringstream
        line.erase(std::remove_if(line.begin(), line.end(), [](unsigned char c) { return !std::isdigit(c); }), line.end());
        mem_free = std::stoi(line);
      }
      i++;
    }
  float mem_util;
  mem_util = (((float)mem_total - (float)mem_free)) / (float)mem_total;
  return mem_util; }

// DTODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  vector<string> c;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  std::string line;
  long uptime;
  while (getline(stream, line)){
    int space = line.find(".", 0);
    line.resize(space);
    uptime = std::stoi(line);
  }
  return uptime;}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
//long LinuxParser::ActiveJiffies(int pid) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
float LinuxParser::CpuUtilization(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  string line;
  std::vector<int> info_spaces = {13, 14, 15, 16, 21};
  std::vector<long> info = {0, 0, 0, 0, 0};
  std::vector<long> temp_vec;
  int i = 0;
  int j = 0;
  int k = 0;
  // throw away everything until we find the right space
  while(std::getline(stream, line)){
    std::vector<char> v(line.begin(), line.end());
    for(char c: v){
      if(i == info_spaces[j]){
        // if we currently want to capture data
        if(isdigit(c)){
          long temp = c - '0';
          temp_vec.push_back(temp);
          k++;
        }
        else{
          std::reverse(temp_vec.begin(), temp_vec.end());
          int t = 0;
          for(int y: temp_vec){
            info[j] += y * pow(10,t);
            t++;
          }
          temp_vec.clear();
          // if we're done with this data
          j++;
        }
      }
      if(c == ' '){
        i++; // iterate by a space at the end
        k = 0;
      }
    }
  }
  long time = info[0] + info[1] + info[2] + info[3];
  long uptime = LinuxParser::UpTime();
  float hz = sysconf(_SC_CLK_TCK);
  float since_start = (float)uptime - ((float)info[4] / hz);
  float cpu_usage = ((float)time / hz / since_start);
  /*
  #0 utime - CPU time spent in user code, measured in clock ticks
#1 stime - CPU time spent in kernel code, measured in clock ticks
#2 cutime - Waited-for children's CPU time spent in user code (in clock ticks)
#3 cstime - Waited-for children's CPU time spent in kernel code (in clock ticks)
#4 starttime - Time when the process started, measured in clock ticks
total_time = utime + stime
total_time = total_time + cutime + cstime
seconds = uptime - (starttime / Hertz)
cpu_usage = 100 * ((total_time / Hertz) / seconds)
  */
  return cpu_usage; }

// DTODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  int i = 0;
  int num_proc;
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line;
  while(std::getline(stream, line)){
    if(6 == i){
      // thanks https://stackoverflow.com/questions/61144980/extract-all-numbers-from-stringstream
      line.erase(std::remove_if(line.begin(), line.end(), [](unsigned char c) { return !std::isdigit(c); }), line.end());
      num_proc = std::stoi(line);
    }
    i++;
  }
  return num_proc; }

// DTODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {   
  int i = 0;
  int run_proc;
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line;
  while(std::getline(stream, line)){
    if(7 == i){
      // thanks https://stackoverflow.com/questions/61144980/extract-all-numbers-from-stringstream
      line.erase(std::remove_if(line.begin(), line.end(), [](unsigned char c) { return !std::isdigit(c); }), line.end());
      run_proc = std::stoi(line);
    }
    i++;
  }
  return run_proc;}

// TODO: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  string line;
  string cmdline;
  while(std::getline(stream, line)){
    cmdline = line;
  }
  return cmdline; }

// DTODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  int i = 0;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  string line;
  string ram;
  while(std::getline(stream, line)){
    if(17 == i){
      std::vector<char> v(line.begin(), line.end());
      v.erase(v.begin()); // aparantly for loops dont work today
      v.erase(v.begin());
      v.erase(v.begin());
      v.erase(v.begin());
      v.erase(v.begin());
      v.erase(v.begin());
      v.erase(v.begin());
      v.erase(v.begin());
      bool flag = 1;
      for (const char &c: v){
        // find all of the digits for the first uid
        if(isdigit(c) && flag){
          ram.push_back(c);
        }
        else if(c == ' '){
        }
        else{
          flag = 0;
        }
      }
    }
  i++;
  }
    long ra = std::stol(ram);
    string r = to_string( ra / 1000);
  return r;
}

//DTODO: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  int i = 0;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  string line;
  string user;
  while(std::getline(stream, line)){
    if(8 == i){
      std::vector<char> v(line.begin(), line.end());
      v.erase(v.begin()); // aparantly for loops dont work today
      v.erase(v.begin());
      v.erase(v.begin());
      v.erase(v.begin());
      v.erase(v.begin());
      v.erase(v.begin());
      v.erase(v.begin());
      v.erase(v.begin());
      v.erase(v.begin());
      bool flag = 1;
      for (const char &c: v){
        // find all of the digits for the first uid
        if(isdigit(c) && flag){
          user.push_back(c);
        }
        else{
          flag = 0;
        }
      }
    }
  i++;
  }
  return user; }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  string line;
  int info_space = 21;
  long info = 0;
  std::vector<long> temp_vec;
  int i = 0;
  int k = 0;
  // throw away everything until we find the right space
  while(std::getline(stream, line)){
    std::vector<char> v(line.begin(), line.end());
    for(char c: v){
      if(i == info_space){
        // if we currently want to capture data
        if(isdigit(c)){
          long temp = c - '0';
          temp_vec.push_back(temp);
          k++;
        }
        else{
          std::reverse(temp_vec.begin(), temp_vec.end());
          int t = 0;
          for(int y: temp_vec){
            info += y * pow(10,t);
            t++;
          }
          temp_vec.clear();
          // if we're done with this data
        }
      }
      if(c == ' '){
        i++; // iterate by a space at the end
        k = 0;
      }
    }
  }
  info = info / sysconf(_SC_CLK_TCK);
  return info; }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string user_id = LinuxParser::Uid(pid);
  std::ifstream stream(LinuxParser::kPasswordPath);
  string line;
  string user;
  while(std::getline(stream, line)){
      string temp_string;
    std::vector<long> temp_vec;
    std::vector<char> v(line.begin(), line.end());
    int colon_counter = 0;
    for(char c: v){
      if(colon_counter == 2){
        if(isdigit(c)){
          // retreive the uid
          temp_string += c;
        }
      }
      if(c == ':'){
          colon_counter ++;
      }
    }
    if(user_id == temp_string){
      int col_count_2 = 0;
      for(char c: v){
        if((col_count_2 == 0)&&(c != ':')){
          user.push_back(c);
        }
        if(c == ':'){
          col_count_2++;
        }
      }
    }
  temp_vec.clear();
  }
  //std::reverse(user.begin(), user.end());
    // after second : is user id
    // scrape current uid
    // if uid corresponds to input uid
    // scrape again for everything until first :
    
  return user; }