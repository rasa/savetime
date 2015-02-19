// Copyright (c) 2005-2015 Ross Smith II. See Mit LICENSE in /LICENSE

/*

\todo

get time in a different thread
use port 137 so we can use pool.ntp.org

Start at Lo&gon

disply real time even if time isn't paused:
so ask server the time, when the program first starts, and use it + offset

support multiple time servers:

time-nw.nist.gov;ntp.nasa.gov;canon.inria.fr;swisstime.ethz.ch

A D E F H L O P R S T W U X

		&About...
		&Help...
		View &Log...
		&Force Shutdown
		Shutdo&wn
        Update Real &Time
check	Save on Shut&down
check	Rest&ore Time on Restart
check	Paus&e Time on Restart
		Rese&t Time to Now
		&Restore Time
		&Save Time
		&Un-pause Time
		&Pause Time
		E&xit

read settings from config file and/or registry
log of time changes:

2005-02-10 01:23:45 Time paused
2005-02-10 01:23:45 Time un-paused
2005-02-10 01:23:45 Time saved
2005-02-10 01:23:45 Time restored
2005-02-10 01:23:45 System shutdown
2005-02-10 01:23:45 System startup

dialog box to specify:
	auto-save on shutdown
	auto-pause on startup
	NTP host(s)
	seconds timeout to pause time if no mouse/keyboard activity
	auto-unpause time if mouse/keyboard activity sensed

  Have icon's hands display the time when running, against green background
support unicode
use user defined date/time format
internationalize

*/

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

#include <tchar.h>
#include <windows.h>
#include <stdio.h>		// _vsnprintf
#include <shellapi.h>	// NOTIFYICONDATA
#include <process.h>	// system
#include <stdlib.h>		// _wgetenv
#include <limits.h>		// UINT_MAX

#include <debug.h>

#include "config.h"
#include "getopt.h"
#include "set_time.h"
#include "wslib.h"

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")

#include "version.h"

#define APPNAME			TEXT(VER_INTERNAL_NAME)
#define APPVERSION		TEXT(VER_STRING2)
#define APPCOPYRIGHT	TEXT(VER_LEGAL_COPYRIGHT)
#define APPNAMEVERSION	APPNAME TEXT(" ") APPVERSION

#define REGKEY_RUNONCE "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"
#define REGKEY_SOFTWARE_SAVETIME "SOFTWARE\\Savetime"
#define REGKEY_LOCALTIME "Localtime"

#define WINDOW_CLASS APPNAME

#define ICON_ID_LARGE 100
#define ICON_ID_SMALL 100

#define ICON_GREEN_LARGE 101
#define ICON_GREEN_SMALL 101

#define ICON_PINK_LARGE 102
#define ICON_PINK_SMALL 102

#define ICON_RED_LARGE 103
#define ICON_RED_SMALL 103

#define IDT_TIMER1 1

int tray_icon;

#define NTP_HOST TEXT("utcnist.colorado.edu")

/*

supports port 37:

nist1-ny.WiTime.net	64.90.182.55	New York City, NY	Recommended for new users
time-a.nist.gov	129.6.15.28	NIST, Gaithersburg, Maryland	Ok
time-b.nist.gov	129.6.15.29	NIST, Gaithersburg, Maryland	Ok
nist1-dc.WiTime.net	206.246.118.250	Alexandria, Virginia	Recommended for new users
nist1.aol-va.symmetricom.com	64.236.96.53	Reston, Virginia	Recommended for new users
nist1.columbiacountyga.gov	68.216.79.113	Columbia County, Georgia	Recommended for new users
nist.expertsmi.com	99.150.184.201	Monroe, Michigan	Recommended for new users
nist.netservicesgroup.com	64.113.32.5	Southfield, Michigan	Recommended for new users
time-a.timefreq.bldrdoc.gov	132.163.4.101	NIST, Boulder, Colorado	Ok
time-b.timefreq.bldrdoc.gov	132.163.4.102	NIST, Boulder, Colorado	Very busy
time-c.timefreq.bldrdoc.gov	132.163.4.103	NIST, Boulder, Colorado	Ok
utcnist.colorado.edu	128.138.140.44	University of Colorado, Boulder	ok
utcnist2.colorado.edu	128.138.188.172	University of Colorado, Boulder	Recommended for new users
nist1.aol-ca.symmetricom.com	207.200.81.113	Mountain View, California	Recommended for new users
nist1.symmetricom.com	69.25.96.13	San Jose, California	Recommended for new users
nist1-sj.WiTime.net	64.125.78.85	San Jose, California	Recommended for new users

supports port 37, but returns all zeros:

nist1-la.WiTime.net	64.147.116.229	Los Angeles, California	Recommended for new users

does not support port 37:

time.nist.gov	192.43.244.18	NCAR, Boulder, Colorado	Very busy
time-nw.nist.gov	131.107.13.100	Microsoft, Redmond, Washington	ok

*/

