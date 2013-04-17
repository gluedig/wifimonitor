rm -f monitor
g++ -g -I /usr/local/include monitor.cpp RadioTapParser.cpp Dot11Parser.cpp ClientDb.cpp -ltins -o monitor
