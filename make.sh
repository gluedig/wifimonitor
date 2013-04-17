rm -f monitor
g++ -g -std=c++0x -I /usr/local/include monitor.cpp RadioTapParser.cpp Dot11Parser.cpp ClientDb.cpp -ltins -o monitor