TCHAR time_host[64] = NTP_HOST;

BOOL no_real_time = FALSE;

BOOL restore_time_on_startup = TRUE;

#ifndef SHTDN_REASON_MAJOR_OPERATINGSYSTEM
#define SHTDN_REASON_MAJOR_OPERATINGSYSTEM (0x00020000)
#endif

#ifndef SHTDN_REASON_MINOR_OTHER
#define SHTDN_REASON_MINOR_OTHER (0x00000000)
#endif

#ifndef SHTDN_REASON_FLAG_PLANNED
#define SHTDN_REASON_FLAG_PLANNED (0x80000000)
#endif

static HINSTANCE	app_instance;
static HWND			app_window;
static HMENU		popup_menu;
static SYSTEMTIME	lpPauseTime;

enum {
	MY_WM_TRAY			= WM_USER
};

char* short_options = "afh:nprRstuvw?";

/*check	Save on Shut&down
check	Rest&ore Time on Restart
check	Paus&e Time on Restart
*/

struct option long_options[] = {
  {"about",				0, 0, 'a'},
  {"forceshutdown",		0, 0, 'f'},
  {"help",				0, 0, '?'},
  {"host",				1, 0, 'h'},
  {"nontp",				1, 0, 'n'},
  {"pause",				0, 0, 'p'},
  {"reset",				0, 0, 't'},
  {"restore",			0, 0, 'r'},
  {"norestore",			0, 0, 'R'},
  {"save",				0, 0, 's'},
  {"shutdown",			0, 0, 'w'},
  {"unpause",			0, 0, 'u'},
  {"version",			0, 0, 'v'},
  {NULL,				0, 0, 0}
};

enum {
	MENU_EXIT,
	MENU_PAUSE_TIME,
	MENU_UNPAUSE_TIME,
	MENU_SAVE_TIME,
	MENU_RESTORE_TIME,
	MENU_RESET_TIME,
	MENU_UPDATE_REAL_TIME,
	MENU_SHUTDOWN,
	MENU_FORCE_SHUTDOWN,
	MENU_HELP,
	MENU_ABOUT
};

static void info(const char *format, ...);

int usage() {
//	printf("%s\n", TEXT("(") __DATE__ TEXT(" ") __TIME__ TEXT(")\n\n") COPYRIGHT);

	info(TEXT(
	"Usage: savetime [options]\n"
	"Options:\n\n"
	"           -h\t| --host hostname\tNTP host(default is "
	NTP_HOST
	")\n"
	"           -n\t| --nontp        \tDon't use NTP to get real time\n"
	"           -p\t| --pause        \tPause time on startup\n"
	"           -u\t| --unpause      \tUnpause time on startup\n"
	"           -s\t| --save         \tSave time on startup\n"
	"           -r\t| --restore      \tRestore saved time on startup\n"
	"           -R\t| --norestore    \tDon't restore saved time on startup\n"
	"           -t\t| --reset        \tReset time to real time on startup\n"
	"           -w\t| --shutdown     \tShutdown Windows\n"
	"           -f\t| --forceshutdown\tForce shutdown Windows\n"
	"           -a\t| --about        \tDisplay the about box\n"
	"           -v\t| --version      \tDisplay the program version\n"
	"           -?\t| --help         \tDisplay this help dialog\n"
	));
	return 0;
}

static void info(const char *format, ...) {
	char buf[4096];
	va_list args;

	va_start(args, format);
	_vsnprintf(buf, sizeof(buf) - 1, format, args);
	va_end(args);

	D((_T("info: %s"), buf));

	MessageBox(NULL, buf, APPNAMEVERSION, MB_OK | MB_ICONINFORMATION);
}

static void error(const char *format, ...) {
	char buf[4096];
	va_list args;

	va_start(args, format);
	_vsnprintf(buf, sizeof(buf) - 1, format, args);
	va_end(args);

	D((_T("error: %s"), buf));

	MessageBox(NULL, buf, APPNAMEVERSION, MB_OK | MB_ICONERROR);
}

