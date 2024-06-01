#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint16_t combine_bytes(uint8_t b1, uint8_t b2) { return (b1 << 8) | b2; }

const int WIDTH = 400;
const int HEIGHT = 400;
const int RAM_SIZE = 4096;
const int START_ADD = 0x200;

typedef struct {
  uint8_t ram[4096];
  int pc;
} chip;

void clear_ram(chip *cpu) {
  for (int i = 0; i < RAM_SIZE; i++) {
    cpu->ram[i] = 0;
  }
}

void load_rom(chip *cpu) {
  char *file_address = "/Users/synyster7x/Downloads/ibm.ch8";
  cpu->pc = 0x200;
  FILE *rom = fopen(file_address, "rb");
  if (rom == NULL) {
    printf("the file isn't loaded there is an issue \n");
  } else {
    fseek(rom, 0, SEEK_END);
    long rom_size = ftell(rom);
    rewind(rom);
    fread(&cpu->ram[START_ADD], sizeof(uint8_t), rom_size, rom);
    fclose(rom);
  }
}

int main() {
  chip cpu;
  clear_ram(&cpu);
  load_rom(&cpu);
  printf("the data at start %d \n", cpu.ram[3]);
  printf("the data at start of rom %d \n", cpu.ram[0x200 + 5]);

  // don't remove this is for printing
  printf("\n");
  return 0;
}

int previous_main() {
  // the ram arrays and clearing all the data to zero
  uint8_t ram[4096];
  for (int i = 0; i < RAM_SIZE; i++)
    ram[i] = 0;

  int pc = 0x200;
  // create a filestream and read from it
  char *file_address = "/Users/synyster7x/Downloads/ibm.ch8";
  FILE *rom = fopen(file_address, "rb");

  if (rom == NULL) {
    printf("file is not there where it is supposed to be \n");
  } else {

    // read the file
    printf("file is found and is going to be read\n");
  }
  // seek to the end of the file and ask for the position
  fseek(rom, 0, SEEK_END);
  long rom_size = ftell(rom);
  printf("the size of the file is %ld \n", rom_size);
  rewind(rom);
  fread(&ram[START_ADD], sizeof(uint8_t), rom_size, rom);
  fclose(rom);

  // bool state = true;
  // while (state) {
  //   uint8_t b1 = ram[pc];
  //   uint8_t b2 = ram[pc + 1];
  //   uint16_t opcode = combine_bytes(b1, b2);
  //   printf("%d\n", opcode);
  //   pc += 2;
  //   if (pc > (rom_size + START_ADD)) {
  //     state = false;
  //   }
  // }

  printf("\n"); // don't remove its for the printing the new line
  return 0;
}
