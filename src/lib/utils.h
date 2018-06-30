#ifndef __UTILS_H
#define __UTILS_H

typedef char error_t;
typedef char bool_t;

#define TRUE                        ((bool_t)1)
#define FALSE                       ((bool_t)0)

#define ERR_OK                      0   // ok
#define ERR_FAILURE                 1   // unspecified general failure
#define ERR_OUT_OF_BOUNDS           2   // parameter out of bounds
#define ERR_OUT_OF_MEMORY           3   // cannot allocate more memory

#define offsetof(s, m)              ((unsigned int)(&((s*)0)->m))
#define dimof(x)                    (sizeof(x) / sizeof(x[0]))

#define static_assert(condition) typedef char x##__LINE__[condition ? 1 : -1]

#endif
