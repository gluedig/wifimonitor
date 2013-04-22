#include <time.h>
#include "ApDb.h"
bool ApDb::inDb(Tins::Dot11::address_type mac)
{
        return db.count(mac);
}


bool ApDb::newApEvent(ClientInfo *info)
{
        if (info->mac == null_address || info->mac == Tins::Dot11::BROADCAST)
                return false;

        db_mutex.lock();
        if (inDb(info->mac)) {
                bool send_update = false;
                ApData data = db[info->mac];
                data.age = 0;
                if (data.ssid != info->asked_SSID) {
                        data.ssid = info->asked_SSID;
                        send_update = true;
                }
                if (data.channel != info->channel) {
                        data.channel = info->channel;
                        send_update = true;
                }
                db[info->mac] = data;

                if (send_update) {
//TODO: use proper id
                        ApEventMessage msg(EventMessage::AP_UPDATE, 1, data.mac,
                                           data.rssi, data.channel, data.ssid);
                        std::cout << msg.serialize() << std::endl;
                        sender->sendMessage(msg);
                }



        } else {
                ApData data;
                data.mac = info->mac;
                data.rssi = (int)info->rssi;
                data.channel = info->channel;
                data.first_seen = info->timestamp.seconds();
                if (info->asked_SSID.size())
                        data.ssid = info->asked_SSID;

                added++;
                db.insert(std::pair<Tins::Dot11::address_type, ApData>(info->mac, data));
//TODO: use proper id
                ApEventMessage msg(EventMessage::AP_ADD, 1, data.mac,
                                   data.rssi, data.channel, data.ssid);
                std::cout << msg.serialize() << std::endl;
                sender->sendMessage(msg);
        }
        db_mutex.unlock();

        return true;
}

void ApDb::cleanup(int maxage)
{
        db_mutex.lock();
        auto it = db.begin();
        while (it != db.end()) {
                it->second.age++;
                if (it->second.age > maxage) {
//TODO: use proper id
                        ApEventMessage msg(EventMessage::AP_REMOVE, 1, it->second.mac,
                                           it->second.rssi, it->second.channel, it->second.ssid);
                        std::cout << msg.serialize() << std::endl;
                        sender->sendMessage(msg);

                        removed++;
                        db.erase(it++);
                } else {
                        ++it;
                }
        }

        db_mutex.unlock();
}

ApDb::ApDb(EventSender *_sender) : sender(_sender), added(0), removed(0)
{

}

std::ostream &operator<<(std::ostream &os, const ApDb &obj)
{
        const std::unordered_map<Tins::Dot11::address_type, ApData> *db = &(obj.db);
        std::unordered_map<Tins::Dot11::address_type, ApData>::const_iterator it;

        os << "ApDb dump, elements: " << db->size() << " added: " << obj.added << " removed: " << obj.removed << std::endl;
        for ( it = db->begin(); it != db->end(); ++it) {
                os << it->second << std::endl;
        }

        return os;
}

std::ostream &operator<<(std::ostream &os, const ApData &obj)
{
        os << "AP MAC: " << obj.mac << "\n\tRSSI: " << obj.rssi;
        os << "\n\tFirst seen: " << ctime(&obj.first_seen);
        os << "\tSSID: " << obj.ssid << "\n\tChannel: " << obj.channel;

        return os;
}

