CC?=gcc
CXX?=g++
RM=rm -f
INCLUDE_DIR?=/usr/local/include
CPPFLAGS=-g -std=c++0x -I $(INCLUDE_DIR)
LDFLAGS=-g
LDLIBS=-ltins -lpthread -ljansson -lczmq -lzmq

SRCS = $(wildcard ./*.cpp)
OBJS=$(subst .cpp,.o,$(SRCS))

all: monitor

monitor: $(OBJS)
	$(CXX) $(LDFLAGS) -o monitor $(OBJS) $(LDLIBS) 

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) *~ .dependtool

include .depend
