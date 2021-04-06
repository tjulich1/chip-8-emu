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

Supported Instructions:
  - 0E00 - Clear screen
  - 1NNN - Jump
  - 3XNN - Skip if equal
  - 6XNN - Set register VX
  - 7XNN - Add to register VX
  - 8XY0 - Store VY in VX
  - ANNN - Set index register
  - BNNN - Set program counter
  - DXYN - Draw sprite
  - FX15 - Set delay timer
  - FX18 - Set sound timer
  - FX1E - Add register to index
  - FX33 - Store binary coded decimal
  - FX55 - Store registers to memory
  - FX65 - Read memory to registers

Aiming to support all classic chip-8 instructions.

Currently, this emulator has only been tested and confirmed to work on the IBM Chip-8 test rom (included in this repo).

![Capture](https://user-images.githubusercontent.com/33639246/113223386-e6eec480-923d-11eb-8411-ddf3da644aa2.JPG)
