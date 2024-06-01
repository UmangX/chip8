#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct {

  // the chip system
  uint8_t ram[4096];
  int pc;
  uint16_t opcode;

} cpu;

cpu chip;

void init_sys(void)
{
  chip.pc = 0x200;
  memset(&chip.ram, 0, 4096);
}

void emu00E0()
{
  uint16_t op = chip.opcode;
  printf("CLEAR THE SCREEN\n");
}

void emu00ee()
{
  uint16_t op = chip.opcode;
  printf("PC == SP SP--\n");
}

void emu1000()
{
  uint16_t op = chip.opcode;
  uint16_t NNN = op & 0x0fff;
  printf("PC == NNN  HERE NNN = %x DON'T INCREMENT THE PC \n", NNN);
}

void decode(void)
{
  uint16_t op = chip.opcode;
  printf("%x\n", op);
}

int load_rom(char *loc)
{
  FILE *rom = fopen(loc, "rb");
  if (rom == NULL) {
    printf("THERE IS A PROBLEM IN THE FILE\n");
  }
  fseek(rom, 0, SEEK_END);
  int rom_size = (int)ftell(rom);
  rewind(rom);
  fread(&chip.ram[0x200], sizeof(uint8_t), rom_size, rom);
  fclose(rom);
  return rom_size;
}

int main(int argc, char *argv[])
{

  if (argc <= 1) {
    printf("PROVIDE THE FILE\n");
    return 0;
  }

  init_sys();
  int rom_size = load_rom(argv[1]);
  for (int i = 0; i < rom_size; i += 2) {
    // update the opcode
    chip.opcode = chip.ram[chip.pc + i] << 8 | chip.ram[chip.pc + (i + 1)];
    decode();
  }

  return 0;
}
