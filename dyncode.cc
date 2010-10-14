#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "typelist.h"
#include "bytes.h"

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

template <bool condition, class Then, class Else>
struct If {
  typedef Then Type;
};

template <class Then, class Else>
struct If<false, Then, Else> {
  typedef Else Type;
};

/**
 * Inputs:  Register, Base, Index, Scale, Displacement
 * Outputs: Typelist ModRM
 *
 * If displacement isn't 0 or base is EBP, then
 *   If displacement is signed byte, then
 *     Append int8(displacement)
 *   Else
 *     Append int32(displacement)
 * Append uint8(scale << 6 | index << 3 | base)
 * If displacement is 0 and base is not EBP, then
 *   Append uint8(register << 3 | 4)
 * Else
 *   If displacement is signed byte, then
 *     Append uint8(1 << 6 | register << 3 | 4)
 *   Else
 *     Append uint8(2 << 6 | register << 3 | 4)
 */
template <Register32 reg, int base, int index, int scale, int displacement>
struct ModRMSIB {
  typedef typename Splice<

    typename If<displacement == 0 && base != EBP,
      typename Imm<reg << 3 | 4, 1>::Type,
      typename If<int8_t(displacement) == int32_t(displacement),
        typename Imm<1 << 6 | reg << 3 | 4, 1>::Type,
        typename Imm<2 << 6 | reg << 3 | 4, 1>::Type>::Type>::Type,

    typename Splice<
      typename Imm<scale << 6 | index << 3 | base, 1>::Type,
      typename If<int8_t(displacement) == int32_t(displacement),
        typename Imm<displacement, 1>::Type,
        typename Imm<displacement, 4>::Type>::Type>::Type>::Type Type;
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
