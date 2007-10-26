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

#ifndef __SET_TIME_H
#define __SET_TIME_H

extern BOOL get_filetime(char *host, FILETIME *ft, char *errmsg);
extern BOOL get_localfiletime(char *host, FILETIME *ft, char *errmsg);
extern BOOL get_systemtime(char *host, SYSTEMTIME *st, char *errmsg);

#endif /* __SET_TIME_H */
