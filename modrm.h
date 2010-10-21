#ifndef _MODRM_H_
#define _MODRM_H_

template <int32_t constant, size_t bytes>
struct WillFit;

template <int32_t constant>
struct WillFit<constant, 1> {
  enum { value = int32_t(constant) == int8_t(constant) };
};

enum SibScale {
  SibScale_1 = 1,
  SibScale_2 = 2,
  SibScale_4 = 4,
  SibScale_8 = 8
};

/**
 * Inputs:  Register, Base, Index, Scale, Displacement
 * Outputs: Typelist ModRM
 *
 * If displacement is 0 and base is not EBP, then
 *   Append uint8(register << 3 | 4)
 * Else
 *   If displacement is signed byte, then
 *     Append uint8(1 << 6 | register << 3 | 4)
 *   Else
 *     Append uint8(2 << 6 | register << 3 | 4)
 * Append uint8(scale << 6 | index << 3 | base)
 * If displacement isn't 0 or base is EBP, then
 *   If displacement is signed byte, then
 *     Append int8(displacement)
 *   Else
 *     Append int32(displacement)
 */
template <Register reg, int base, Register index, SibScale scale, int displacement>
struct ModRMSIB {
  typedef typename Splice<

    typename If<displacement == 0 && base != EBP,
      typename Imm<reg << 3 | 4, 1>::Type,
      typename If<WillFit<displacement, 1>::value,
        typename Imm<1 << 6 | reg << 3 | 4, 1>::Type,
        typename Imm<2 << 6 | reg << 3 | 4, 1>::Type>::Type>::Type,

    typename Splice<
      typename If<index == ESP,
        typename Imm<index << 3 | base, 1>::Type,
        typename Imm<scale << 6 | index << 3 | base, 1>::Type>::Type,
      typename If<WillFit<displacement, 1>::value,
        typename Imm<displacement, 1>::Type,
        typename Imm<displacement, 4>::Type>::Type>::Type>::Type Type;
};

#endif // _MODRM_H_
