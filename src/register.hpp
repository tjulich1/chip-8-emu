// Trent Julich ~ 25 March 2021

#ifndef REGISTER_HPP
#define REGISTER_HPP

#include <iostream>
#include <bitset>

/*
* Class representing an 8-bit register. 8 bits can be written and read from 
* the register.
*/
template <int register_size_>
class Register {

public:

  /**
   * Stores the bits p_bits in the register.
   */
  void Write(std::bitset<register_size_> p_bits);

  /**
   * Reads and returns the bits that are stored in the register.
   */
  std::bitset<register_size_> Read();

  /*
  * Used to print the contents of the register to the console.
  */
  void Print();
private: 

  /**
   * The array of bits stored in register.
   */
  std::bitset<register_size_> bits_; 
};

template<int register_size_>
void Register<register_size_>::Write(std::bitset<register_size_> p_bits) {
  bits_ = p_bits;
}

template<int register_size_>
std::bitset<register_size_> Register<register_size_>::Read() {
  return bits_;
}

template<int register_size_>
void Register<register_size_>::Print() {
  std::cout << "Register: ";
  for (int i = bits_.size() - 1; i > -1; i--) {
    std::cout << bits_[i];

    if ((i % 4) == 3) {
      std::cout << ' ';
    }
  }
  std::cout << std::endl;
}

#endif