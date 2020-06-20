#include "processor.h"

#include <string>

#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  float cpu_usage;
  //   calculating average
  cpu_usage = (LinuxParser::Jiffies() - LinuxParser::IdleJiffies());
  cpu_usage = cpu_usage / LinuxParser::Jiffies();
  return cpu_usage;
}