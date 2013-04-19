#ifndef _CLIENTINFO_H_
#define _CLIENTINFO_H_

#include <stdint.h>
#include <string>
#include <iostream>
#include <tins/tins.h>

class ClientInfo
{
        public:
                Tins::Timestamp timestamp;
                bool interesting;
                int8_t rssi;
                Tins::Dot11::address_type mac;
                Tins::PDU::PDUType pdu_type;
                std::string asked_SSID;
                int channel;


                ClientInfo() : rssi(0), channel(0), interesting(false) {};

                friend std::ostream &operator<<(std::ostream &os, ClientInfo &obj) {
                        // write obj to stream
                        os << "[" << obj.timestamp.seconds() << "." << obj.timestamp.microseconds() << "] ";
                        os << "PDUtype: "<< obj.pdu_type << " MAC: " << obj.mac << " RSSI: " << std::dec << (int)obj.rssi;
                        if (obj.asked_SSID.size())
                                os << " SSID: " << obj.asked_SSID;
                        if (obj.channel)
                                os << " Channel: " << obj.channel;
                        os <<  std::endl;
                        return os;
                };
};
#endif
