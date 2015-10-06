#ifndef __STDDEF_H
#define __STDDEF_H

#ifndef NULL
#define NULL    (void *)0
#endif /* NULL */

typedef long int    ptrdiff_t;
typedef unsigned int size_t;
typedef long long   off_t;
typedef short wchar_t;

#ifndef offsetof
/* Offset of member MEMBER in a struct of type TYPE. */
#ifndef __cplusplus
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#else
/* The cast to "char &" below avoids problems with user-defined
   "operator &", which can appear in a POD type.  */
#define offsetof(TYPE, MEMBER)                  \
  (__offsetof__ (reinterpret_cast <size_t>          \
                 (&reinterpret_cast <const volatile char &> \
                  (static_cast<TYPE *> (0)->MEMBER))))
#endif /* C++ */
#endif /* offsetof */

#endif /* __GCC_STDDEF_H */

