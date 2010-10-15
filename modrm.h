#ifndef _MODRM_H_
#define _MODRM_H_

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

#endif // _MODRM_H_
