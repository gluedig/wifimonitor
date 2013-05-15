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
                        AP_REMOVE,
                        AP_UPDATE,
                };

                EventMessage(EventType type, Tins::Dot11::address_type mac);
                virtual std::string serialize() = 0;
                friend std::ostream &operator<<(std::ostream &os, const EventMessage &obj);
        protected:
                void serialize_start();
                std::string serialize_end();
                EventType type;
                Tins::Dot11::address_type mac;
                unsigned long int timestamp;

                json_t *root;
};

class ClientEventMessage : public EventMessage
{
        public:
                ClientEventMessage(EventType type, Tins::Dot11::address_type mac,
                                   int rssi, int prev_rssi, std::set<std::string> ssids);
                virtual std::string serialize();
                friend std::ostream &operator<<(std::ostream &os, const ClientEventMessage &obj);
        protected:
                int rssi;
                int prev_rssi;
                std::set<std::string> ssids;

};

class ApEventMessage : public EventMessage
{
        public:
                ApEventMessage(EventType type, Tins::Dot11::address_type mac,
                               int rssi, int channel, std::string ssid);
                virtual std::string serialize();
                friend std::ostream &operator<<(std::ostream &os, const ApEventMessage &obj);
        protected:
                int rssi;
                int channel;
                std::string ssid;

};

#endif
