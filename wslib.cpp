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

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

#include "config.h"
#include <winsock2.h>  /* include *before* windows */
#include <windows.h>
#include <stdio.h>
#include "wslib.h"

char *windows_strerror (const DWORD err) {
	static char buf[2048];

	int ret = FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, err, MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &buf, sizeof (buf), NULL);

	if (ret == 0) {
		snprintf (buf, sizeof(buf), "Unknown Windows error %d", err);
	}

	return buf;
}

BOOL resolve(const char* host, struct in_addr& in_addr) {
	// On Linux, gethostbyname will happily accept a numeric IP address, but
	// on Solaris we need to call 'inet_addr'.
	in_addr_t addr = inet_addr(host);
	if (addr != (in_addr_t)-1) {
		in_addr.s_addr = addr;
		return TRUE;
	}

	struct hostent* ent = gethostbyname(host);
	if (ent == NULL || ent->h_addr_list[0] == NULL) {
		return FALSE;
	}
	in_addr = *(struct in_addr*)ent->h_addr_list[0];
	return TRUE;
}

char *wsa_initialize() {
	WSADATA wsaData;
	ZeroMemory(&wsaData, sizeof(wsaData));
	int i = WSAStartup(MAKEWORD(1, 1), &wsaData);
	if (i != 0) {
		return wsa_strerror(i);
	}

	if ((LOBYTE(wsaData.wVersion) != 1) || (HIBYTE(wsaData.wVersion) != 1)) {
		static char buf[256];
		_snprintf(buf, sizeof(buf), "WSAStartup() returned %d.%d, expected %d.%d",  LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion), 1, 1);
		return buf;
	}

	return "";
}

char *wsa_finalize() {
	int i = WSACleanup();
	if (i != 0) {
		return wsa_strerror(i);
	}
	return "";
}

