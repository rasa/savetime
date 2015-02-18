/*

$Id$

Copyright (c) 2005-2008 Ross Smith II (http://smithii.com). All rights reserved.

This program is free software; you can redistribute it and/or modify it
under the terms of version 2 of the GNU General Public License 
as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

*/

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

#include "config.h"

#include <tchar.h>

#include <winsock2.h>  /* include *before* windows */
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>

#include "set_time.h"
#include "wslib.h"

#include "debug.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "user32.lib")

BOOL get_filetime(char *host, FILETIME *ft, char *errmsg) {
	if (!host || !*host) {
		errmsg = "Host name is missing";
		return FALSE;
	}

	errmsg = "";
	int rv = FALSE;
	while (true) {
		int sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_IP /* 0 */);
		if (sock == INVALID_SOCKET) {
			errmsg = wsa_strerror(WSAGetLastError());
			break;
		}

		struct sockaddr_in addr;
		ZeroMemory(&addr, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(IPPORT_TIMESERVER /* 37 */);
		if (!resolve(host, addr.sin_addr)) {
			static char buf[2048];

			errmsg = wsa_strerror(WSAGetLastError());
			snprintf(buf, sizeof(buf), "Failed to resolve '%s': %s", host, rv);
			errmsg = buf;
			break;
		}

		if (::connect(sock, (struct sockaddr*) &addr, sizeof(addr)) == SOCKET_ERROR) {
			errmsg = wsa_strerror(WSAGetLastError());
			break;
		}

		char data[4];
		ZeroMemory(&data, sizeof(data));

		if (::recv(sock, data, sizeof(data), 0) == SOCKET_ERROR) {
			errmsg = wsa_strerror(WSAGetLastError());
			break;
		}

		if (::closesocket(sock) == SOCKET_ERROR) {
			errmsg = wsa_strerror(WSAGetLastError());
			break;
		}

		DWORD dw;		
		memcpy(&dw, data, sizeof(data));

		dw = htonl(dw); // Reverse byte ordering
		dw -= 2208988800ul; // seconds bewteen 1/1/1900 and 1/1/1970

		// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/sysinfo/base/converting_a_time_t_value_to_a_file_time.asp
		ULONGLONG ll = Int32x32To64(dw, 10000000) + 116444736000000000ui64;
		ft->dwLowDateTime = (DWORD) ll;
		ft->dwHighDateTime = (DWORD) (ll >> 32);
		rv = TRUE;
		break;
	} // while (true)
	return rv;
}

BOOL get_localfiletime(char *host, FILETIME *lt, char *errmsg) {
	FILETIME ft;
	errmsg = "";
	if (!get_filetime(host, &ft, errmsg))
		return FALSE;
	if (!FileTimeToLocalFileTime(&ft, lt)) {
		errmsg = windows_strerror(GetLastError());
		return FALSE;
	}
	return TRUE;
}

BOOL get_systemtime(char *host, SYSTEMTIME *st, char *errmsg) {
	FILETIME ft;
	errmsg = "";
	if (!get_filetime(host, &ft, errmsg))
		return FALSE;
	if (!FileTimeToSystemTime(&ft, st)) {
		errmsg = windows_strerror(GetLastError());
		return FALSE;
	}
	return TRUE;
}

#ifdef MAIN

int main(int argc, char *argv[]) {
	char *arg1 = "time.nist.gov";
	if (argc > 1) {
		arg1 = argv[1];
	}
	char *s = wsa_initialize();
	if (s || *s) {
			printf(s);
			return 1;
	}
	char *rv = set_time(arg1);
	printf("set_time(%s) returned '%s'\n", arg1, rv);
	s = wsa_finalize();
	if (s || *s) {
			printf(s);
			return 1;
	}
	return strlen(rv) > 0 ? 1 : 0;
}

#endif
