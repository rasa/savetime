// Copyright (c) 2005-2015 Ross Smith II. See Mit LICENSE in /LICENSE

#ifndef __WSA_STRERROR_H
#define __WSA_STRERROR_H

//#include "config.h"

extern char *windows_strerror (const DWORD err);
extern char *wsa_strerror (int wsaerrno);
extern BOOL resolve(const char* host, struct in_addr& in_addr);
extern char *wsa_initialize();
extern char *wsa_finalize();

#endif /* __WSA_STRERROR_H */
