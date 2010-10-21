#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include <sys/user.h>
#include <sys/mman.h>

#include "conditionals.h"
#include "typelist.h"
#include "bytes.h"

#include "registers.h"
#include "modrm.h"

template <Register reg>
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

class AlignedPointer {
 public:
  AlignedPointer() :
      m_pointer(NULL),
      m_aligned_pointer(NULL) {
  }

  ~AlignedPointer() {
    m_aligned_pointer = NULL;
    free(m_pointer);
  }

  int init(size_t size, off_t alignment) {
    m_pointer = malloc(size + (alignment - 1));
    if (!m_pointer) {
      return -ENOMEM;
    }

    m_aligned_pointer = reinterpret_cast<void*>(
        (reinterpret_cast<off_t>(m_pointer) + (alignment - 1)) &
        ~(alignment - 1));

    return 0;
  }

  void* operator()() {
    return m_aligned_pointer;
  }

  const void* operator()() const {
    return m_aligned_pointer;
  }

 private:
  void* m_pointer;
  void* m_aligned_pointer;
};

template <typename ...Statements>
struct CodeBlock;

template <typename Statement, typename... Tail>
struct CodeBlock<Statement, Tail...> {
  typedef typename Splice<
    Statement, typename CodeBlock<Tail...>::Type >::Type Type;
};

template <>
struct CodeBlock<> {
  typedef NullType Type;
};

int main(int argc, char** argv) {
  typedef CodeBlock<
      MovRM32ToR32<ModRMSIB<EAX, ESP, ESP, SibScale_1, 4>::Type>::Type,
      Inc<EAX>::Type,
      Ret::Type>::Type Fn;

  AlignedPointer inc_mem;
  if (inc_mem.init(Length<Fn>::value, PAGE_SIZE) < 0) {
    fprintf(stderr, "Unable to allocate memory!\n");
    return -1;
  }
  Call inc = reinterpret_cast<Call>(inc_mem());
  if (mprotect(
          inc_mem(),
          Length<Fn>::value,
          PROT_READ | PROT_WRITE | PROT_EXEC) < 0) {
    perror("Memory protection failed");
    return -1;
  }
  Serialize<Fn>::Do(reinterpret_cast<uint8_t*>(inc));

  printf("%d = inc(41)\n", inc(41));

  return 0;
}
