#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <sys/user.h>
#include <sys/mman.h>

#include "conditionals.h"
#include "typelist.h"
#include "bytes.h"

#include "registers.h"
#include "modrm.h"

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
