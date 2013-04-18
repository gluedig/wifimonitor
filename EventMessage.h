#ifndef _EVENT_MESSAGE_H_
#define _EVENT_MESSAGE_H_
#include <string>
#include <tins/tins.h>
#include <jansson.h>

class EventMessage
{
        public:
                enum EventType {
                        CLIENT_ADD,
                        CLIENT_REMOVE,
                        CLIENT_UPDATE,
                        AP_ADD,
                };

                EventMessage(EventType type, int origin_id, Tins::Dot11::address_type mac);
                ~EventMessage();
                virtual std::string serialize();
        protected:
                EventType type;
                int origin_id;
                Tins::Dot11::address_type mac;

                json_t *root;
};

class ClientEventMessage : public EventMessage
{
        public:
                ClientEventMessage(EventType type, int origin_id, Tins::Dot11::address_type mac,
                                   int rssi, int prev_rssi, std::set<std::string> ssids);

        protected:
                int rssi;
                int prev_rssi;
                std::set<std::string> ssids;

};

class ApEventMessage : public EventMessage
{
        public:
                ApEventMessage(EventType type, int origin_id, Tins::Dot11::address_type mac,
                               int rssi, int channel, std::string ssid);

        protected:
                int rssi;
                int channel;
                std::string ssid;

};

#endif
