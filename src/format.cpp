#include "format.h"

#include <string>

using std::string;

string Format::ElapsedTime(long seconds) {
  int h, m, s;
  int m_;  // temporary minutes
  h = seconds / 3600;
  m_ = seconds % 3600;
  m = m_ / 60;
  s = m_ % 60;
  string ans =
      std::to_string(h) + ":" + std::to_string(m) + ":" + std::to_string(s);
  return ans;
}