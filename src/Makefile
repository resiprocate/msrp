# This is a quick and dirty Makefile that gets us going
# for the meanwhile. It will need to be updated significantly
# if things get complex in any way. We may ultimately want to
# move to an autotools based system.
#
# Currently, it just collects everything with a ".cpp"
# extension, compiles it, and gloms it together into a
# library. It does basic dependency management.
#
# Note that this makefile requires gcc 3.0.0 or later, and
# relies on gnu make.

RESIPDIR := ../../../resiprocate/main/sip
include Makefile.in

all: common/libcommon.a libmsrp.a test

SRC := $(wildcard *.cpp)
CPPFLAGS += -Wall -g -I. -I$(RESIPDIR)

DEPS := $(patsubst %.cpp, .%.d, $(SRC))

OBJS := $(patsubst %.cpp, %.o, $(SRC))

libmsrp.a: $(OBJS)
	$(AR) r $@ $^

ifneq ($(MAKECMDGOALS),clean)
  -include $(DEPS)
endif

.%.d: %.cpp
	@echo Generating dependencies for $*.o
	@$(CPP) $(CPPFLAGS) -MM $< -MT $*.o -MT .$*.d > $@

.PHONY: clean test
clean:
	$(RM) *.o *.a .*.d
	$(MAKE) -C test clean
	$(MAKE) -C common clean

test:
	$(MAKE) -C test

show.%:
	@echo $*=$($*)

common/libcommon.a: $(wildcard common/*.{hxx,cxx} common/os/*.{hxx,cxx})
	make -C common libcommon.a
