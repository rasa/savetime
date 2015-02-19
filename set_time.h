// Copyright (c) 2005-2015 Ross Smith II. See Mit LICENSE in /LICENSE

#ifndef __SET_TIME_H
#define __SET_TIME_H

extern BOOL get_filetime(char *host, FILETIME *ft, char *errmsg);
extern BOOL get_localfiletime(char *host, FILETIME *ft, char *errmsg);
extern BOOL get_systemtime(char *host, SYSTEMTIME *st, char *errmsg);

#endif /* __SET_TIME_H */
