// Trent Julich ~ 23 March 2021

#include "ram.hpp"

/*
  Writes p_value to ram at memory_[p_address].
*/
void Ram::Write(int p_address, std::bitset<8> p_value) {
  // Check that the address given is within bounds.
  if (!(p_address < 0 || p_address >= ADDRESSES)) {
    memory_[p_address] = p_value;
  }
}

void Ram::Write(int p_address, std::vector<std::bitset<8>> p_values) {
  for (int i = 0; i < p_values.size(); i++) {
    if ( (p_address + i < ADDRESSES) && (p_address >= 0) ) {
      memory_[p_address + i] = p_values[i];
    }
  }
}

/*
  Reads the value at memory_[p_address] and returns it.
*/
std::bitset<8> Ram::Read(int p_address) {
  std::bitset<8> ret_val("00000000");
  if (!(p_address < 0 || p_address >= ADDRESSES)) {
    ret_val = memory_[p_address];
  }
  return ret_val;
}