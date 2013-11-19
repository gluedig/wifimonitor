CC?=gcc
CXX?=g++
RM=rm -f
INCLUDE_DIR?=/usr/local/include
LIB_DIR?=/usr/local/lib
CPPFLAGS=-g -std=c++0x -I $(INCLUDE_DIR) -D_WEBSOCKETPP_CPP11_STL_ -D_WEBSOCKETPP_NO_CPP11_REGEX -I /usr/include
LDFLAGS=-g -L $(LIB_DIR) -L /usr/lib
LDLIBS=-lpcap -ltins -lpthread -ljansson -lczmq -lzmq -lboost_system

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
