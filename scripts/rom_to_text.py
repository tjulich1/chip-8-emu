import binascii
import getopt
import sys

# Path of file to read in


def print_help():
  print("rom_to_text.py -i <inputfile> -o <outputfile>")

def main(argv):

  input_file = ""
  output_file = ""

  try:
    # Option flags that the script can recognize
    # -h: Help, prints script syntax
    # -o (required): file to output to
    # -i (required): input file to read rom from
    opts, args = getopt.getopt(argv, "hi:o:")
  except getopt.GetoptError:
    print_help()
    sys.exit(2)
  for opt, arg in opts:
    if opt == "-h":
      print_help()
    elif opt == "-i":
      input_file = arg
    elif opt == "-o":
      output_file = arg


  with open(input_file, "rb") as rom_file:
    # Read in the first 2 bytes of the file 
    current_instruction = rom_file.read(2)
  
    with open(output_file, "w") as output: 

      # While there are more instructions to read in
      while current_instruction:
        # Print current instruction 
        instruction_as_string = binascii.hexlify(current_instruction).decode("utf-8")
        decode_instruction(instruction_as_string, output)

        output.write('\n')

        # Grab next instruction.
        current_instruction = rom_file.read(2)
      output.close()    
    rom_file.close()

def decode_instruction(instruction, output):

  first_nibble = instruction[0]
  
  if first_nibble == "0":

    # 00e0: Clear screen
    if instruction[2] == "e" and instruction[3] == "0":
      output.write(instruction + ": Clear screen")

    # 00ee: Return from subroutine
    elif instruction[2] == "e" and instruction[3] == "e":
      output.write(instruction + ": Return")
    else:
      output.write("-----> UNKNOWN INSTRUCTION: " + instruction)
  
  # 1nnn: Jump
  elif first_nibble == "1":
    new_address = instruction[1:]
    output.write(instruction + ": PC <- " + new_address)
  
  # 2nnn: Call subroutine
  elif first_nibble == "2":
    sub_routine_address = instruction[1:]
    output.write(instruction + ": Executing subroutine at " + sub_routine_address)
  
  # 3xnn: Skip if VX == nn
  elif first_nibble == "3":
    register_num = instruction[1]
    value = instruction[2:]
    output.write(instruction + ": Skip following if V" + register_num + " == " + value)

  # 4xnn: Skip if VX != nn
  elif first_nibble == "4":
    register_num = instruction[1]
    value = instruction[2:]
    output.write(instruction + ": Skip following if V" + register_num + " != " + value)

  # 5xy0: Skip if VX == VY
  elif first_nibble == "5":
    if instruction[3] == "0":
      first_register = instruction[1]
      second_register = instruction[2]
      output.write(instruction + ": Skip following if V" + first_register + " == V" + second_register)
    else:
      output.write("-----> UNKNOWN INSTRUCTION: " + instruction)

  # 6xnn: VX <- nn
  elif first_nibble == "6":
    register_num = instruction[1]
    value = instruction[2:]
    output.write(instruction + ": V" + register_num + " <- " + value)

  # 7xnn: VX <- VX + nn
  elif first_nibble == "7":
    register_num = instruction[1]
    value = instruction[2:]
    output.write(instruction + ": V" + register_num + " <- V"  + register_num + " + "  + value)

  elif first_nibble == "8":
    first_register = instruction[1]
    second_register = instruction[2]
    fourth_nibble = instruction[3]

    # 8xy0: VX <- VY
    if fourth_nibble == "0":
      output.write(instruction + ": V" + first_register + " <- V" + second_register)
    
    # 8xy1: VX <- VX | VY
    elif fourth_nibble == "1":
      output.write(instruction + ": V" + first_register + " <- V" + first_register + " OR V" + second_register)
    
    # 8xy2: VX <- VX & VY
    elif fourth_nibble == "2":
      output.write(instruction + ": V" + first_register + " <- V" + first_register + " AND V" + second_register)

    # 8xy3: VX <- VX ^ VY
    elif fourth_nibble == "3":
      output.write(instruction, ": V" + first_register + " <- V" + first_register + " XOR V" + second_register)
    
    # 8xy4: VX <- VX + VY
    elif fourth_nibble == "4":
      output.write(instruction + ": V" + first_register + "<- V" + first_register + " + " + "V" + second_register)

    # 8xy5: VX <- VX - VY
    elif fourth_nibble == "5":
      output.write(instruction + ": V" + first_register + " <- V" + first_register + " - V" + second_register)

    # 8xy6: VX <- VY >> 1
    elif fourth_nibble == "6":
      output.write(instruction + ": V" + first_register + " <- V" + second_register + " << 1")

    # 8xy7: VX <- VY - VX
    elif fourth_nibble == "7":
      output.write(instruction + ": V" + first_register + " <- V" + second_register + " - V" + first_register)

    # 8xye: VX <- VY << 1
    elif fourth_nibble == "e":
      output.write(instruction + ": V" + first_register + " <- V" + second_register + " << 1")

    else:
      output.write("-----> UNKNOWN INSTRUCTION: " + instruction)

  # 9xy0: Skip if VX != V
  elif first_nibble == "9":
    if instruction[3] == "0":
      first_register = instruction[1]
      second_register = instruction[2]
      output.write(instruction + ": Skip if V" + first_register + " != V" + second_register)
    else:
      output.write("-----> UNKNOWN INSTRUCTION: " + instruction)

  # annn: Index <- nnn
  elif first_nibble == "a":
    address = instruction[1:]
    output.write(instruction + ": Index <- " + address)

  # bnnn: PC <- V0 + nnn
  elif first_nibble == "b":
    address = instruction[1:]
    output.write(instruction + ": PC <- " + address + " + V0 ")

  #cxnn: VX <- Random & nn
  elif first_nibble == "c":
    register_num = instruction[1]
    mask = instruction[2:]
    output.write(instruction + ": V" + register_num + " <- random & " + mask)

  # dxyn: Draw sprite
  elif first_nibble == "d":
    output.write(instruction + ": Draw sprite " + '\n')
    output.write("  -> Rows: " + instruction[3] + '\n')
    output.write("  -> X: V" + instruction[1] + '\n')
    output.write("  -> Y: V" + instruction[2])

  elif first_nibble == "e":
    last_half = instruction[2:]
    register_num = instruction[1]

    # ex9e: Skip if key corresponding to value in VX is pressed
    if last_half == "9e":
      output.write(instruction + ": Skip if key corresponding to value in V" + register_num + " is pressed ")

    # exa1: Skip if key corresponding to value in VX is not pressed
    elif last_half == "a1":
      output.write(instruction + ": Skip if key corresponding to value in V" + register_num + " is not pressed ")

    else:
      output.write("-----> UNKNOWN INSTRUCTION: " + instruction)

  elif first_nibble == "f":
    last_half = instruction[2:]
    register_num = instruction[1]

    # fx07: VX <- Delay register
    if last_half == "07":
      output.write(instruction + ": V" + register_num + " <- delay register")

    # fx0a: VX <- Wait for key press
    elif last_half == "0a":
      output.write(instruction + ": V" + register_num + " <- Wait for key press")
    
    #fx15: Delay <-  VX
    elif last_half == "15":
      output.write(instruction + ": Delay <- V" + register_num)

    # fx18: Sound <- VX
    elif last_half == "18":
      output.write(instruction + ": Sound <- V" + register_num)

    # fx1e: Index <- index + VX
    elif last_half == "1e":
      output.write(instruction + ": Index <- Index + V" + register_num)

    # fx29: I <- mem address of sprite of char in VX
    elif last_half == "29":
      output.write(instruction + ": Index <- Mem address of sprite of char in V" + register_num)

    # fx33: VX <- Binary coded decimal from mem[index]
    elif last_half == "33":
      output.write(instruction + ": V" + register_num + " <- Binary coded decimal starting at Index")

    # fx55: mem[index + x] <- V0..VX
    elif last_half == "55":
      output.write(instruction + ": Mem[index + " + register_num + "] <- V0...V" + register_num)

    elif last_half == "65":
      output.write(instruction + ": V0..V" + register_num + " <- mem[index +" + register_num + "]")

    else:
      output.write("-----> UNKNOWN INSTRUCTION: " + instruction)
  else:
    output.write("-----> UNKNOWN INSTRUCTION: " + instruction)

if __name__ == "__main__":
  main(sys.argv[1:])