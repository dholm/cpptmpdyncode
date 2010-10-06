#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

enum Register32 {
  EAX,
  ECX,
  EDX,
  EBX,
  ESP,
  EBP,
  ESI,
  EDI
};

template <Register32 reg>
struct Inc {
  enum { opcode = 0x40 + reg };
};

struct Ret {
  enum { opcode = 0xc3 };
};

template <Register32 reg, int base, int index, int scale, int displacement>
struct ModRMSIB {
};

struct ModRM {
  union {
    struct {
      int mode       : 2;
      int reg_opcode : 3;
      int reg_memory : 3;
    } __attribute__((packed));
    uint8_t byte;
  };
};

typedef int (*call)(int i);

unsigned char inc_code[] = {
  0x8b, 0x44, 0x24, 0x04, /* MOV EAX, [ESP+4] */
  Inc<EAX>::opcode,
  Ret::opcode
};

int main(int argc, char** argv) {
  call inc = reinterpret_cast<call>(malloc(sizeof(inc_code)));
  memcpy(reinterpret_cast<void*>(inc), inc_code, sizeof(inc_code));

  printf("%d = inc(41)\n", inc(41));
  return 0;
}
