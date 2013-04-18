rm -f monitor
g++ -g -std=c++0x -I /usr/local/include monitor.cpp RadioTapParser.cpp Dot11Parser.cpp ClientDb.cpp EventMessage.cpp -ltins -lpthread -ljansson -o monitor