static void fatal_error(const char *format, ...) {
	va_list args;
	va_start(args, format);
	error(format, args);
	va_end(args);

	ExitProcess(1);
}

static char *last_error(int err) {
	static char windows_error_msg[2048];
	if (FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &windows_error_msg,
		sizeof(windows_error_msg),
		NULL
		) == 0) {
		_snprintf(windows_error_msg, sizeof(windows_error_msg), TEXT("Unknown Windows error %d"), err);
	}
	return windows_error_msg;
}

static void display_last_error(const char *format, ...) {
	int err = GetLastError();

	static char msg[4096];
	msg[0] = '\0';

	if (format && format[0]) {
		static char buf[2048];

		va_list args;

		va_start(args, format);
		_vsnprintf(buf, sizeof(buf) - 1, format, args);
		va_end(args);

		strcpy(msg, buf);
		strcat(msg, TEXT(": "));
	}

	strcat(msg, last_error(err));

	D((_T("display_last_error: msg=%s"), msg));

	MessageBox(NULL, msg, APPNAMEVERSION, MB_OK | MB_ICONERROR);
}

#ifndef MIIM_STRING
#define MIIM_STRING      0x00000040
#endif
#ifndef MIIM_FTYPE
#define MIIM_FTYPE       0x00000100
#endif

static void insert_menu_item(HMENU menu, int id, char *caption) {
	D((_T("insert_menu_item: id=%d, caption=%s"), id, caption));

	MENUITEMINFO item;
	ZeroMemory(&item, sizeof(item));

	item.cbSize = sizeof(item);
//	item.fMask = MIIM_BITMAP | MIIM_ID | MIIM_STATE | MIIM_FTYPE | MIIM_STRING | MIIM_CHECKMARKS;
	item.fMask = MIIM_ID | MIIM_STATE | MIIM_FTYPE | MIIM_STRING;
	item.wID = id;
//	item.fState = MFS_CHECKED;
	item.fType = MFT_STRING;
	item.dwTypeData = caption;
//	item.hbmpChecked = NULL;
//	item.hbmpUnchecked = NULL;
	item.cch = strlen(caption);

	InsertMenuItem(menu, 0, TRUE, &item);
}

static void insert_menu_sep(HMENU menu) {
	D((_T("insert_menu_sep")));

	MENUITEMINFO item;
	ZeroMemory(&item, sizeof(item));

	item.cbSize = sizeof(item);
	item.fMask = MIIM_TYPE;
	item.fType = MFT_SEPARATOR;

	InsertMenuItem(menu, 0, TRUE, &item);
}

static void change_menu_item_caption(HMENU menu, int id, char *caption) {
	D((_T("change_menu_item_caption: id=%d, caption=%s"), id, caption));

	MENUITEMINFO item;
	ZeroMemory(&item, sizeof(item));

	item.cbSize = sizeof(item);
	item.fMask = MIIM_TYPE;
	item.fType = MFT_STRING;
	item.dwTypeData = caption;
	item.cch = strlen(caption);

	SetMenuItemInfo(menu, id, FALSE, &item);
}

static void change_menu_item_state(HMENU menu, int id, int state) {
	MENUITEMINFO item;

	D((_T("change_menu_item_state: id=%d, state=%d"), id, state));

	ZeroMemory(&item, sizeof(item));

	item.cbSize = sizeof(item);
	item.fMask = MIIM_STATE;
	item.fState = state;

	SetMenuItemInfo(menu, id, FALSE, &item);
}

static void add_tray(const char* format, ...) {
	va_list args;

	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));

	nid.cbSize = sizeof(nid);
	nid.hWnd = app_window;
	nid.uID = 0;
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	nid.uCallbackMessage = MY_WM_TRAY;
	nid.hIcon = LoadIcon(app_instance,(LPCSTR) ICON_ID_SMALL);

	va_start(args, format);
	_vsnprintf(nid.szTip, sizeof(nid.szTip) - 1, format, args);
	va_end(args);

	D((_T("add_tray: nid.szTip=%s"), nid.szTip));

	Shell_NotifyIcon(NIM_ADD, &nid);
}

