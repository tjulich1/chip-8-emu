// Trent Julich ~ 23 March 2021

#ifndef RAM_HPP
#define RAM_HPP

#include <array>
#include <bitset>

/**
 * This class represents the RAM of the Chip-8 emulator, containing 4096 addresses holding 8-bit
 * values (4kB). 
**/
class Ram {

public:
  /*
  * Writes the value p_value into memory at p_address.
  */
  void Write(int p_address, std::bitset<8> p_value);

  /*
  * Returns the value that is stored in memory_[p_address]. If ADDRESSES < p_address < 0 Read will
  * return an array containing all 0.
  */
  std::bitset<8> Read(int p_address);
private:
  /*
  * The number of memory addresses that can be accessed.
  */  
  const static int ADDRESSES = 4096;

  /*
  * Each element of the memory_ array is a boolean array, representing an 8-bit binary string 
  */  
  std::array<std::bitset<8>, ADDRESSES> memory_;
};

#endif