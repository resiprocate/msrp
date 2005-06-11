CPPFLAGS += -L.. -lmsrp
CPPFLAGS += -I..
CPPFLAGS += -Wall -g

SRC := $(wildcard *.cpp)
TESTS := $(patsubst %.cpp,%,$(SRC))

all: $(TESTS)

$(TESTS): ../libmsrp.a ../common/libcommon.a -lpthread

%: %.cpp
	$(CXX) $(CPPFLAGS) -o $@ $^
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
