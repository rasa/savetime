/*

$Id$

Copyright (c) 2005-2006 Ross Smith II (http://smithii.com). All rights reserved.

This program is free software; you can redistribute it and/or modify it
under the terms of version 2 of the GNU General Public License 
as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

*/

#ifndef __WSA_STRERROR_H
#define __WSA_STRERROR_H

//#include "config.h"

extern char *windows_strerror (const DWORD err);
extern char *wsa_strerror (int wsaerrno);
extern BOOL resolve(const char* host, struct in_addr& in_addr);
extern char *wsa_initialize();
extern char *wsa_finalize();

#endif /* __WSA_STRERROR_H */
