#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

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

float LinuxParser::MemoryUtilization() {
  string name, totalmem, freemem, kb;
  string line;
  float total = 0.0, free = 0.0;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> name >> totalmem >> kb;
    std::getline(stream, line);
    std::istringstream linestream2(line);
    linestream2 >> name >> freemem >> kb;
  }
  total = std::stof(totalmem);
  free = std::stof(freemem);
  return ((total - free) / total);
}

long LinuxParser::UpTime() {
  long uptime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}

long LinuxParser::Jiffies() {
  long ans{0};
  for (auto i : CpuUtilization()) {
    ans += std::stol(i);
  }
  return ans;
}

long LinuxParser::ActiveJiffies() {
  long ans{0};
  vector<string> jif = CpuUtilization();
  for (int i = 0; i < 10; i++) {
    if ((i != 3) && (i != 4)) {
      ans += std::stol(jif[i]);
    }
  }
  return ans;
}

long LinuxParser::IdleJiffies() {
  long ans{0};
  vector<string> jif = CpuUtilization();
  for (int i = 0; i < 10; i++) {
    if ((i == 3) || (i == 4)) {
      ans += std::stol(jif[i]);
    }
  }
  return ans;
}

vector<string> LinuxParser::CpuUtilization() {
  vector<string> ans;
  string cpu, line, data;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    while (linestream >> data) {
      ans.push_back(data);
    }
  }
  return ans;
}

int LinuxParser::TotalProcesses() {
  string procs, line, data;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> procs;
      if (procs == "processes") {
        linestream >> data;
      }
    }
  }
  return std::stoi(data);
}

int LinuxParser::RunningProcesses() {
  string procs, line, data;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> procs;
      if (procs == "procs_running") {
        linestream >> data;
      }
    }
  }
  return std::stoi(data);
}

string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  return line;
}

string LinuxParser::Ram(int pid) {
  string line, data, ans;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> data;
      if (data == "VmSize:") {
        linestream >> ans;
      }
    }
  }
  return ans;
}
string LinuxParser::Uid(int pid) {
  string line, data, ans;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> data;
      if (data == "Uid:") {
        linestream >> ans;
      }
    }
  }
  return ans;
}

string LinuxParser::User(int pid) {
  string name, x, no, match_no, line, ans;
  match_no = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> name >> x >> no;
      if (no == match_no) ans = name;
    }
  }
  return ans;
}

long LinuxParser::UpTime(int pid) {
  string dummy, ans, line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 1; i < 22; i++) {
      linestream >> dummy;
    }
    linestream >> ans;
  }
  return std::stol(ans);
}

// for extracting processs cpu utiliztion information
std::vector<long int> LinuxParser::ProcessCpu(int pid) {
  vector<long int> ansv;
  string dummy, last_attr, line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 1; i < 22; i++) {
      linestream >> dummy;
      // taking all necessary attributes to use
      if ((i == 14) || (i == 15) || (i == 16) || (i == 17))
        ansv.push_back(std::stol(dummy));
    }
    linestream >> last_attr;
    ansv.push_back(std::stol(last_attr));
  }
  return ansv;
}