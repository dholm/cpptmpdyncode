#ifndef _CONDITIONALS_H_
#define _CONDITIONALS_H_

template <bool condition, class Then, class Else>
struct If;

template <bool condition, class Then, class Else>
struct If {
  typedef Then Type;
};

template <class Then, class Else>
struct If<false, Then, Else> {
  typedef Else Type;
};

#endif // _CONDITIONALS_H_
