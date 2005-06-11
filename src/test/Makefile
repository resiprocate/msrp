RESIPDIR := ../../../../resiprocate/main/sip
include ../Makefile.in

CPPFLAGS += -I.. -I../.. -I$(RESIPDIR)
CPPFLAGS += -Wall -g
LDFLAGS= -L.. -L../common
LIBS=-lmsrp -lcommon -lpthread

SRC := $(wildcard *.cpp)
TESTS := $(patsubst %.cpp,%,$(SRC))

all: $(TESTS)

$(TESTS): ../libmsrp.a ../common/libcommon.a 

%: %.cpp
	$(CXX) $(LDFLAGS) $(CPPFLAGS) -o $@ $< $(LIBS)
	./$@

DEPS := $(patsubst %.cpp, .%.d, $(SRC))

ifneq ($(MAKECMDGOALS),clean)
#  -include $(DEPS)
endif

.%.d: %.cpp
	@echo Generating dependencies for $*.o
	@$(CPP) $(CFLAGS) -MM $< -MT $*.o -MT .$*.d > $@

.PHONY: clean
clean:
	$(RM) *.o *.a .*.d $(TESTS)
