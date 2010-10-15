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

template <class ByteList, size_t index>
struct Serialize;

template <size_t index>
struct Serialize<NullType, index> {
  static void Do(uint8_t* destination) { }
};

template <class ByteList, size_t index = 0>
struct Serialize {
  static void Do(uint8_t* destination) {
    destination[index] = ByteList::Head::value;
    Serialize<typename ByteList::Tail, index + 1>::Do(destination);
  }
};

#endif // _BYTES_H_
