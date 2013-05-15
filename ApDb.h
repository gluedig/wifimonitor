#ifndef _APDB_H_
#define _APDB_H_

#include <unordered_map>
#include <string>
#include <iostream>

#include <mutex>
#include <tins/tins.h>

#include "ClientInfo.h"
#include "EventSender.h"

struct ApData {
        ApData() : age(0), rssi(0), channel(0) {};
        Tins::Dot11::address_type mac;
        int age;
        time_t first_seen;

        int rssi;
        int channel;

        std::string ssid;
        friend std::ostream &operator<<(std::ostream &os, const ApData &obj);
};

class ApDb
{
                std::unordered_map<Tins::Dot11::address_type, ApData> db;
                Tins::Dot11::address_type null_address;
                std::mutex db_mutex;
                int added, removed;
                EventSender *sender;
        public:
                bool newApEvent(ClientInfo *info);
                friend std::ostream &operator<<(std::ostream &os, const ApDb &obj);
                void cleanup(int maxage);
                ApDb(EventSender *sender);
                bool inDb(Tins::Dot11::address_type mac);
};
#endif