/*
static void modify_tray(int icon, const char* format, ...) {
	va_list args;

	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));

	nid.cbSize = sizeof(nid);
	nid.hWnd = app_window;
	nid.uID = 0;
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	nid.uCallbackMessage = MY_WM_TRAY;
	nid.hIcon = LoadIcon(app_instance,(LPCSTR) icon);

	va_start(args, format);
	_vsnprintf(nid.szTip, sizeof(nid.szTip) - 1, format, args);
	D((_T("modify_tray: nid.szTip=%s"), nid.szTip));
	va_end(args);

	Shell_NotifyIcon(NIM_MODIFY, &nid);
}
*/

static void modify_tray_icon(int icon) {
	D((_T("modify_tray_icon: icon=%d"), icon));

	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));

	nid.cbSize = sizeof(nid);
	nid.hWnd = app_window;
	nid.uID = 0;
	nid.uFlags = NIF_ICON;
	nid.uCallbackMessage = MY_WM_TRAY;
	nid.hIcon = LoadIcon(app_instance,(LPCSTR) icon);

	Shell_NotifyIcon(NIM_MODIFY, &nid);
}

static void modify_tray_tip(const char* format, ...) {
	va_list args;

	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));

	nid.cbSize = sizeof(nid);
	nid.hWnd = app_window;
	nid.uID = 0;
	nid.uFlags = NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = MY_WM_TRAY;

	va_start(args, format);
	_vsnprintf(nid.szTip, sizeof(nid.szTip) - 1, format, args);
	va_end(args);

//	D((_T("modify_tray_tip: nid.szTip=%s"), nid.szTip));

	Shell_NotifyIcon(NIM_MODIFY, &nid);
}

static void remove_tray() {
	D((_T("remove_tray")));

	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));

	nid.cbSize = sizeof(nid);
	nid.hWnd = app_window;
	nid.uID = 0;
	nid.uFlags = 0;

	Shell_NotifyIcon(NIM_DELETE, &nid);
}

static HMENU create_popup_menu() {
	D((_T("create_popup_menu")));

	HMENU menu;
	menu = CreatePopupMenu();

	insert_menu_item(menu, MENU_EXIT,				TEXT("E&xit"));
	insert_menu_item(menu, MENU_PAUSE_TIME,			TEXT("&Pause Time"));
	insert_menu_item(menu, MENU_UNPAUSE_TIME,    	TEXT("&Un-pause Time"));
	insert_menu_item(menu, MENU_SAVE_TIME,			TEXT("&Save Time"));
	insert_menu_item(menu, MENU_RESTORE_TIME,		TEXT("&Restore Time"));
	insert_menu_item(menu, MENU_RESET_TIME,			TEXT("Rese&t Time to Now"));
	insert_menu_item(menu, MENU_UPDATE_REAL_TIME,	TEXT("Update Real &Time"));
	insert_menu_item(menu, MENU_SHUTDOWN,			TEXT("Save && Shutdo&wn"));
	insert_menu_item(menu, MENU_FORCE_SHUTDOWN,		TEXT("Save && &Force Shutdown"));
	insert_menu_item(menu, MENU_HELP,				TEXT("&Help..."));
	insert_menu_item(menu, MENU_ABOUT,				TEXT("&About..."));

	return menu;
}

static void handle_tray_message(UINT message) {
	switch(message) {
	 case WM_LBUTTONDOWN: // let's respond to any button
	 case WM_RBUTTONDOWN:
		 {
			D((_T("handle_tray_message: message=%04x"), message));

			POINT point;
			GetCursorPos(&point);
			SetForegroundWindow(app_window);

			TrackPopupMenuEx(popup_menu, 0, point.x, point.y,
							  app_window, NULL);

			PostMessage(app_window, WM_NULL, 0, 0);

			break;
		 }
	 case WM_MOUSEFIRST:
		 break;
	 default:
		D((_T("handle_tray_message: Unexpected tray message %04x"), message));
	}
}

static BOOL add_milliseconds(FILETIME *ft, ULONGLONG milliseconds, SYSTEMTIME *rv) {
	ULARGE_INTEGER uli;

	uli.HighPart		= ft->dwHighDateTime;
	uli.LowPart			= ft->dwLowDateTime;
	uli.QuadPart		+= milliseconds * 10000ui64;
	FILETIME t;
	t.dwHighDateTime	= uli.HighPart;
	t.dwLowDateTime		= uli.LowPart;

	return(BOOL) FileTimeToSystemTime(&t, rv);
};

ULARGE_INTEGER freeze_tick_count;
ULARGE_INTEGER last_tick_count;

