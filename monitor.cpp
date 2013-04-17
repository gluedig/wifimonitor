#include <iostream>
#include <exception>
#include <stdio.h>
#include <signal.h>
#include <thread>
#include <chrono>

#include <tins/tins.h>

#define CLEANUP_PERIOD 30
#define CLEANUP_MAXAGE 4


using namespace Tins;

#include "ClientInfo.h"
#include "Parser.h"
#include "RadioTapParser.h"
#include "Dot11Parser.h"
#include "ClientDb.h"

std::thread cleanup_thread;
std::vector<Parser*> parsers;
ClientDb clientdb;

static int count;
static int ignored;

void terminate_handler(int sig)
{
	
	for (std::vector<Parser*>::iterator it = parsers.begin() ; it != parsers.end(); ++it) {
		Parser *parser = *it;
		delete parser;
	}

	std::cerr << "Finishing, total packets: " << count << " ignored: " << ignored << std::endl;
	std::cerr << clientdb << std::endl;

	/* re-raise */
        signal(sig, SIG_DFL);
        raise(sig);

}

bool parse(const RefPacket &ref) {
	ClientInfo info;

	count++;
	info.timestamp = ref.timestamp();
	for (std::vector<Parser*>::iterator it = parsers.begin() ; it != parsers.end(); ++it) {
		if (!(*it)->parse(&info, ref.pdu())) 
			break;
	}
	
	if (info.interesting) {
//		std::cout << info;
		clientdb.newClientEvent(&info);
	}
	else {
//		std::cout << "Ignored: " << info;
		ignored++;

	}


	return true;
}

void cleanup_function(ClientDb *db)
{
	while (1) {
		std::this_thread::sleep_for(std::chrono::seconds(CLEANUP_PERIOD));
		db->cleanup(CLEANUP_MAXAGE);
	}
}


int main(int argc, char **argv)
{
	char * monitor_dev;

	if (argc != 2)
		exit(1);
	monitor_dev = argv[1];
	
	 /* Ctrl-C handler */
        signal(SIGINT, terminate_handler);

	cleanup_thread = std::thread(cleanup_function, &clientdb);
	parsers.push_back(new RadioTapParser());
	parsers.push_back(new Dot11Parser());
	
	Sniffer sniffer(std::string(monitor_dev), 1500, true);
	sniffer.sniff_loop(parse);


/*
	FileSniffer sniffer("/home/developer/cap1.pcap");
	while (count++ < 100000) {
		std::cout << "packet: " << count << std::endl;
		PDU *pdu = sniffer.next_packet();
		PDU::PDUType type = pdu->pdu_type();
		std::cout << "type: " << type << std::endl;
	}
*/

}
