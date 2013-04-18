#ifndef _DOT11_PARSER_H_
#define _DOT11_PARSER_H_

#include <map>
#include <set>
#include "Parser.h"


class Dot11Parser : public Parser
{
                std::set<Tins::Dot11::address_type> ap_set;
                void parseBeacon(ClientInfo *info, const Tins::PDU &pdu);
                void parseRTS(ClientInfo *info, const Tins::PDU &pdu);
                void parseData(ClientInfo *info, const Tins::PDU &pdu);
                void parseProbeReq(ClientInfo *info, const Tins::PDU &pdu);
        public:
                virtual bool parse(ClientInfo *info, const Tins::PDU &pdu);
                virtual ~Dot11Parser();


};

#endif
