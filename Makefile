# Makefile to build savetime
# Copyright (c) 2004-2006 Ross Smith II (http://smithii.com). All rights reserved.
# 
# This program is free software; you can redistribute it and/or modify it
# under the terms of version 2 of the GNU General Public License 
# as published by the Free Software Foundation.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# $Id$

VER?=$(shell perl -n -e '/define\s+VER_STRING2\s+"(.*)"/ && print $$1' version.h)
APP?=$(shell perl -n -e '/define\s+VER_INTERNAL_NAME\s+"(.*)"/ && print $$1' version.h)
VC98?=$(PROGRAMFILES)\\Microsoft Visual Studio\\VC98
MSVC_DIR?=$(shell cygpath -u `cygpath -ds "$(VC98)"`)
CYGENV=MAKEFLAGS="" PATH=$(MSVC_DIR)/bin:$$PATH
NSI?=$(shell ls *.nsi)
PREFIX?=$(APP)-$(VER)-win32
EXE?=$(PREFIX).exe
NSHS?=$(shell ls ../*.nsh)
APP_ZIP?=$(APP)-$(VER)-win32.zip
SRC_ZIP?=$(APP)-$(VER)-win32-src.zip
ZIP?=zip
ZIP_OPTS?=-9jquX
CWD=$(shell pwd)
APP_FILES=${EXE} changelog.txt COPYING readme.txt 
SRC_FILES=$(APP_FILES) Makefile $(shell ls *.cpp *.c *.h *.hpp *.ico *.inl *.rc *.dsp *.dsw *.mak *.dep 2>/dev/null)

.PHONY:	dist
dist:	all $(APP_ZIP) $(SRC_ZIP)

.PHONY: installer
installer:	$(EXE)

$(EXE):	$(NSI) $(NSHS) Release/savetime.exe
	makensis /V4 /O$(APP).log /DPRODUCT_VERSION=$(VER) /DPRODUCT_OUTFILE=$@ $<
	chmod a+x *.exe
	
$(APP_ZIP):	$(APP_FILES)
	-rm -f $(APP_ZIP)
	chmod a+x $^
	${ZIP} ${ZIP_OPTS} $@ $^

$(SRC_ZIP):	$(SRC_FILES)
	-rm -f $(SRC_ZIP)
	chmod a+x $^
	${ZIP} ${ZIP_OPTS} $@ $^

.PHONY:	distclean
distclean:	clean
	rm -f $(APP_ZIP) $(SRC_ZIP) ${EXE}

.PHONY:	all clean realclean

include nmake.mak
