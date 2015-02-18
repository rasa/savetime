#ifndef __CONFIG_H
#define __CONFIG_H

/* MSVC defines u_long in winsock.h */
#define in_addr_t u_long

#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN 1
#endif

#ifdef _MSC_VER
//#if _MSC_VER < 1500
# ifndef my_vsnprintf
#  define my_vsnprintf _vsnprintf
# endif
# ifndef my_snprintf
#  define my_snprintf  _snprintf
# endif
//#endif // _MSC_VER < 1500
#endif // _MSC_VER

#endif /* __CONFIG_H */
