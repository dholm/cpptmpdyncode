#ifndef _TYPELIST_H_
#define _TYPELIST_H_

struct NullType {};

template <class T, class U>
struct Typelist {
  typedef T Head;
  typedef U Tail;
};

template <typename T1 = NullType, typename T2 = NullType, typename T3 = NullType, typename T4 = NullType>
struct MakeTypelist {
 private:
  typedef typename MakeTypelist<T2, T3, T4>::Type TailType;

 public:
  typedef Typelist<T1, TailType> Type;
};

template <>
struct MakeTypelist<> {
  typedef NullType Type;
};

template <class TList> struct Length;
template <> struct Length<NullType> {
  enum { value = 0 };
};

template <class T, class U>
struct Length<Typelist<T, U> > {
  enum { value = 1 + Length<U>::value };
};

template <class TList, int i>
struct TypeAt;

template <class Head, class Tail>
struct TypeAt<Typelist<Head, Tail>, 0> {
  typedef Head Type;
};

template <class Head, class Tail, int i>
struct TypeAt<Typelist<Head, Tail>, i> {
  typedef typename TypeAt<Tail, i - 1>::Type Type;
};

template <class TList, class T>
struct Append;

template <>
struct Append<NullType, NullType> {
  typedef NullType Type;
};

template <class T>
struct Append<NullType, T> {
  typedef Typelist<T, NullType> Type;
};

template <class Head, class Tail>
struct Append<NullType, Typelist<Head, Tail> > {
  typedef Typelist<Head, Tail> Type;
};

template <class Head, class Tail, class T>
struct Append<Typelist<Head, Tail>, T> {
  typedef Typelist<Head, typename Append<Tail, T>::Type> Type;
};

template <class TList1, class TList2>
struct Splice;

template <class TList1>
struct Splice<TList1, NullType> {
  typedef TList1 Type;
};

template <class TList1, class TList2>
struct Splice {
  typedef typename Splice<
      typename Append<TList1, typename TList2::Head>::Type,
      typename TList2::Tail>::Type Type;
};

#endif // _TYPELIST_H_
