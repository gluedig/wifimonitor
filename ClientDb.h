#ifndef _CLIENTDB_H_
#define _CLIENTDB_H_

#include <map>
#include <set>
#include <string>
#include <iostream>

#include <tins/tins.h>
#include "ClientInfo.h"

struct ClientData {
	ClientData() : age(0), last_rssi(0), avg_rssi(0) {};
	Tins::Dot11::address_type mac;
	int age;

	int last_rssi;
	int avg_rssi;

	std::set<std::string> asked_ssids;
	friend std::ostream& operator<<(std::ostream &os, const ClientData &obj);
};

class ClientDb {
	std::map<Tins::Dot11::address_type, ClientData> db;
	Tins::Dot11::address_type null_address;

public:
	bool newClientEvent(ClientInfo *info);
	friend std::ostream& operator<<(std::ostream &os, const ClientDb &obj);


};
#endif