FILETIME last_local_time;

static int update_real_time() {
	D((_T("update_real_time")));

	char *errmsg = TEXT("");
	int rv = get_localfiletime(time_host, &last_local_time, errmsg);
	D((_T("update_real_time: get_localfiletime() returned %d errmsg=%s"), rv, errmsg));

	if (rv) {
		DWORD tick_count = GetTickCount();

		D((_T("update_real_time: tick_count=%08lx"), tick_count));
		freeze_tick_count.QuadPart	=(ULONGLONG) tick_count;
		last_tick_count.QuadPart	=(ULONGLONG) tick_count;
	}
	return rv;
}

static int update_tooltip() {
//	D((_T("update_tooltip")));

	static char sz[256];
	SYSTEMTIME lpLocalTime;
	int icon;

	if (lpPauseTime.wYear == 0) {
		icon = ICON_GREEN_SMALL;
//		GetLocalTime(&lpLocalTime);
//		last_tick_count.QuadPart = 0ui64;
	} else {
		icon = ICON_RED_SMALL;

#ifdef _DEBUG
		_snprintf(sz, sizeof(sz), "Calling SetLocalTime(lpPauseTime) with %02d/%02d/%02d %02d:%02d:%02d",
			lpPauseTime.wMonth, lpPauseTime.wDay, lpPauseTime.wYear % 100,
			lpPauseTime.wHour, lpPauseTime.wMinute, lpPauseTime.wSecond);
		D((_T(sz)));
#endif

		SetLocalTime(&lpPauseTime);
	}

	if (icon != tray_icon) {
		tray_icon = icon;
		modify_tray_icon(tray_icon);
	}

	ULARGE_INTEGER tick_difference;
	if (last_tick_count.QuadPart == 0ui64) {
		update_real_time();
	} else {
		DWORD tick_count = GetTickCount();
//		D((_T("update_tooltip: tick_count=%08x"), tick_count));
		if (tick_count < last_tick_count.LowPart) {
			++last_tick_count.HighPart;
		}
		last_tick_count.LowPart = tick_count;
	}

	tick_difference.QuadPart = last_tick_count.QuadPart - freeze_tick_count.QuadPart;
	add_milliseconds(&last_local_time, tick_difference.QuadPart, &lpLocalTime);

	_snprintf(sz, sizeof(sz), TEXT("Real time: %02d/%02d/%02d %02d:%02d:%02d"),
		lpLocalTime.wMonth, lpLocalTime.wDay, lpLocalTime.wYear % 100,
		lpLocalTime.wHour, lpLocalTime.wMinute, lpLocalTime.wSecond);
	modify_tray_tip(sz);

	return 0;
}

static int pause_time() {
	D((_T("pause_time")));

	GetLocalTime(&lpPauseTime);
	lpPauseTime.wMilliseconds = 0;
#ifdef _DEBUG
	char sz[256];
	_snprintf(sz, sizeof(sz), "Calling SetLocalTime(lpPauseTime) with %02d/%02d/%02d %02d:%02d:%02d",
		lpPauseTime.wMonth, lpPauseTime.wDay, lpPauseTime.wYear % 100,
		lpPauseTime.wHour, lpPauseTime.wMinute, lpPauseTime.wSecond);
	D((_T(sz)));
#endif
	SetLocalTime(&lpPauseTime);
	update_tooltip();
	change_menu_item_state(popup_menu, MENU_PAUSE_TIME, MFS_DISABLED);
	change_menu_item_state(popup_menu, MENU_UNPAUSE_TIME, MFS_ENABLED);
	return 0;
}

static int unpause_time() {
	D((_T("unpause_time")));

	ZeroMemory(&lpPauseTime, sizeof(lpPauseTime));
	update_tooltip();
	change_menu_item_state(popup_menu, MENU_PAUSE_TIME, MFS_ENABLED);
	change_menu_item_state(popup_menu, MENU_UNPAUSE_TIME, MFS_DISABLED);
	return 0;
}

