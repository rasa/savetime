# savetime

VER?=$(shell perl -n -e '/define\s+VER_STRING2\s+"(.*)"/ && print $$1' version.h)
APP?=$(shell perl -n -e '/define\s+VER_INTERNAL_NAME\s+"(.*)"/ && print $$1' version.h)
APP_FILES=CHANGELOG.md LICENSE README.md $(wildcard Release/*.exe)
SRC_FILES=$(APP_FILES) $(wildcard Makefile *.cpp *.dep *.dsp *.dsw *.h *.ico *.mak *.rc *.sln *.vcproj *.vcxproj *.vcxproj.filters)


APP_ZIP?=$(APP)-$(VER)-win32.zip
SRC_ZIP?=$(APP)-$(VER)-win32-src.zip
ZIP?=zip
ZIP_OPTS?=-9jquX

.PHONY:	dist
dist:	all $(APP_ZIP) $(SRC_ZIP)

$(APP_ZIP):	$(APP_FILES)
	-rm -f $(APP_ZIP)
	chmod +x $^
	${ZIP} ${ZIP_OPTS} $@ $^

$(SRC_ZIP):	$(SRC_FILES)
	-rm -f $(SRC_ZIP)
	chmod +x $^
	${ZIP} ${ZIP_OPTS} $@ $^

.PHONY:	distclean
distclean:	clean
	rm -f $(APP_ZIP) $(SRC_ZIP) ${EXE}

ifneq (,$(shell which MSBuild.exe 2>/dev/null))
include msbuild.mak
else
include nmake.mak
endif

NSI?=$(shell ls *.nsi)
PREFIX?=$(APP)-$(VER)-win32
EXE?=$(PREFIX).exe
NSHS?=$(shell ls ../*.nsh)

.PHONY: installer
installer:	$(EXE)

$(EXE):	$(NSI) $(NSHS) Release/savetime.exe
	makensis /V4 /O$(APP).log /DPRODUCT_VERSION=$(VER) /DPRODUCT_OUTFILE=$@ $<
	chmod +x *.exe
