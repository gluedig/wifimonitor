#ifndef _DOT11_PARSER_H_
#define _DOT11_PARSER_H_

#include <map>
#include <set>
#include "Parser.h"
#include "ClientDb.h"
#include "ApDb.h"

class Dot11StaParser : public Parser
{
                bool parseRTS(ClientInfo *info, const Tins::PDU &pdu);
                bool parseData(ClientInfo *info, const Tins::PDU &pdu);
                bool parseQosData(ClientInfo *info, const Tins::PDU &pdu);
                bool parseProbeReq(ClientInfo *info, const Tins::PDU &pdu);
                ClientDb *db;
                ApDb *ap_db;
        public:
                virtual bool parse(ClientInfo *info, const Tins::PDU &pdu);
                Dot11StaParser(ClientDb *db, ApDb *apdb);
};

class Dot11ApParser : public Parser
{
                bool parseBeacon(ClientInfo *info, const Tins::PDU &pdu);
                ApDb *db;
        public:
                virtual bool parse(ClientInfo *info, const Tins::PDU &pdu);
                Dot11ApParser(ApDb *db);
};

#endif
