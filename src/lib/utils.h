#ifndef __UTILS_H
#define __UTILS_H

#define offsetof(s, m) ((unsigned int)(&((s*)0)->m))

#define static_assert(condition) typedef char x##__LINE__[condition ? 1 : -1]

#endif