static int save_time(SYSTEMTIME lpTime) {
	D((_T("save_time")));

	HKEY hk;
	if (lpTime.wYear == 0)
		GetLocalTime(&lpTime);

	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGKEY_SOFTWARE_SAVETIME, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hk, NULL)) {
		display_last_error(TEXT("Can't create key %s"), REGKEY_SOFTWARE_SAVETIME);
		return 1;
	}

	if (RegSetValueEx(hk, REGKEY_LOCALTIME, 0, REG_BINARY,
		(LPBYTE) &lpTime, sizeof(SYSTEMTIME))) {
		display_last_error(TEXT("Can't set key %s\\%s"), REGKEY_RUNONCE, REGKEY_LOCALTIME);
		RegCloseKey(hk);
		return 1;
	}

	RegCloseKey(hk);
	return 0;
}

static int save_local_time() {
	D((_T("save_local_time")));

	SYSTEMTIME lpLocalTime;
	GetLocalTime(&lpLocalTime);
	return save_time(lpLocalTime);
}

static int restore_time() {
	D((_T("restore_time")));

	HKEY hk;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGKEY_SOFTWARE_SAVETIME, 0, KEY_QUERY_VALUE,
		&hk))
		return 1;

	SYSTEMTIME lpLocalTime;
	DWORD dwLen = sizeof(SYSTEMTIME);

	if (RegQueryValueEx(hk, REGKEY_LOCALTIME, NULL, NULL,(LPBYTE) &lpLocalTime, &dwLen)) {
		RegCloseKey(hk);
		return 1;
	}

	RegCloseKey(hk);

	if (lpLocalTime.wYear == 0) {
		return 1;
	}

#ifdef _DEBUG
	char sz[256];
	_snprintf(sz, sizeof(sz), "Calling SetLocalTime(lpLocalTime) with %02d/%02d/%02d %02d:%02d:%02d",
		lpLocalTime.wMonth, lpLocalTime.wDay, lpLocalTime.wYear % 100,
		lpLocalTime.wHour, lpLocalTime.wMinute, lpLocalTime.wSecond);
	D((_T(sz)));
#endif
	SetLocalTime(&lpLocalTime);

	ZeroMemory(&lpLocalTime, sizeof(lpLocalTime));

	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, REGKEY_SOFTWARE_SAVETIME, 0, NULL,
		REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hk, NULL)) {
//		display_last_error(TEXT("Can't create key %s"), REGKEY_SOFTWARE_SAVETIME);
		return 1;
	}

	if (RegSetValueEx(hk, REGKEY_LOCALTIME, 0, REG_BINARY,
		(LPBYTE) &lpLocalTime, sizeof(SYSTEMTIME))) {
		display_last_error(TEXT("Can't set key %s\\%s"), REGKEY_SOFTWARE_SAVETIME, REGKEY_LOCALTIME);
		RegCloseKey(hk);
		return 1;
	}

	RegCloseKey(hk);

	return 0;
}

static int reset_time() {
	D((_T("reset_time")));

	FILETIME ft;
	char *errmsg = TEXT("");
	int rv = get_localfiletime(time_host, &ft, errmsg);

	if (!rv) {
		error(TEXT("%s"), errmsg);
		return 1;
	}

	SYSTEMTIME LocalTime;
	GetLocalTime(&LocalTime);

	SYSTEMTIME NewLocalTime;

#ifdef _DEBUG
	static char s[256];
	sprintf(s, TEXT("The current time is %02d/%02d/%04d %02d:%02d:%02d.%04d local time\n"),
		LocalTime.wMonth,
		LocalTime.wDay,
		LocalTime.wYear,
		LocalTime.wHour,
		LocalTime.wMinute,
		LocalTime.wSecond,
		LocalTime.wMilliseconds);
	OutputDebugString(s);
#endif
	if (!FileTimeToSystemTime(&ft, &NewLocalTime)) {
		errmsg = windows_strerror(GetLastError());
		return 1;
	}

#ifdef _DEBUG
	sprintf(s, TEXT("Setting the time to %02d/%02d/%04d %02d:%02d:%02d.%04d local time\n"),
		NewLocalTime.wMonth,
		NewLocalTime.wDay,
		NewLocalTime.wYear,
		NewLocalTime.wHour,
		NewLocalTime.wMinute,
		NewLocalTime.wSecond,
		NewLocalTime.wMilliseconds);
	OutputDebugString(s);
#endif
	if (
		NewLocalTime.wMilliseconds == LocalTime.wMilliseconds &&
		NewLocalTime.wSecond == LocalTime.wSecond &&
		NewLocalTime.wMinute == LocalTime.wMinute &&
		NewLocalTime.wHour == LocalTime.wHour &&
		NewLocalTime.wDay == LocalTime.wDay &&
		NewLocalTime.wMonth == LocalTime.wMonth &&
		NewLocalTime.wYear == LocalTime.wYear)
		return 0;

#ifdef _DEBUG
	char sz[256];
	_snprintf(sz, sizeof(sz), "Calling SetLocalTime(NewLocalTime) with %02d/%02d/%02d %02d:%02d:%02d",
		NewLocalTime.wMonth, NewLocalTime.wDay, NewLocalTime.wYear % 100,
		NewLocalTime.wHour, NewLocalTime.wMinute, NewLocalTime.wSecond);
	D((_T(sz)));
#endif

	if (!SetLocalTime(&NewLocalTime)) {
		errmsg = windows_strerror(GetLastError());
		return 1;
	}

	SendNotifyMessage(HWND_BROADCAST, WM_TIMECHANGE, 0, 0); // Post nofity message to all windows

	return 0;
}

