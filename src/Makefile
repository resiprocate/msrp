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

SRC := $(wildcard *.cpp)
CPPFLAGS += -Wall -g

all: libmsrp.a

libmsrp.a: $(SRC:.cpp=.o)
	$(AR) r $@ $^

DEPS := $(patsubst %.cpp, .%.d, $(SRC))
-include $(DEPS)
.%.d: %.cpp
	@echo Generating dependencies for $*.o
	@$(CPP) $(CFLAGS) -MM $< -MT $*.o -MT .$*.d > $@

.PHONY: clean
clean:
	$(RM) *.o *.a .*.d
