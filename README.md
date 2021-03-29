# chip-8-emu
Chip-8 Emulator

This is my Chip-8 emulator project which im basing off the following specs:
  - 4kB RAM (4096 addresses, 8-bit stored values)
  - 64x32 pixel display
  - 16 16-bit Variable registers V0-VF
  - 16-bit Program Counter
  - 16-bit Index Counter
  - 60hz delay timer
  - 60hz sound timer

Currently, this emulator is only able to load and run the IBM chip-8 test program.

Supported Instructions:
  - 1NNN - Jump
  - 6XNN - Set register VX
  - 7XNN - Add to register VX
  - ANNN - Set index register
  - DXYN - Draw 
  - 0E00 - Clear screen

Aiming to support all classic chip-8 instructions.