static int shutdown_windows(int options) {
	D((_T("shutdown_windows: options=%d"), options));

	save_time(lpPauseTime);

	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	ZeroMemory(&tkp, sizeof(tkp));

	// Get a token for this process.
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		display_last_error(TEXT("Unable to shutdown Windows"));
		return 1;
	}

	// Get the LUID for the shutdown privilege.
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
		&tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;  // one privilege to set
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Get the shutdown privilege for this process.
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
		(PTOKEN_PRIVILEGES)NULL, 0);

	if (GetLastError() != ERROR_SUCCESS) {
		display_last_error(TEXT("Unable to shutdown Windows"));
		return 1;
	}

	// Shut down the system
	if (!ExitWindowsEx(EWX_SHUTDOWN | options,
               SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
               SHTDN_REASON_MINOR_OTHER |
               SHTDN_REASON_FLAG_PLANNED)) {
		display_last_error(TEXT("Unable to shutdown Windows"));
		return 1;
	}

	return 0;
}

static int force_shutdown_windows() {
	D((_T("force_shutdown_windows")));

	return shutdown_windows(EWX_FORCE);
}

static void show_about_box()  {
	MessageBox(
		NULL,
		APPNAMEVERSION
		TEXT(" (") __DATE__ TEXT(" ") __TIME__ TEXT(")\n\n") APPCOPYRIGHT,
		APPNAME,
		MB_OK | MB_ICONINFORMATION
	);
}

static int handle_command(int id, int event) {
	D((_T("handle_command: id=%d, event=%d"), id, event));

	switch(id)
	{
	 case MENU_EXIT:			DestroyWindow(app_window);	break;
	 case MENU_PAUSE_TIME:		pause_time();				break;
	 case MENU_UNPAUSE_TIME:	unpause_time();			break;
	 case MENU_SAVE_TIME:		save_local_time();			break;
	 case MENU_RESTORE_TIME:	restore_time();			break;
	 case MENU_RESET_TIME:		reset_time();				break;
	 case MENU_UPDATE_REAL_TIME:update_real_time();        break;
	 case MENU_SHUTDOWN:		shutdown_windows(0);		break;
	 case MENU_FORCE_SHUTDOWN:	force_shutdown_windows();	break;
	 case MENU_HELP:			usage();					break;
	 case MENU_ABOUT:			show_about_box();			break;
	 default:					return FALSE;
	}

	return TRUE;
}

static LRESULT CALLBACK window_proc(HWND window, UINT message,
				     WPARAM wparam, LPARAM lparam) {
	switch(message)
	{
	 case MY_WM_TRAY:
		handle_tray_message(lparam);
		return 0;

	 case WM_COMMAND:
		 if (handle_command(LOWORD(wparam), HIWORD(wparam))) {
		 	return 0;
		 }
		 break;

	 case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	 case WM_TIMER:
		update_tooltip();
		return 0;

	 case WM_ENDSESSION:
		// if (lparam & ENDSESSION_LOGOFF)
		KillTimer(app_window, IDT_TIMER1);
		save_time(lpPauseTime);
		ExitProcess(0);
		return 1;
	}
	 //case WM_QUIT: // ???

	return DefWindowProc(window, message, wparam, lparam);
}

