#include <time.h>
#include "ClientDb.h"

#define AVG_COUNT 8
#define RSSI_HIST 1

bool ClientDb::newClientEvent(ClientInfo *info)
{	
	if (info->mac == null_address || info->mac == Tins::Dot11::BROADCAST)
		return false;

	db_mutex.lock();

	if (db.count(info->mac)) {
		ClientData data = db[info->mac];
		data.age = 0;

		/* do RSSI averaging */
		int prev_rssi = data.last_rssi;
		data.last_rssi = info->rssi;
		int prev_avg = data.avg_rssi;
		data.avg_rssi = prev_avg - prev_rssi/AVG_COUNT + info->rssi/AVG_COUNT;

		if (info->asked_SSID.size() && !data.asked_ssids.count(info->asked_SSID))
			data.asked_ssids.insert(info->asked_SSID);

		/* trigger if average RSSI changed */
		if ((data.avg_rssi != prev_avg) &&
			(data.avg_rssi > prev_avg+RSSI_HIST) || (data.avg_rssi < prev_avg-RSSI_HIST)) {
			std::cout << "RSSI changed Prev RSSI: " << prev_avg << " " << data << std::endl;

		}

		db[info->mac] = data;
	} else {
		ClientData new_data;
		new_data.mac = info->mac;
		new_data.last_rssi = (int)info->rssi;
		new_data.avg_rssi = (int)info->rssi;
		new_data.first_seen = info->timestamp.seconds();
		if (info->asked_SSID.size())
			new_data.asked_ssids.insert(info->asked_SSID);

		std::cout << "New " << new_data << std::endl;
		added++;
		db.insert(std::pair<Tins::Dot11::address_type, ClientData>(info->mac, new_data));
	}	
	db_mutex.unlock();

	return true;
}

void ClientDb::cleanup(int maxage)
{
	db_mutex.lock();
	auto it = db.begin();
	while (it != db.end()) {
		it->second.age++;
		if (it->second.age > maxage) {
			std::cout << "Removing " << it->second << std::endl;
			removed++;
			db.erase(it++);
		} else {
			++it;
		}
	}

	db_mutex.unlock();
}

ClientDb::ClientDb() : added(0), removed(0)
{

}

std::ostream& operator<<(std::ostream &os, const ClientDb &obj)
{
	const std::map<Tins::Dot11::address_type, ClientData> *db = &(obj.db);
	std::map<Tins::Dot11::address_type, ClientData>::const_iterator it;

	os << "ClientDb dump, elements: " << db->size() << " added: " << obj.added << " removed: " << obj.removed << std::endl;
	for ( it = db->begin(); it != db->end(); ++it)
	{
		os << it->second << std::endl;
	}

	return os;
}

std::ostream& operator<<(std::ostream &os, const ClientData &obj)
{
	os << "Client MAC: " << obj.mac << "\n\tAvg RSSI: " << obj.avg_rssi;
	os << "\n\tFirst seen: " << ctime(&obj.first_seen);
	if (obj.asked_ssids.size()) {
		os << "\tAsked SSIDs:";
		std::set<std::string>::const_iterator it;
		for (it = obj.asked_ssids.begin(); it != obj.asked_ssids.end(); ++it) {
			os << " " << *it;
		}
		os << std::endl;
	}
	
	return os;
}