char *wsa_strerror (int wsaerrno) {
	static char buf[2048];

	switch (wsaerrno) {
	 case 0:
		return "Success";
	 case WSAEACCES:                   /* 10013 */
		return "Permission denied.";
	 case WSAEADDRINUSE:               /* 10048 */
		return "Address already in use.";
	 case WSAEADDRNOTAVAIL:            /* 10049 */
		return "Cannot assign requested address.";
	 case WSAEAFNOSUPPORT:             /* 10047 */
		return "Address family not supported by protocol family.";
	 case WSAEALREADY:                 /* 10037 */
		return "Operation already in progress.";
	 case WSAECONNABORTED:             /* 10053 */
		return "Software caused connection abort.";
	 case WSAECONNREFUSED:             /* 10061 */
		return "Connection refused.";
	 case WSAECONNRESET:               /* 10054 */
		return "Connection reset by peer.";
	 case WSAEDESTADDRREQ:             /* 10039 */
		return "Destination address required.";
	 case WSAEFAULT:                   /* 10014 */
		return "Bad address.";
	 case WSAEHOSTDOWN:                /* 10064 */
		return "Host is down.";
	 case WSAEHOSTUNREACH:             /* 10065 */
		return "No route to host.";
	 case WSAEINPROGRESS:              /* 10036 */
		return "Operation now in progress.";
	 case WSAEINTR:                    /* 10004 */
		return "Interrupted function call.";
	 case WSAEINVAL:                   /* 10022 */
		return "Invalid argument.";
	 case WSAEISCONN:                  /* 10056 */
		return "Socket is already connected.";
	 case WSAEMFILE:                   /* 10024 */
		return "Too many open files.";
	 case WSAEMSGSIZE:                 /* 10040 */
		return "Message too long.";
	 case WSAENETDOWN:                 /* 10050 */
		return "Network is down.";
	 case WSAENETRESET:                /* 10052 */
		return "Network dropped connection on reset.";
	 case WSAENETUNREACH:              /* 10051 */
		return "Network is unreachable.";
	 case WSAENOBUFS:                  /* 10055 */
		return "No buffer space available.";
	 case WSAENOPROTOOPT:              /* 10042 */
		return "Bad protocol option.";
	 case WSAENOTCONN:                 /* 10057 */
		return "Socket is not connected.";
	 case WSAENOTSOCK:                 /* 10038 */
		return "Socket operation on nonsocket.";
	 case WSAEOPNOTSUPP:               /* 10045 */
		return "Operation not supported.";
	 case WSAEPFNOSUPPORT:             /* 10046 */
		return "Protocol family not supported.";
	 case WSAEPROCLIM:                 /* 10067 */
		return "Too many processes.";
	 case WSAEPROTONOSUPPORT:          /* 10043 */
		return "Protocol not supported.";
	 case WSAEPROTOTYPE:               /* 10041 */
		return "Protocol wrong type for socket.";
	 case WSAESHUTDOWN:                /* 10058 */
		return "Cannot send after socket shutdown.";
	 case WSAESOCKTNOSUPPORT:          /* 10044 */
		return "Socket type not supported.";
	 case WSAETIMEDOUT:                /* 10060 */
		return "Connection timed out.";
#if 0
	 case WSATYPE_NOT_FOUND:           /* 10109 */
		return "Class type not found.";
#endif
	 case WSAEWOULDBLOCK:              /* 10035 */
		return "Resource temporarily unavailable.";
	 case WSAHOST_NOT_FOUND:           /* 11001 */
		return "Host not found.";
	 case WSANOTINITIALISED:           /* 10093 */
		return "Successful WSAStartup() not yet performed.";
	 case WSANO_DATA:                  /* 11004 */
		return "Valid name, no data record of requested type.";
	 case WSANO_RECOVERY:              /* 11003 */
		return "This is a nonrecoverable error.";
	 case WSASYSNOTREADY:              /* 10091 */
		return "Network subsystem is unavailable.";
	 case WSATRY_AGAIN:                /* 11002 */
		return "Nonauthoritative host not found.";
	 case WSAVERNOTSUPPORTED:          /* 10092 */
		return "Winsock.dll version out of range.";
	 case WSAEDISCON:                  /* 10101 */
		return "Graceful shutdown in progress.";
#ifdef _WINSOCK2API_

/* cygwin fixes */
# ifndef WSASYSCALLFAILURE
#  define WSASYSCALLFAILURE 10107
# endif
# ifndef WSATYPE_NOT_FOUND
#  define WSATYPE_NOT_FOUND 10109
# endif

#if 0
	 case WSA_INVALID_HANDLE:          /* OS dependent */
		return "Specified event object handle is invalid.";
	 case WSA_INVALID_PARAMETER:       /* OS dependent */
		return "One or more parameters are invalid.";
	 case WSAINVALIDPROCTABLE:         /* OS dependent */
		return "Invalid procedure table from service provider.";
	 case WSAINVALIDPROVIDER:          /* OS dependent */
		return "Invalid service provider version number.";
	 case WSAPROVIDERFAILEDINIT:       /* OS dependent */
		return "Unable to initialize a service provider.";
	 case WSA_IO_INCOMPLETE:           /* OS dependent */
		return "Overlapped I/O event object not in signaled state.";
	 case WSA_IO_PENDING:              /* OS dependent */
		return "Overlapped operations will complete later.";
	 case WSA_NOT_ENOUGH_MEMORY:       /* OS dependent */
		return "Insufficient memory available.";
	 case WSA_OPERATION_ABORTED:       /* OS dependent */
		return "Overlapped operation aborted.";
#endif
	 case WSASYSCALLFAILURE:           /* OS dependent (10107) */
		return "System call failure.";
	 case WSAENOMORE:                  /* 10102 */
		return "WSAENOMORE";
	 case WSAECANCELLED:               /* 10103 */
		return "WSAECANCELLED";
	 case WSAEINVALIDPROCTABLE:        /* 10104 */
		return "WSAEINVALIDPROCTABLE";
	 case WSAEINVALIDPROVIDER:         /* 10105 */
		return "WSAEINVALIDPROVIDER";
	 case WSAEPROVIDERFAILEDINIT:      /* 10106 */
		return "WSAEPROVIDERFAILEDINIT";
	 case WSASERVICE_NOT_FOUND:        /* 10108 */
		return "WSASERVICE_NOT_FOUND";
	 case WSATYPE_NOT_FOUND:           /* 10109 */
		return "WSATYPE_NOT_FOUND";
	 case WSA_E_NO_MORE:               /* 10110 */
		return "WSA_E_NO_MORE";
	 case WSA_E_CANCELLED:             /* 10111 */
		return "WSA_E_CANCELLED";
	 case WSAEREFUSED:                 /* 10112 */
		return "WSAEREFUSED";

#endif /* _WINSOCK2API_ */

	default:
		_snprintf (buf, sizeof (buf), "Unknown winsock error %d.", wsaerrno);
		return buf;
	}
}
