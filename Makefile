# stuff to make
#
ifndef ROOTSYS
all:
	@echo "ROOTSYS is not set. Please set ROOT environment properly"; echo
else

all: 	build
help:
	@echo "Available Targets:";\
	cat Makefile | perl -ne 'printf("\t%-15s %s\n",$$1,$$2) if(/^(\S+):[^#]+(#.*)$$/)'

ifndef VERBOSE
  QUIET := @
endif

CC=g++

INCLUDE=$(shell root-config --cflags) -I./ -I./CORE/ -I./Tools/
CFLAGS=$(INCLUDE)

LINKER=g++
LINKERFLAGS=$(shell root-config --libs) -lEG -lGenVector -lTMVA -L./CORE/ -Wl,-rpath,./CORE/ -lCMS2NtupleMacrosCORE

SOURCES=ElectronLooper.cc Tools/goodrun.cc
OBJECTS=$(SOURCES:.cc=.o)

LIBS=CORE/libCMS2NtupleMacrosCORE.so
EXE=ElectronLooper.exe

.PRECIOUS: $(LIBS) $(OBJECTS)

#
# how to make it
#

CORE/libCMS2NtupleMacrosCORE.so:
	$(MAKE) -C ./CORE

%.exe:  $(LIBS) $(OBJECTS)
	$(QUIET) echo "Building $@";

	$(CC) -o $@ $(LINKERFLAGS) $(LIBS) $(OBJECTS)
%.o: 	%.cc
	$(QUIET) echo "Compiling $<";
	$(CC) $(CFLAGS) $< -c -o $@

build:  $(EXE)

b: build

loopclean:
	rm -f \
	*_out.*	 \
	*.o \
	*.*~ \
	ElectronLooper.exe

clean: loopclean
	$(MAKE) -C ./CORE/ clean

endif
