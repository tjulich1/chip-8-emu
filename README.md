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

Supports all 35 original Chip-8 instructions, a list of which can be found [here](https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Set)

Big thanks to Matt Mikolay who created the above list, as it was extremely helpful for developing this emulator!

Although all 35 instructions are implemented, there are still some bugs present in some roms. I have only confirmed that Tetris and the IBM Logo Rom work as expected.

Features in progress:
  - View panel for the 16 variable registers, to show values during runtime. 
  - View panel for viewing contents of memory.

![Capture](https://user-images.githubusercontent.com/33639246/116004825-6e1f2600-a5b9-11eb-8876-5bf1c1a4dac5.JPG)
![Capture](https://user-images.githubusercontent.com/33639246/113223386-e6eec480-923d-11eb-8411-ddf3da644aa2.JPG)
