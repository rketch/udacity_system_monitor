#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t; 
using std::string;
using std::vector;
/*You need to complete the mentioned TODOs in order to satisfy the rubric criteria "The student will be able to extract and display basic data about the system."

You need to properly format the uptime. Refer to the comments mentioned in format. cpp for formatting the uptime.*/

// DTODO: Return the system's CPU
Processor& System::Cpu() { 
  return cpu_; }

// DTODO: Return a container composed of the system's processes  
vector<Process>& System::Processes() {
  //https://knowledge.udacity.com/questions/134260
  vector<int> pids = LinuxParser::Pids();
  for(int c: pids){
    Process this_process(c);
    processes_.push_back(this_process);
  }
  return processes_; }

// DTODO: Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// DTODO: Return the system's memory utilization
float System::MemoryUtilization() { 
  float MemUtil = LinuxParser::MemoryUtilization();
  return MemUtil; }

// DTODO: Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// DTODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// DTODO: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// DTODO: Return the number of seconds since the system started running
long int System::UpTime() { 
  long int uptime = LinuxParser::UpTime();
  return uptime; }
