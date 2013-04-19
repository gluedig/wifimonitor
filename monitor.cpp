#include <iostream>
#include <exception>
#include <stdio.h>
#include <signal.h>
#include <thread>
#include <chrono>
#include <atomic>

#include <tins/tins.h>

#define CLEANUP_PERIOD 30
#define CLEANUP_MAXAGE 4


using namespace Tins;

#include "ClientInfo.h"
#include "Parser.h"
#include "RadioTapParser.h"
#include "Dot11Parser.h"
#include "ClientDb.h"
#include "ZmqEventSender.h"

struct CleanupFunction {
                CleanupFunction() : is_terminated(false) {};
                void cleanup_function(ClientDb *db) {
                        while (!is_terminated) {
                                std::this_thread::sleep_for(std::chrono::seconds(CLEANUP_PERIOD));
                                db->cleanup(CLEANUP_MAXAGE);
                        }
                }
                void terminate() {
                        is_terminated = true;
                }
        private:
                std::atomic_bool is_terminated;
};


CleanupFunction *cf;
std::thread *cleanup_thread;
std::once_flag terminate_flag;

std::vector<Parser *> parsers;
ClientDb *clientdb;

ZmqEventSender *sender;

static int count;
static int ignored;

void terminate_function()
{
        cf->terminate();
        delete cf;

        for (std::vector<Parser *>::iterator it = parsers.begin() ; it != parsers.end(); ++it) {
                Parser *parser = *it;
                delete parser;
        }

        std::cerr << "Finishing, total packets: " << count << " ignored: " << ignored << std::endl;
        std::cerr << *clientdb << std::endl;
        delete clientdb;
        delete sender;
}
void terminate_handler(int sig)
{
        std::call_once(terminate_flag, terminate_function);
        /* re-raise */
        signal(sig, SIG_DFL);
        raise(sig);
}

bool parse(const RefPacket &ref)
{
        ClientInfo info;

        count++;
        info.timestamp = ref.timestamp();
        for (std::vector<Parser *>::iterator it = parsers.begin() ; it != parsers.end(); ++it) {
                if (!(*it)->parse(&info, ref.pdu()))
                        break;
        }

        if (info.interesting) {
                clientdb->newClientEvent(&info);
        } else {
                ignored++;
        }

        return true;
}


int main(int argc, char **argv)
{
        char *monitor_dev;

        if (argc != 2)
                exit(1);
        monitor_dev = argv[1];

        sender = new ZmqEventSender();
        if (!sender->bind("tcp://127.0.0.1:5555"))
                exit(1);

        clientdb = new ClientDb(sender);

        cf = new CleanupFunction();
        cleanup_thread = new std::thread(std::bind(&CleanupFunction::cleanup_function, std::ref(cf), clientdb));

        parsers.push_back(new RadioTapParser());
        parsers.push_back(new Dot11Parser());

        /* Ctrl-C handler */
        signal(SIGINT, terminate_handler);


        Sniffer sniffer(std::string(monitor_dev), 1500, true);
        sniffer.sniff_loop(parse);

        std::cerr << "sniff_loop exited\n";
        terminate_function();
        exit(1);
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
