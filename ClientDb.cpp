#include "ClientDb.h"

#define AVG_COUNT 8
#define RSSI_HIST 1

bool ClientDb::newClientEvent(ClientInfo *info)
{	
	if (info->mac == null_address)
		return false;

	if (db.count(info->mac)) {
		ClientData data = db[info->mac];
		data.age = 0;

		/* do RSSI averaging */
		int prev_rssi = data.last_rssi;
		data.last_rssi = info->rssi;
		int prev_avg = data.avg_rssi;
		data.avg_rssi = prev_avg - prev_rssi/AVG_COUNT + info->rssi/AVG_COUNT;
		
		/* trigger if average RSSI changed */
		if ((data.avg_rssi != prev_avg) &&
			(data.avg_rssi > prev_avg+RSSI_HIST) || (data.avg_rssi < prev_avg-RSSI_HIST)) {
			std::cout << "RSSI changed: " << data << " Prev RSSI: " << prev_avg << std::endl;

		}


		if (info->asked_SSID.size() && !data.asked_ssids.count(info->asked_SSID))
			data.asked_ssids.insert(info->asked_SSID);

		db[info->mac] = data;

	} else {
		ClientData new_data;
		new_data.mac = info->mac;
		new_data.last_rssi = (int)info->rssi;
		new_data.avg_rssi = (int)info->rssi;
		if (info->asked_SSID.size())
			new_data.asked_ssids.insert(info->asked_SSID);


		db.insert(std::pair<Tins::Dot11::address_type, ClientData>(info->mac, new_data));
	}	

	return true;
}

std::ostream& operator<<(std::ostream &os, const ClientDb &obj)
{
	const std::map<Tins::Dot11::address_type, ClientData> *db = &(obj.db);
	std::map<Tins::Dot11::address_type, ClientData>::const_iterator it;

	os << "ClientDb dump, elements: " << db->size() << std::endl;
	for ( it = db->begin(); it != db->end(); ++it)
	{
		os << it->second << std::endl;
	}

	return os;
}

std::ostream& operator<<(std::ostream &os, const ClientData &obj)
{
	os << "Client MAC: " << obj.mac << " Avg RSSI: " << obj.avg_rssi;
	if (obj.asked_ssids.size()) {
		os << std::endl << "Asked SSIDs: " << std::endl;
		std::set<std::string>::const_iterator it;
		for (it = obj.asked_ssids.begin(); it != obj.asked_ssids.end(); ++it) {
			os << *it << std::endl;

		}
	}
	
	return os;
}

