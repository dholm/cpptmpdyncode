#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <sys/user.h>
#include <sys/mman.h>

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
  typedef typename Imm<0x40 + reg, 1>::Type Type;
};

template <class _MovRMSIB>
struct MovRM32ToR32 {
  typedef typename Splice<Imm<0x8b, 1>::Type, _MovRMSIB>::Type Type;
};

struct Ret {
  typedef Imm<0xc3, 1>::Type Type;
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

typedef int (*Call)(int i);

int main(int argc, char** argv) {
  typedef Splice<
      Splice<

      MovRM32ToR32<ModRMSIB<EAX, ESP, 4, 0, 4>::Type>::Type,
      Inc<EAX>::Type>::Type,

      Ret::Type>::Type Fn;

  void *mem = malloc(Length<Fn>::value + PAGE_SIZE);
  Call inc = reinterpret_cast<Call>(
      reinterpret_cast<off_t>(mem) + (PAGE_SIZE - 1) & PAGE_MASK);
  if (mprotect(
          reinterpret_cast<void*>(inc),
          Length<Fn>::value,
          PROT_READ | PROT_WRITE | PROT_EXEC) < 0) {
    perror("Memory protection failed");
    return -1;
  }
  Serialize<Fn>::Do(reinterpret_cast<uint8_t*>(inc));

  printf("%d = inc(41)\n", inc(41));
  free(mem);

  return 0;
}
