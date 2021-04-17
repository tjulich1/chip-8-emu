import binascii

# Path of file to read in
FILE = "../roms/tetris.rom"
FILE_OUTPUT = "../rom_txt/tetris.txt"

def main():
  with open(FILE, "rb") as rom_file:
    # Read in the first 2 bytes of the file 
    current_instruction = rom_file.read(2)
  
    with open(FILE_OUTPUT, "w") as output: 

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
  
  if first_nibble == "1":
    new_address = instruction[1:]
    output.write(instruction + " -> Jump to " + new_address)
  
  elif first_nibble == "2":
    sub_routine_address = instruction[1:]
    output.write(instruction + " -> Executing subroutine at " + sub_routine_address)
  
  elif first_nibble == "3":
    register_num = instruction[1]
    value = instruction[2:]
    output.write(instruction + " -> Skip following if register " + register_num + " equals " + value)

  elif first_nibble == "4":
    register_num = instruction[1]
    value = instruction[2:]
    output.write(instruction + " -> Skip following if register " + register_num + " doesn't equal " + value)

  elif first_nibble == "5":
    first_register = instruction[1]
    second_register = instruction[2]
    output.write(instruction + " -> Skip following if register " + first_register + " equals register " + second_register)

  elif first_nibble == "6":
    register_num = instruction[1]
    value = instruction[2:]
    output.write(instruction + " -> Store value " + value + " in register " + register_num)

  elif first_nibble == "7":
    register_num = instruction[1]
    value = instruction[2:]
    output.write(instruction + " -> Add value " + value + " to register " + register_num)

  elif first_nibble == "8":
    first_register = instruction[1]
    second_register = instruction[2]
    fourth_nibble = instruction[3]
    if fourth_nibble == "0":
      output.write(instruction + " -> Store value of register " + second_register + " in register " + first_register)
    
    elif fourth_nibble == "1":
      output.write(instruction + " -> Set register " + first_register + " to register " + first_register + " OR " + second_register)
    
    elif fourth_nibble == "2":
      output.write(instruction + " -> Set register " + first_register + " to register " + first_register + " AND " + second_register)

    elif fourth_nibble == "3":
      output.write(instruction, " -> Set register " + first_register + " to register " + first_register + " XOR " + second_register)
    
    elif fourth_nibble == "4":
      output.write(instruction + " -> Add value of register " + second_register + " to register " + first_register)

    elif fourth_nibble == "5":
      output.write(instruction + " -> Subtract value of register " + second_register + " from register " + first_register)

    elif fourth_nibble == "6":
      output.write(instruction + " -> Shift register " + second_register + " one bit right and store in " + first_register)

    elif fourth_nibble == "7":
      output.write(instruction + " -> Store register " + second_register + " minus " + first_register + " in register " + first_register)

    elif fourth_nibble == "e":
      output.write(instruction + " -> Store" + second_register + " shifted left one bit in register " + first_register)

    else:
      output.write("-----> UNKNOWN INSTRUCTION: " + instruction)

  elif first_nibble == "9":
    first_register = instruction[1]
    second_register = instruction[2]
    output.write(instruction + " -> Skip following if value of register " + first_register + " doesn't equal register " + second_register)

  elif first_nibble == "a":
    address = instruction[1:]
    output.write(instruction + " -> Store memory address " + address + " to index register ")

  elif first_nibble == "b":
    address = instruction[1:]
    output.write(instruction + " -> Jump to address " + address + " + V0 ")

  elif first_nibble == "c":
    register_num = instruction[1]
    mask = instruction[2:]
    output.write(instruction + " -> Store random number with mask of " + mask + " to register " + register_num)

  elif first_nibble == "d":
    output.write(instruction + " -> Draw sprite ")

  elif first_nibble == "e":
    last_half = instruction[2:]
    register_num = instruction[1]
    if last_half == "9e":
      output.write(instruction + " -> Skip following if key corresponding to value in " + register_num + " is pressed ")

    elif last_half == "a1":
      output.write(instruction + " -> Skip following if key corresponding to value in " + register_num + " is not pressed ")

    else:
      output.write("-----> UNKNOWN INSTRUCTION: " + instruction)

  elif first_nibble == "f":
    last_half = instruction[2:]
    register_num = instruction[1]
    if last_half == "07":
      output.write(instruction + " -> Store delay in register " + register_num)

    elif last_half == "0a":
      output.write(instruction + " -> Wait for keypress and store in register " + register_num)
    
    elif last_half == "15":
      output.write(instruction + " -> Set delay timer to value in " + register_num)

    elif last_half == "18":
      output.write(instruction + " -> Set sound timer to value in " + register_num)

    elif last_half == "1e":
      output.write(instruction + " -> Add value stored in register " + register_num + " to index register ")

    elif last_half == "29":
      output.write(instruction + " -> Set I to memory address of sprite corresponding to character in " + register_num)

    elif last_half == "33":
      output.write(instruction + " -> Store binary coded decimal in register " + register_num)

    elif last_half == "55":
      output.write(instruction + " -> Store values from register 0 to register " + register_num + " to memory ")

    elif last_half == "65":
      output.write(instruction + " -> Fill registers 0 to " + register_num + " with values from memory ")

    else:
      output.write("-----> UNKNOWN INSTRUCTION: " + instruction)
  else:
    output.write("-----> UNKNOWN INSTRUCTION: " + instruction)

if __name__ == "__main__":
  main()