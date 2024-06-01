#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RAM_SIZE 4096
#define FONTSET_SIZE 80
#define FONTSET_START 0x50
#define START_ADDRESS 0x200
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define SCALE 10
#define newline printf("\n")

typedef struct {
  uint8_t ram[4096];
  int pc;
  uint16_t opcode;
  uint8_t regs[16];
  uint16_t index;
  uint8_t sp;
  uint16_t stack[16];
  uint8_t display[SCREEN_WIDTH * SCREEN_HEIGHT];
} chip;

void load_fonts(chip *brd) {
  uint8_t font_set[FONTSET_SIZE] = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  for (int i = 0; i < FONTSET_SIZE; i++) {
    brd->ram[FONTSET_START + i] = font_set[i];
  }
}

void init_chip(chip *brd) {
  brd->pc = START_ADDRESS;
  for (int i = 0; i < 16; i++) {
    brd->regs[i] = 0;
    brd->stack[i] = 0;
  }
  brd->index = 0;
  brd->sp = 0;
  memset(brd->display, 0, SCREEN_WIDTH * SCREEN_HEIGHT);
  for (int i = 0; i < RAM_SIZE; i++)
    brd->ram[i] = 0;
  load_fonts(brd);
}

void load_rom(chip *brd, char *location) {
  FILE *rom = fopen(location, "rb");
  if (rom == NULL) {
    fprintf(stderr, "Failed to open ROM file: %s\n", location);
    exit(EXIT_FAILURE);
  }
  fseek(rom, 0, SEEK_END);
  long rom_size = ftell(rom);
  rewind(rom);
  fread(&brd->ram[0x200], sizeof(uint8_t), rom_size, rom);
  fclose(rom);
}

void fetch(chip *cpu) {
  uint8_t byte1 = cpu->ram[cpu->pc];
  uint8_t byte2 = cpu->ram[cpu->pc + 1];
  uint16_t opc = (byte1 << 8) | byte2;
  cpu->opcode = opc;
}

void emu_0000(uint16_t op, chip *brd) {
  if (op == 0x00e0) {
    memset(brd->display, 0, SCREEN_WIDTH * SCREEN_HEIGHT);
    printf("DISPLAY HAS BEEN CLEARED\n");
  } else if (op == 0x00ee) {
    --brd->sp;
    brd->pc = brd->stack[brd->sp];
    printf("THE PC HAS BEEN UPDATED TO %x %d \n", brd->pc, brd->pc);
  }
  brd->pc += 2;
}

void emu_1000(uint16_t op, chip *brd) {
  brd->pc = op & 0x0fff;
  printf("THE PC HAS BEEN UPDATED TO HEX : %x DEC : %d \n", brd->pc, brd->pc);
}

void emu_2000(uint16_t op, chip *brd) {
  uint16_t address = op & 0x0fff;
  brd->stack[brd->sp] = brd->pc;
  ++brd->sp;
  brd->pc = address;
  printf("THE PC AND THE SP HAS BEEN UPDATED TO PC: %x %d SP: %x %d\n", brd->pc,
         brd->pc, brd->sp, brd->sp);
}

void emu_3000(uint16_t op, chip *brd) {
  uint8_t vx = (op & 0x0f00u) >> 8u;
  uint8_t byte = op & 0x00ffu;
  if (brd->regs[vx] == byte) {
    brd->pc += 2;
    printf("PC HAS BEEN UPDATED\n");
  } else {
    printf("PC HAS NOT BEEN UPDATED\n");
  }
  brd->pc += 2;
}

void emu_4000(uint16_t op, chip *brd) {
  uint8_t vx = (op & 0x0f00u) >> 8u;
  uint8_t byte = op & 0x00ffu;
  if (brd->regs[vx] != byte) {
    brd->pc += 2;
    printf("PC HAS BEEN UPDATED\n");
  } else {
    printf("PC HAS NOT BEEN UPDATED\n");
  }
  brd->pc += 2;
}

void emu_5000(uint16_t op, chip *brd) {
  uint8_t vx = (op & 0x0f00u) >> 8u;
  uint8_t vy = (op & 0x00f0u) >> 4u;
  if (brd->regs[vx] == brd->regs[vy]) {
    brd->pc += 2;
    printf("PC HAS BEEN UPDATED\n");
  } else {
    printf("PC HAS NOT BEEN UPDATED\n");
  }
  brd->pc += 2;
}

void emu_6000(uint16_t op, chip *brd) {
  uint8_t vx = (op & 0x0f00u) >> 8u;
  uint8_t byte = op & 0x00ffu;
  brd->regs[vx] = byte;
  printf("The register %d is updated to %x\n", vx, byte);
  brd->pc += 2;
}

