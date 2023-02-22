ifeq ($(OS), Windows_NT)
RELEASE=./builds/win/libassemblelib.a
DEBUG=./builds/win/libdassemblelib.a

OBJEXT=.w.o
DEPEXT=.w.d
else
RELEASE=./builds/linux/libassemblelib.a
DEBUG=./builds/linux/libdassemblelib.a

OBJEXT=.l.o
DEPEXT=.l.d
endif

CODEDIRS=src
INCDIRS=include/ libs/include/

OBJDIR=objbuilds
DEPDIR=depfiles

CC=gcc
AR=ar
OPT=
# automatically grab all the headers
CFLAGS=-Wall -Wextra $(foreach D,$(INCDIRS),-I$(D)) $(OPT)

# grab every *.c file in every CODEDIR
CFILES=$(foreach D,$(CODEDIRS),$(wildcard $(D)/*.c))
# regular expression replacement
OBJECTS=$(patsubst %.c,$(OBJDIR)/%$(OBJEXT),$(CFILES))
DEPFILES=$(patsubst %.c,$(DEPDIR)/%$(DEPEXT),$(CFILES))

DEBUGOBJECTS=$(patsubst %.c,$(OBJDIR)/%.d$(OBJEXT),$(CFILES))
DEBUGDEPFILES=$(patsubst %.c,$(OBJDIR)/%.d$(DEPDIR),$(CFILES))

DEPENDENTFLAGS=-MP -MMD -MF

define DEPENDENTFLAGS
	$(DEPENDENTFLAGS) $(patsubst %.c,$(DEPDIR)/%$(2),$(1))
endef

all: $(RELEASE) $(DEBUG)

ifeq ($(OS), Windows_NT)
setup:
		mkdir builds
		mkdir builds\\win
		mkdir depfiles
		mkdir depfiles\\src
		mkdir objbuilds
		mkdir objbuilds\\src
else
setup:
		mkdir -p builds
		mkdir -p builds/linux
		mkdir -p depfiles
		mkdir -p depfiles/src
		mkdir -p objbuilds
		mkdir -p objbuilds/src
endif

$(RELEASE): $(OBJECTS)
		$(AR) rcs $@ $^
		@echo "Success Release Build"

$(OBJDIR)/%$(OBJEXT): %.c
		$(CC) $(CFLAGS) $(call depflags,$<,$(DEPEXT)) -c -o $@ $<

$(DEBUG): $(DEBUGOBJECTS)
		$(AR) rcs $@ $^
		@echo "Success Debug Build"

$(OBJDIR)/%.d$(OBJEXT): %.c
		$(CC) $(CFLAGS) $(call depflags,$<,.d$(DEPEXT)) -DDEBUG -ggdb -c -o $@ $<

ifeq ($(OS), Windows_NT)
clean:
	del $(subst /,\\,$(RELEASE) $(DEBUG) $(OBJECTS) $(DEPFILES) $(DEBUGOBJECTS) $(DEBUGDEPFILES))
else
clean:
	rm -rf $(RELEASE) $(DEBUG) $(OBJECTS) $(DEPFILES) $(DEBUGOBJECTS) $(DEBUGDEPFILES)
endif

-include $(DEPFILES) $(DEBUGDEPFILES)

# non-target files ????
.PHONY: all clean
