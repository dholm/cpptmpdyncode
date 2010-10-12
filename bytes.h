#ifndef _BYTES_H_
#define _BYTES_H_

template <uint8_t byte>
struct Byte {
  enum { value = byte };
};

template <uint32_t value, int bytes>
struct Imm;

template <uint32_t value>
struct Imm<value, 1> {
  typedef typename MakeTypelist<Byte<value & 0xff> >::Type Type;
};

template <uint32_t value, int bytes>
struct Imm {
  typedef typename Splice<
      typename MakeTypelist<Byte<(value >> ((bytes - 1) * 8)) & 0xff> >::Type,
      typename Imm<value, bytes - 1>::Type>::Type Type;
};

#endif // _BYTES_H_