static HWND create_app_window() {
	WNDCLASSEX wcex;

	ZeroMemory(&wcex, sizeof(wcex));

	wcex.cbSize = sizeof(wcex);
	wcex.style = 0;
	wcex.lpfnWndProc =(WNDPROC) window_proc;
	wcex.cbClsExtra	= 0;
	wcex.cbWndExtra	= 0;
	wcex.hInstance = app_instance;
	wcex.hIcon = LoadIcon(app_instance,(LPCSTR) ICON_ID_LARGE);
	wcex.hCursor = NULL;
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = WINDOW_CLASS;
	wcex.hIconSm = LoadIcon(app_instance,(LPCSTR) ICON_ID_SMALL);

	RegisterClassEx(&wcex);

	return CreateWindow(WINDOW_CLASS, APPNAME, 0,
	                     CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL,
	                     NULL, app_instance, NULL);
}

int version() {
	info(TEXT("%s\n"), TEXT("(") __DATE__ TEXT(" ") __TIME__ TEXT(")\n\n") APPCOPYRIGHT);
	return 0;
}

int process_options(int argc, char **argv) {
	int option_index = 0;
	char* arg;

	while(true) {
		int c = getopt_long(argc, argv, short_options, long_options, &option_index);
		if (opterr) {
			usage();
			exit(1);
		}
		if (c == -1) {
			break;
		}
		arg =(char*) optarg;
		switch(c) {
			case 'h':	// host
				strncpy(time_host, arg, sizeof(time_host));
				break;
			case 'n':	// nontp
				no_real_time = TRUE;
				break;
			case 'p': 	// pause
				pause_time();
				break;
			case 'u':	// unpause
				unpause_time();
				break;
			case 's':	// save
				save_local_time();
				break;
			case 'r':	// restore
				restore_time_on_startup = true;
				break;
			case 'R':	// norestore
				restore_time_on_startup = false;
				break;
			case 't':	// reset
				reset_time();
				break;
			case 'w':	// shutdown
				shutdown_windows(0);
				break;
			case 'f':	// forceshutdown
				force_shutdown_windows();
				break;
			case 'a':	// about
				show_about_box();
				break;
			case 'v':	// version
				version();
				break;
			case '?':	// help
				usage();
				exit(0);
				break;
			default:
				usage();
				exit(1);
		}
	}
	return 0;
}

#define TOKEN_SEPARATORS " \t"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance,
                    LPSTR cmd_line, int cmd_show) {
	DSETTIME(0);

	HANDLE mutex;

	// only allow one instance of this application
	mutex = CreateMutex(NULL, TRUE, APPNAME);
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		// TODO popup existing instance?
		return 0;
	} else {
		if (mutex == NULL) {
			display_last_error(TEXT("Can't create mutex"));
			//fatal_error("Can't create mutex");
		}
	}

	wsa_initialize();

	app_instance = instance;
	app_window = create_app_window();
	popup_menu = create_popup_menu();

	change_menu_item_state(popup_menu, MENU_PAUSE_TIME, MFS_ENABLED);
	change_menu_item_state(popup_menu, MENU_UNPAUSE_TIME, MFS_DISABLED);

	add_tray(APPNAME);

	RegisterWindowMessage(APPNAME);

	TCHAR savetime[256] = {0};

	DWORD dw = GetEnvironmentVariable("SAVETIME", savetime, sizeof(savetime));

//strcpy(savetime, "-n -p");

	unsigned int tokenc = 1;
	TCHAR *token = strtok(savetime, TOKEN_SEPARATORS);

	while (token) {
		++tokenc;
		token = strtok(NULL, TOKEN_SEPARATORS);
	}

	TCHAR **tokenv = (TCHAR **) malloc((tokenc + 1) * sizeof(TCHAR *));

	for (unsigned int i = 0; i <= tokenc; ++i) {
		tokenv[i] = NULL;
	}

	dw = GetEnvironmentVariable("SAVETIME", savetime, sizeof(savetime));

//strcpy(savetime, "-n -p");

	tokenc = 1;
	token = strtok(savetime, TOKEN_SEPARATORS);

	while (token) {
		tokenv[tokenc++] = token;
		token = strtok(NULL, TOKEN_SEPARATORS);
	}

	process_options(tokenc, tokenv);

	process_options(__argc, __argv);

	if (restore_time_on_startup) {
		restore_time();
	}

	// fire every 990 milliseconds
	// \q will this work on all systems?
	SetTimer(app_window, IDT_TIMER1, 990,(TIMERPROC) NULL);

    update_tooltip();

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		Sleep(0);
	}

	DestroyMenu(popup_menu);

	remove_tray();

	ReleaseMutex(mutex);

	wsa_finalize();

	return msg.wParam;
}
