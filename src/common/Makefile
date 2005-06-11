# This is a quick and dirty Makefile that gets us going
# for the meanwhile. It will need to be updated significantly
# if things get complex in any way. We may ultimately want to
# move to an autotools based system.
#
# Currently, it just collects everything with a ".cxx"
# extension, compiles it, and gloms it together into a
# library. It does basic dependency management.
#
# Note that this makefile requires gcc 3.0.0 or later, and
# relies on gnu make.

RESIPDIR := ../../../../resiprocate/main/sip
include ../Makefile.in

all: libcommon.a

SRC := $(wildcard *.cxx os/*.cxx)
CPPFLAGS += -Wall -g -I.. -I$(RESIPDIR)

DEPS := $(patsubst %.cxx, %.d, $(SRC))

OBJS := $(patsubst %.cxx, %.o, $(SRC))

libcommon.a: $(OBJS)
	$(AR) r $@ $^

ifneq ($(MAKECMDGOALS),clean)
  -include $(DEPS)
endif

%.d: %.cxx
	@echo Generating dependencies for $*.o
	@$(CPP) $(CPPFLAGS) -MM $< -MT $*.o -MT .$*.d > $@

%.o: %.cxx
	$(CXX) $(CPPFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) *.o *.a *.d os/*.o os/*.d

show.%:
	@echo $*=$($*)
