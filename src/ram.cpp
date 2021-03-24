// Trent Julich ~ 23 March 2021

#include "ram.hpp"

Ram::Ram() {
  // Fill memory with all 0's.
  for (int i = 0; i < memory_.size(); i++) {
    memory_[i].fill(0);
  }
}

/*
  Writes p_value to ram at memory_[p_address].
*/
void Ram::Write(int p_address, std::array<bool, 8> p_value) {
  // Check that the address given is within bounds.
  if (!(p_address < 0 || p_address >= ADDRESSES)) {
    memory_[p_address] = p_value;
  }
}

/*
  Reads the value at memory_[p_address] and returns it.
*/
std::array<bool, 8> Ram::Read(int p_address) {
  std::array<bool, 8> ret_val;
  ret_val.fill(0);
  if (!(p_address < 0 || p_address >= ADDRESSES)) {
    ret_val = memory_[p_address];
  }
  return ret_val;
}