# Frenda Makefile for Dreamcast/KOS

# Project properties
PROJECT =	libfrenda
TIMEZONE =	CET-1CEST

# Code paths & files
SRCPATH =	source
INCPATH =	$(SRCPATH)/include
SRCS :=		$(shell find $(SRCPATH) -name '*.cpp' ! -name "version.cpp")
OBJS =		$(SRCS:.cpp=.o)
VERFILE =	$(SRCPATH)/Version
OBJS +=		$(VERFILE).o

# Additional compiler flags
CFLAGS +=	-Wno-reorder

# Targets
TARGETELF = $(PROJECT).a

.PHONY all: version
all: rm-elf $(TARGETELF)

include $(KOS_BASE)/Makefile.prefab

clean: rm-elf

version:
	-rm -f $(VERFILE).cpp
	@echo 'const char * FRENDA_BUILD_DATE = "$(shell export TZ=$(TIMEZONE) && date +'%a %b %d %Y')";' >> $(VERFILE).cpp;
	@echo 'const char * FRENDA_BUILD_TIME = "$(shell export TZ=$(TIMEZONE) && date +'%H:%M:%S %Z')";' >> $(VERFILE).cpp;
	@echo 'const char * FRENDA_BUILD_CREATOR = "$(shell id -un)@$(shell hostname)";' >> $(VERFILE).cpp;

rm-elf:
	-rm -f $(TARGETELF)

$(TARGETELF): $(OBJS)
	$(KOS_AR) rcs $(TARGETELF) $(OBJS)

dist:
	@rm -f $(OBJS)
	@$(KOS_STRIP) $(TARGETELF)
