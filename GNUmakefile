# UT Video Linux/BSD Makefile
#
# Written by: Derek Buitenhuis <derek.buitenhuis AT gmail.com>
#
# Usage: make
#        make install prefix=/some/dir/here
#        make uninstall prefix=/some/dir/here

DESTDIR=

prefix=/usr/local
libdir=$(DESTDIR)$(prefix)/lib
includedir=$(DESTDIR)$(prefix)/include

CROSS_PREFIX=
CXX=$(CROSS_PREFIX)g++
AR=$(CROSS_PREFIX)ar
RANLIB=$(CROSS_PREFIX)ranlib

UTV_CORE_DIR=utv_core

CXXFLAGS=-g -O2 -Wall -Wextra -Wno-multichar -Wno-unused-parameter

# Pretty-ify Building
ifndef V
$(foreach VAR,CXX AR RANLIB,\
    $(eval override $(VAR) = @printf " %s\t%s\n" $(VAR) "$$@"; $($(VAR))))
endif

OBJ = $(UTV_CORE_DIR)/Codec.o \
      $(UTV_CORE_DIR)/Convert.o \
      $(UTV_CORE_DIR)/DummyCodec.o \
      $(UTV_CORE_DIR)/Format.o \
      $(UTV_CORE_DIR)/FrameBuffer.o \
      $(UTV_CORE_DIR)/GlobalConfig.o \
      $(UTV_CORE_DIR)/HuffmanCode.o \
      $(UTV_CORE_DIR)/Predict.o \
      $(UTV_CORE_DIR)/Thread.o \
      $(UTV_CORE_DIR)/TunedFunc.o \
      $(UTV_CORE_DIR)/UL00Codec.o \
      $(UTV_CORE_DIR)/ULRACodec.o \
      $(UTV_CORE_DIR)/ULRGCodec.o \
      $(UTV_CORE_DIR)/ULY0Codec.o \
      $(UTV_CORE_DIR)/ULY2Codec.o \
      $(UTV_CORE_DIR)/utv_core.o

%.a:
	$(AR) rcu $@ $^
	$(RANLIB) $@

all: static-lib

$(UTV_CORE_DIR)/libutvideo.a: $(OBJ)

static-lib: $(UTV_CORE_DIR)/libutvideo.a

clean:
	@printf " RM\t$(UTV_CORE_DIR)/*.o\n";
	@rm -f $(UTV_CORE_DIR)/*.o
	@printf " RM\t$(UTV_CORE_DIR)/libutvideo.a\n";
	@rm -f $(UTV_CORE_DIR)/libutvideo.a

install: all
	@mkdir -p $(libdir) # in case of custom install dir
	@printf " MKDIR\t\t$(includedir)/utvideo\n";
	@mkdir -p $(includedir)/utvideo
	@printf " INSTALL\t$(includedir)/utvideo/Codec.h\n";
	@cp -f $(UTV_CORE_DIR)/Codec.h $(includedir)/utvideo/Codec.h
	@printf " INSTALL\t$(includedir)/utvideo/utvideo.h\n";
	@cp -f $(UTV_CORE_DIR)/utvideo.h $(includedir)/utvideo/utvideo.h
	@printf " INSTALL\t$(libdir)/libutvideo.a\n";
	@cp -f $(UTV_CORE_DIR)/libutvideo.a $(libdir)/libutvideo.a

uninstall:
	@printf " RM\t$(includedir)/utvideo/*.h\n";
	@rm -f $(includedir)/utvideo/*.h
	@printf " RMDIR\t$(includedir)/utvideo\n";
	@-rmdir $(includedir)/utvideo 2> /dev/null || \
	  if [ -d $(includedir)/utvideo ]; then \
	    printf " NOTE: Not removing $(includedir)/utvideo since it is not empty.\n"; \
	  fi
	@printf " RM\t$(libdir)/libutvideo.a\n";
	@rm -f $(libdir)/libutvideo.a

.PHONY: all static-lib clean install uninstall
