#ifndef _CLIENTDB_H_
#define _CLIENTDB_H_

#include <unordered_map>
#include <set>
#include <string>
#include <iostream>

#include <mutex>
#include <tins/tins.h>

#include "ClientInfo.h"
#include "EventSender.h"

struct ClientData {
        ClientData() : age(0), last_rssi(0), avg_rssi(0), prev_avg_rssi(0) {};
        Tins::Dot11::address_type mac;
        int age;
        time_t first_seen;

        int last_rssi;
        int avg_rssi;
        int prev_avg_rssi;

        std::set<std::string> asked_ssids;
        friend std::ostream &operator<<(std::ostream &os, const ClientData &obj);
};

class ClientDb
{
                std::unordered_map<Tins::Dot11::address_type, ClientData> db;
                Tins::Dot11::address_type null_address;
                std::mutex db_mutex;
                std::mutex updates_mutex;
                int added, removed;
                EventSender *sender;
                std::set<Tins::Dot11::address_type> need_update;
                std::set<Tins::Dot11::address_type> watched;
                std::mutex watched_mutex;
        public:
                bool newClientEvent(ClientInfo *info);
                friend std::ostream &operator<<(std::ostream &os, const ClientDb &obj);
                void cleanup(int maxage);
                void send_updates();
                ClientDb(EventSender *sender);
                void add_watched(std::string mac);
                void remove_watched(std::string mac);
};
#endif
