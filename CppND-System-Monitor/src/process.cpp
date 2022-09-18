#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int p){
	pid_ = p; 
  	cpu = LinuxParser::CpuUtilization(p);
};

// DTODO: Return this process's ID
int Process::Pid() { 
  int pid = pid_;
  return pid; } 

// DTODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
  return LinuxParser::CpuUtilization(pid_); }

// DTODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// DTODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// DTODO: Return the user (name) that generated this process
string Process::User() { 
  return LinuxParser::User(pid_); }

// DTODO: Return the age of this process (in seconds)
long int Process::UpTime() { 
  return LinuxParser::UpTime(pid_); }


// DTODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
  bool less_than;
  if(this->cpu < a.cpu){
    less_than = true;
  }
  else{
    less_than = false;
  }
  return less_than; }