void emu_7000(uint16_t op, chip *brd) {
  uint8_t vx = (op & 0x0f00u) >> 8u;
  uint8_t byte = op & 0x00ffu;
  brd->regs[vx] += byte;
  printf("The register %d has been added with %x\n", vx, byte);
  brd->pc += 2;
}

void emu_a000(uint16_t op, chip *brd) {
  uint16_t address = op & 0xfffu;
  brd->index = address;
  printf("Index is updated to %x\n", brd->index);
  brd->pc += 2;
}

void emu_d000(uint16_t op, chip *brd) {
  uint8_t vx = (op & 0x0f00u) >> 8u;
  uint8_t vy = (op & 0x00f0u) >> 4u;
  uint8_t height = op & 0x000fu;
  uint8_t pixel;
  uint8_t xpos = brd->regs[vx] % SCREEN_WIDTH;
  uint8_t ypos = brd->regs[vy] % SCREEN_HEIGHT;

  brd->regs[0xf] = 0;

  for (int yline = 0; yline < height; yline++) {
    pixel = brd->ram[brd->index + yline];
    for (int xline = 0; xline < 8; xline++) {
      if ((pixel & (0x80 >> xline)) != 0) {
        if (brd->display[(xpos + xline + ((ypos + yline) * SCREEN_WIDTH))] ==
            1) {
          brd->regs[0xF] = 1; // Collision detected
        }
        brd->display[xpos + xline + ((ypos + yline) * SCREEN_WIDTH)] ^= 1;
      }
    }
  }
  brd->pc += 2;
}

void decode_and_execute(chip *chip) {
  uint16_t opcode = chip->opcode;

  switch (opcode & 0xf000) {
  case 0x0000:
    emu_0000(opcode, chip);
    break;
  case 0x1000:
    emu_1000(opcode, chip);
    break;
  case 0x2000:
    emu_2000(opcode, chip);
    break;
  case 0x3000:
    emu_3000(opcode, chip);
    break;
  case 0x4000:
    emu_4000(opcode, chip);
    break;
  case 0x5000:
    emu_5000(opcode, chip);
    break;
  case 0x6000:
    emu_6000(opcode, chip);
    break;
  case 0x7000:
    emu_7000(opcode, chip);
    break;
  case 0xa000:
    emu_a000(opcode, chip);
    break;
  case 0xd000:
    emu_d000(opcode, chip);
    break;
  default:
    printf("UNKNOWN OPCODE %x\n", opcode);
    chip->pc += 2;
    break;
  }
}

void display_opcode(chip *brd) {
  for (int i = 0; i < 80; i += 2) {
    uint16_t opcode = brd->ram[0x200 + i] << 8 | brd->ram[(0x200 + i) + 1];
    printf("%d : %x \n", i, opcode);
  }
}

void CustomLog(int msgType, const char *text, va_list args) {
  (void)msgType;
  (void)text;
  (void)args;
}

int counter = 0;

void emulate_cycle(chip *chip) {
  fetch(chip);
  newline;
  printf("%d ---------------------\n", counter);
  printf("current opcode : %x\n", chip->opcode);
  printf("current program counter : %d\n", chip->pc);
  printf("-----------------------\n");
  decode_and_execute(chip);
  counter++;
}

void drawDisplay(chip *brd) {
  BeginDrawing();
  ClearBackground(BLACK);
  for (int y = 0; y < SCREEN_HEIGHT; y++) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      if (brd->display[(y * SCREEN_WIDTH) + x] == 1) {
        DrawRectangle(x * SCALE, y * SCALE, SCALE, SCALE, WHITE);
      }
    }
  }
  EndDrawing();
}

int main(int argc, char *argv[]) {
  printf("This is the CHIP-8 emulator written by umangx\n");

  newline;
  char *file_location = "/Users/synyster7x/projects/chip8/ibm.ch8";

  chip chip;
  init_chip(&chip);
  load_fonts(&chip);
  load_rom(&chip, file_location);

  printf("The data at 552 code : %x \n", chip.ram[552]);

  if (argc > 1) {
    for (int i = 0; i < argc; i++) {
      if (strcmp(argv[i], "-display_opcodes") == 0) {
        display_opcode(&chip);
        for (int i = 0; i < 50; i++)
          emulate_cycle(&chip);
        return 0;
      }
    }
  }

  InitWindow(640, 320, "umangx chip8");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    emulate_cycle(&chip);
    drawDisplay(&chip);
  }

  CloseWindow();
  return 0;
}
