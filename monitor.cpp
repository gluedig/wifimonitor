#include <iostream>
#include <exception>
#include <stdio.h>
#include <signal.h>
#include <atomic>
#include <mutex>
#include <pthread.h>

#include <tins/tins.h>


#define CLEANUP_PERIOD 30
#define CLEANUP_MAXAGE 4
#define UPDATE_PERIOD 1

using namespace Tins;

#include "ClientInfo.h"
#include "Parser.h"
#include "RadioTapParser.h"
#include "Dot11Parser.h"
#include "ClientDb.h"
#include "ApDb.h"
#include "ZmqEventSender.h"
#include "WebsocketEventSender.h"
#include "MonitorId.h"

struct cleanup_thread_params {
        ClientDb *clt_db;
        ApDb *ap_db;
        std::atomic_bool cleanup_thread_flag;
} cln_thread_params;

void *cleanup_function(void *param)
{
        struct cleanup_thread_params *params = (struct cleanup_thread_params *)param;

        while (params->cleanup_thread_flag.load()) {
                sleep(CLEANUP_PERIOD);
                params->clt_db->cleanup(CLEANUP_MAXAGE);
                params->ap_db->cleanup(CLEANUP_MAXAGE);
        }
}

struct update_thread_params {
        ClientDb *clt_db;
        std::atomic_bool update_thread_flag;
} upd_thread_params;

void *update_function(void *param)
{
        struct update_thread_params *params = (struct update_thread_params *)param;

        while (params->update_thread_flag.load()) {
                sleep(UPDATE_PERIOD);
                params->clt_db->send_updates();
        }
}


pthread_t cleanup_thread;
pthread_t update_thread;
std::once_flag terminate_flag;

std::vector<Parser *> parsers;
ClientDb *clientdb;
ApDb *apdb;
BaseSniffer *sniffer;
EventSender *sender;

static int count;
static int ignored;

void terminate_function()
{
        std::cerr << "Terminating, waiting for cleanup thread to finish\n";
        cln_thread_params.cleanup_thread_flag.store(false);
        pthread_cancel(cleanup_thread);
        pthread_join(cleanup_thread, NULL);
        std::cerr << "Terminating, waiting for update thread to finish\n";
        upd_thread_params.update_thread_flag.store(false);
        pthread_cancel(update_thread);
        pthread_join(update_thread, NULL);

        for (std::vector<Parser *>::iterator it = parsers.begin() ; it != parsers.end(); ++it) {
                Parser *parser = *it;
                delete parser;
        }

        std::cerr << "Finishing, total packets: " << count << " ignored: " << ignored << std::endl;
        std::cerr << *clientdb << std::endl;
        std::cerr << *apdb << std::endl;
        delete clientdb;
        delete apdb;
        delete sender;
        delete sniffer;
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

        if (!info.interesting) {
                ignored++;
        }


        return true;
}


int main(int argc, char **argv)
{
        int ret;
        sigset_t signal_mask;
        sigemptyset(&signal_mask);
        sigaddset(&signal_mask, SIGINT);
        std::string report_dev;

        if (argc == 5 && !strcmp(argv[1], "-f")) {
                sniffer = new FileSniffer(std::string(argv[2]));
                report_dev = std::string(argv[3]);
                MonitorId::getInstance().setId(atoi(argv[4]));
        } else if (argc == 4) {
                sniffer = new Sniffer(std::string(argv[1]), 1500, true);
                report_dev = std::string(argv[2]);
                MonitorId::getInstance().setId(atoi(argv[3]));
        } else {
                std::cerr << "Usage: " << argv[0] << " <monitor device> <report url> <monitor id>" << std::endl;
                exit(1);
        }

        if (!sniffer)
                exit(1);

        ret = pthread_sigmask (SIG_BLOCK, &signal_mask, NULL);
        if (ret != 0)
                std::cerr << "failed to set sigmask\n";
        
        sender = new ZmqEventSender();
        clientdb = new ClientDb(sender);
        apdb = new ApDb(sender);

        if (!sender->connect(report_dev)) {
                std::cerr << "EventSender failed\n";
                exit(1);
        }

        cln_thread_params.clt_db = clientdb;
        cln_thread_params.ap_db = apdb;
        cln_thread_params.cleanup_thread_flag.store(true);
        ret = pthread_create(&cleanup_thread, NULL, cleanup_function, (void *)&cln_thread_params);
        if (ret)
                std::cerr << "failed to create cleanup thread\n";

        upd_thread_params.clt_db = clientdb;
        upd_thread_params.update_thread_flag.store(true);
        ret = pthread_create(&update_thread, NULL, update_function, (void *)&upd_thread_params);
        if (ret)
                std::cerr << "failed to create update thread\n";


        parsers.push_back(new RadioTapParser());
        parsers.push_back(new Dot11ApParser(apdb));
        parsers.push_back(new Dot11StaParser(clientdb, apdb));

        /* Ctrl-C handler */
        ret = pthread_sigmask (SIG_UNBLOCK, &signal_mask, NULL);
        if (ret != 0)
                std::cerr << "failed to set sigmask\n";

        signal(SIGINT, terminate_handler);

        sniffer->sniff_loop(parse);

        std::cerr << "sniff_loop exited\n";
        terminate_function();
        exit(1);
}
