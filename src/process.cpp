#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "format.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {}

int Process::Pid() { return pid_; }

float Process::CpuUtilization() {
  vector<long int> attr = LinuxParser::ProcessCpu(pid_);
  float total_time = attr[0] + attr[1];
  total_time = total_time + attr[2] + attr[3];
  float seconds = (LinuxParser::UpTime()) - (attr[4] / sysconf(_SC_CLK_TCK));
  cpu_ = ((total_time / sysconf(_SC_CLK_TCK)) / seconds);
  return cpu_;
}

string Process::Command() { return LinuxParser::Command(pid_); }

string Process::Ram() {
  string ans = LinuxParser::Ram(pid_);
  // converting kb to mb
  long a = std::stol(ans) / 1000;
  return std::to_string(a);
}

string Process::User() { return LinuxParser::User(pid_); }

long int Process::UpTime() {
  long int sec = (LinuxParser::UpTime(pid_)) / sysconf(_SC_CLK_TCK);

  return sec;
}
