#include <set>
#include <chrono>
#include "EventMessage.h"
#include "MonitorId.h"

EventMessage::EventMessage(EventType _type, Tins::Dot11::address_type _mac) :
        type(_type), mac(_mac)
{
        std::chrono::time_point<std::chrono::high_resolution_clock> tp = std::chrono::system_clock::now();
        timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
}

void EventMessage::serialize_start()
{
        root = json_object();
        json_object_set_new(root, "timestamp", json_integer(timestamp));
        json_object_set_new(root, "origin_id", json_integer(MonitorId::getInstance().getId()));
        json_object_set_new(root, "mac", json_string(mac.to_string().c_str()));
        json_object_set_new(root, "event_type", json_integer(type));
}

std::string EventMessage::serialize_end()
{
        std::string ret;
        if (root) {
                json_t *root_arr = json_array();
                json_array_append(root_arr, root);
                char *dump = json_dumps(root_arr, 0);
                json_decref(root_arr);
                if (dump) {
                        ret = std::string(dump);
                        free(dump);
                }
                json_decref(root);
        }
        return ret;
}

ClientEventMessage::ClientEventMessage(EventType _type, Tins::Dot11::address_type _mac,
                                       int _rssi, int _prev_rssi, std::set<std::string> _ssids) :
        rssi(_rssi), prev_rssi(_prev_rssi), ssids(_ssids),
        EventMessage(_type, _mac)
{
}

std::string ClientEventMessage::serialize()
{
        EventMessage::serialize_start();
        json_object_set_new(root, "rssi", json_integer(rssi));
        json_object_set_new(root, "prev_rssi", json_integer(prev_rssi));
        json_t *ssid_obj = json_array();

        auto it = ssids.begin();
        while (it != ssids.end()) {
                json_array_append_new(ssid_obj, json_string((*it).c_str()));
                it++;
        }
        json_object_set_new(root, "probed_ssids", ssid_obj);
        return EventMessage::serialize_end();
}

ApEventMessage::ApEventMessage(EventType _type, Tins::Dot11::address_type _mac,
                               int _rssi, int _channel,  std::string _ssid) :
        rssi(_rssi), channel(_channel), ssid(_ssid),
        EventMessage(_type, _mac)
{
}

std::string ApEventMessage::serialize()
{
        EventMessage::serialize_start();
        json_object_set_new(root, "rssi", json_integer(rssi));
        json_object_set_new(root, "channel", json_integer(channel));
        json_object_set_new(root, "ssid", json_string(ssid.c_str()));
        return EventMessage::serialize_end();
}

std::ostream &operator<<(std::ostream &os, const EventMessage &obj)
{
        char buf[128] = {0};
        const time_t time = obj.timestamp / 1000;
        int ms = obj.timestamp % 1000;
        strftime(buf, 128, "%F %T", localtime(&time));
        os << buf
//        << "." << std::setw(3) << std::setfill('0') << ms
           << ", " << obj.mac;

        return os;
}

std::ostream &operator<<(std::ostream &os, const ClientEventMessage &obj)
{
        os << static_cast<const EventMessage &>(obj) << ", "
           << obj.rssi << ",";
//                << obj.prev_rssi << ", ";

        auto it = obj.ssids.begin();
        while (it != obj.ssids.end()) {
                os << " <" << (*it) << ">";
                it++;
        }

        return os;
}

std::ostream &operator<<(std::ostream &os, const ApEventMessage &obj)
{
        os << static_cast<const EventMessage &>(obj) << ", "
           << obj.rssi << ", "
           << obj.channel << ", "
           << obj.ssid;

        return os;
}
