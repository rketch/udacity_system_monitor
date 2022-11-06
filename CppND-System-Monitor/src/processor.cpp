#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <math.h>

#include "processor.h"
#include "linux_parser.h"

// DTODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  int i = 0;
  float cpu_util = 0.0;
  std::ifstream stream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  std::string line;
  while(std::getline(stream, line)){
    if(0 == i){ // if we are on the cpu utilization line
      // thanks https://www.techiedelight.com/convert-string-vector-chars-cpp/
      std::vector<char> v(line.begin(), line.end());
      std::vector<int> spaces(10);
      std::vector<int> util(10);
      int j = 0;
      int k = 0;
      for(int h = 0; h < 5; h++){
        // erase "cpu0 "
        v.erase(v.begin());
      }
      v.push_back(' ');
      for (const char &c: v) {
        if(c == ' '){
          // find where the spaces are
          spaces[k] = j;
          k++;
          }
        j++;
        }
      int last_space_index = 0;
      int places = 0;
      int current_digit = 0;
      for(int h = 0; h < 10; h++){
        places = spaces[h] - last_space_index - 1; // number of digits the util number has
        for(int y = 0; y < places; y++){
          current_digit = last_space_index + y + 1;
          int temp = v[current_digit] - '0';
          util[h] += temp * pow(10,places-y-1);
        }
        last_space_index = spaces[h];
      }
      int nonidle = util[0] + util[1] + util[2] + util[5] + util[6] + util[7];
      int idle = util[3] + util[4];
      int total = idle + nonidle;
      cpu_util = (float)nonidle / (float)total ;     
    }
    i++;
  }
  if(std::isnan(cpu_util)){
    cpu_util = 0.0;
  }
  return cpu_util;
  
  /* from https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux  

      user    nice   system  idle      iowait irq   softirq  steal  guest  guest_nice
cpu  74608   2520   24433   1117073   6176    4054  0        0      0      0
5-space space+2-space
Algorithmically, we can calculate the CPU usage percentage like:

PrevIdle = previdle + previowait
Idle = idle + iowait

PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal
NonIdle = user +     nice +   system +  irq +   softirq + steal

PrevTotal = PrevIdle + PrevNonIdle
Total = Idle + NonIdle

# differentiate: actual value minus the previous one
totald = Total - PrevTotal
idled = Idle - PrevIdle

CPU_Percentage = (totald - idled)/totald
  */
}