#include <set>
#include "EventMessage.h"

EventMessage::EventMessage(EventType _type, int _origin_id, Tins::Dot11::address_type _mac) :
	type(_type), origin_id(_origin_id), mac(_mac)
{
	root = json_object();
	json_object_set_new(root, "event_type", json_integer(type));
	json_object_set_new(root, "origin_id", json_integer(origin_id));	
	json_object_set_new(root, "mac", json_string(mac.to_string().c_str()));
}

EventMessage::~EventMessage()
{
	if(root)
		json_decref(root);

}

std::string EventMessage::serialize()
{
	std::string ret;
	if (root) {
		json_t *root_arr = json_array();
		json_array_append(root_arr, root);
		char* dump = json_dumps(root_arr, 0);
		json_decref(root_arr);
		if (dump) {
			ret = std::string(dump);
			free(dump);
		}
	}
	return ret;
}

ClientEventMessage::ClientEventMessage(EventType _type, int _origin_id, Tins::Dot11::address_type _mac, 
	int _rssi, int _prev_rssi, std::set<std::string> _ssids) :
		rssi(_rssi), prev_rssi(_prev_rssi), ssids(_ssids),
		EventMessage(_type, _origin_id, _mac)
{
	json_object_set_new(root, "rssi", json_integer(rssi));
	json_object_set_new(root, "prev_rssi", json_integer(prev_rssi));
	json_t *ssid_obj = json_array();

	auto it = ssids.begin();
	while (it != ssids.end()) {
		json_array_append_new(ssid_obj, json_string((*it).c_str()));
		it++;
	}
	json_object_set_new(root, "probed_ssids", ssid_obj);
}

ApEventMessage::ApEventMessage(EventType _type, int _origin_id, Tins::Dot11::address_type _mac, 
	int _rssi, int _channel,  std::string _ssid) :
		rssi(_rssi), channel(_channel), ssid(_ssid),
		EventMessage(_type, _origin_id, _mac)
{
	json_object_set_new(root, "rssi", json_integer(rssi));
	json_object_set_new(root, "channel", json_integer(channel));
	json_object_set_new(root, "ssid", json_string(ssid.c_str()));
}

