ifeq ($(OS), Windows_NT)
RELEASE=./builds/win/fcasm.exe
DEBUG=./builds/win/fcasm-d.exe

OBJEXT=.w.o
DEPEXT=.w.d

ASSEMBLELIBDIR=libs/parsing/builds/win/
ASSEMBLELIB=libs/parsing/builds/win/libassemblelib.a
DEBUGASSEMBLELIB=libs/parsing/builds/win/libdassemblelib.a

PARSELIB=libs/libwparselib.a
LNPARSE=wparselib
DEBUGPARSELIB=libs/libwdparselib.a
LNDPARSE=wdparselib

SPECIFICPREFIX=w
else
RELEASE=./builds/linux/fcasm
DEBUG=./builds/linux/fcasm-d

OBJEXT=.l.o
DEPEXT=.l.d

ASSEMBLELIBDIR=libs/parsing/builds/linux/
ASSEMBLELIB=libs/parsing/builds/linux/libassemblelib.a
DEBUGASSEMBLELIB=libs/parsing/builds/linux/libdassemblelib.a

PARSELIB=libs/liblparselib.a
LNPARSE=lparselib
DEBUGPARSELIB=libs/libldparselib.a
LNDPARSE=ldparselib

SPECIFICPREFIX=l
endif

LNASSEMBLE=assemblelib
LNDASSEMBLE=dassemblelib

PARSINGDIR=libs/parsing

CODEDIRS=libs src
INCDIRS=include/ libs/include/

OBJDIR=objbuilds
DEPDIR=depfiles

# grab every *.c file in every CODEDIR
CFILES=$(foreach D,$(CODEDIRS),$(wildcard $(D)/*.c))

SPECIFIC=src/specific
SPECIFICHEADERS=$(wildcard $(SPECIFIC)/include/*.h)


SPECIFICOBJECTS=$(patsubst $(SPECIFIC)/include/%.h,$(OBJDIR)/$(SPECIFIC)/$(SPECIFICPREFIX)%$(OBJEXT),$(SPECIFICHEADERS))
SPECIFICDEPFILES=$(patsubst $(SPECIFIC)/include/%.h,$(DEPDIR)/$(SPECIFIC)/%$(DEPEXT),$(SPECIFICHEADERS))

DEBUGSPECIFICOBJECTS=$(patsubst $(SPECIFIC)/include/%.h,$(OBJDIR)/$(SPECIFIC)/$(SPECIFICPREFIX)%.d$(OBJEXT),$(SPECIFICHEADERS))
DEBUGSPECIFICDEPFILES=$(patsubst $(SPECIFIC)/include/%.h,$(DEPDIR)/$(SPECIFIC)/%.d$(DEPEXT),$(SPECIFICHEADERS))

CC=gcc
OPT=
# automatically grab all the headers
CFLAGS=-Wall -Wextra $(foreach D,$(INCDIRS) $(SPECIFIC)/include,-I$(D)) $(OPT)


# regular expression replacement
OBJECTS=$(patsubst %.c,$(OBJDIR)/%$(OBJEXT),$(CFILES)) $(SPECIFICOBJECTS)
DEPFILES=$(patsubst %.c,$(DEPDIR)/%$(DEPEXT),$(CFILES)) $(SPECIFICDEPFILES)

DEBUGOBJECTS=$(patsubst %.c,$(OBJDIR)/%.d$(OBJEXT),$(CFILES)) $(DEBUGSPECIFICOBJECTS)
DEBUGDEPFILES=$(patsubst %.c,$(DEPDIR)/%.d$(DEPEXT),$(CFILES)) $(DEBUGSPECIFICDEPFILES)

DEPENDENTFLAGS=-MP -MMD -MF

define depflags
	$(DEPENDENTFLAGS) $(patsubst %.c,$(DEPDIR)/%$(2),$(1))
endef

define libflags
	$(foreach L,$(1),-L$(L))
endef

define clibs
	$(foreach Libfile,$(1),-l$(Libfile))
endef

release: $(RELEASE)

debug: $(DEBUG)

ifeq ($(OS), Windows_NT)
setup:
		mkdir builds
		mkdir builds\\win
		mkdir depfiles
		mkdir depfiles\\libs
		mkdir depfiles\\src
		mkdir depfiles\\src\\specific
		mkdir objbuilds
		mkdir objbuilds\\src
		mkdir objbuilds\\src\\specific
		mkdir objbuilds\\libs
else
setup:
		mkdir -p builds
		mkdir -p builds/linux
		mkdir -p depfiles
		mkdir -p depfiles/libs
		mkdir -p depfiles/src
		mkdir -p depfiles/src/specific
		mkdir -p objbuilds
		mkdir -p objbuilds/libs
		mkdir -p objbuilds/src
		mkdir -p objbuilds/src/specific
endif

# release build
$(RELEASE): $(OBJECTS) $(ASSEMBLELIB)
	$(CC) $(CFLAGS) -o $(RELEASE) $(OBJECTS) $(call libflags,libs $(ASSEMBLELIBDIR)) $(call clibs,$(LNPARSE) $(LNASSEMBLE)) -lm
	@echo "Success Release Build"

# debug build
$(DEBUG): $(DEBUGOBJECTS) $(DEBUGASSEMBLELIB)
	$(CC) $(CFLAGS) -o $(DEBUG) $(DEBUGOBJECTS) $(call libflags,libs $(ASSEMBLELIBDIR)) $(call clibs,$(LNDPARSE) $(LNDASSEMBLE)) -lm
	@echo "Success Debug Build"

# test build
test: $(DEBUGOBJECTS) $(DEBUGASSEMBLELIB)
	$(CC) $(CFLAGS) -o $(ofile) $(tfile) $(DEBUGOBJECTS) $(call libflags,libs $(ASSEMBLELIBDIR)) $(call clibs,$(LNDPARSE) $(LNDASSEMBLE)) -lm

# release objects
$(OBJDIR)/%$(OBJEXT): %.c
	$(CC) $(CFLAGS) $(call depflags,$<,$(DEPEXT)) -c -o $@ $<

# debug objects
$(OBJDIR)/%.d$(OBJEXT): %.c
	$(CC) $(CFLAGS) $(call depflags,$<,$(DEPEXT)) -c -o $@ $<

# release assemblelib
$(ASSEMBLELIB):
	$(MAKE) -C $(PARSINGDIR)

# debug assemblelib
$(DEBUGASSEMBLELIB):
	$(MAKE) -c $(PARSINGDIR)

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
