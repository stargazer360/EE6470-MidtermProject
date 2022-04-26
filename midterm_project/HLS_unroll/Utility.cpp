#include "Utility.h"

std::ostream& operator<<(std::ostream& os, const Point& val) {
  os << val.x << "," << val.y;
  return os;
}

inline void sc_trace(sc_core::sc_trace_file*& f, const Point& val, std::string name) {
  sc_trace(f, val.x, name + ".x");
  sc_trace(f, val.y, name + ".y");
}