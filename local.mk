# Copyright (c) 2002-2015, Ross Smith II. MIT licensed.

APP_2=savetime

APP_EXE_2:=$(APP_2).exe

RELEASE_APP_EXE_2:=Release/$(APP_EXE_2)

$(RELEASE_APP_EXE_2):
	make -f $(CMAKER) all APP=$(APP_NAME)

CLEAN_FILES+=$(RELEASE_APP_EXE_2)

RELEASE_UPX_EXE_2:=$(dir $(RELEASE_APP_EXE_2))upxed/$(notdir $(RELEASE_APP_EXE_2))

$(eval $(call UPX_FILE,$(RELEASE_APP_EXE_2),$(RELEASE_UPX_EXE_2)))

$(eval $(call SIGN_FILE,$(RELEASE_UPX_EXE_2),$(APP_EXE_2),$(APP_DESC),$(APP_URL)))

CLEAN_FILES+=$(RELEASE_APP_EXE_2)
CLEAN_FILES+=$(RELEASE_UPX_EXE_2)
CLEAN_FILES+=$(APP_EXE_2)

NSIS_FILES+=$(APP_EXE_2)

