#include "Dot11Parser.h"
#include "EventMessage.h"
using namespace Tins;

static bool isBroadcast(Tins::Dot11::address_type mac)
{
        auto first_octet = mac.begin();
        if ( *first_octet & 0x1)
                return true;
        else
                return false;
}

Dot11ApParser::Dot11ApParser(ApDb *_db) : db(_db)
{
}

bool Dot11ApParser::parseBeacon(ClientInfo *info, const PDU &pdu)
{
        const Dot11Beacon *beacon = pdu.find_pdu<Dot11Beacon>();
        if (!beacon)
                return false;

        info->mac = beacon->addr3();
        info->asked_SSID = beacon->ssid();
        info->channel = (int)beacon->ds_parameter_set();
        info->interesting = true;
        db->newApEvent(info);

        return true;
}

bool Dot11ApParser::parse(ClientInfo *info, const PDU &pdu)
{
        const Dot11 *dot11 = pdu.find_pdu<Dot11>();
        if (!dot11)
                return false;


        PDU::PDUType pdu_type = dot11->pdu_type();
        info->pdu_type = pdu_type;

        /* ignore frames from DS */
        if (dot11->from_ds())
                return false;

        switch (pdu_type) {
                case PDU::DOT11_BEACON:
                        return parseBeacon(info, pdu);
                default:
                        return true;
        }
}

Dot11StaParser::Dot11StaParser(ClientDb *_db, ApDb *_ap_db) : db(_db), ap_db(_ap_db)
{
}

bool Dot11StaParser::parseRTS(ClientInfo *info, const PDU &pdu)
{
        const Dot11RTS *rts = pdu.find_pdu<Dot11RTS>();
        if (!rts)
                return false;

        info->mac = rts->target_addr();
        if (isBroadcast(info->mac))
                return false;
        if(ap_db && ap_db->inDb(info->mac))
                return false;
        info->interesting = true;
        db->newClientEvent(info);

        return true;
}

bool Dot11StaParser::parseData(ClientInfo *info, const PDU &pdu)
{
        const Dot11Data *data = pdu.find_pdu<Dot11Data>();
        if (!data)
                return false;

        info->mac = data->addr3();
        if (isBroadcast(info->mac))
                return false;
        if(ap_db && ap_db->inDb(info->mac))
                return false;
        info->interesting = true;
        db->newClientEvent(info);

        return true;
}

bool Dot11StaParser::parseQosData(ClientInfo *info, const PDU &pdu)
{
        const Dot11Data *data = pdu.find_pdu<Dot11Data>();
        if (!data)
                return false;

        info->mac = data->addr2();
        if(ap_db && ap_db->inDb(info->mac))
                return false;
        info->interesting = true;
        db->newClientEvent(info);

        return true;
}
bool Dot11StaParser::parseProbeReq(ClientInfo *info, const PDU &pdu)
{
        const Dot11ProbeRequest *probe = pdu.find_pdu<Dot11ProbeRequest>();
        if (!probe)
                return false;

        info->mac = probe->addr2();
        if (isBroadcast(info->mac))
                return false;
        if(ap_db && ap_db->inDb(info->mac))
                return false;
        info->asked_SSID = probe->ssid();
        info->interesting = true;
        db->newClientEvent(info);

        return true;
}


bool Dot11StaParser::parse(ClientInfo *info, const PDU &pdu)
{
        const Dot11 *dot11 = pdu.find_pdu<Dot11>();
        if (!dot11)
                return false;

        PDU::PDUType pdu_type = dot11->pdu_type();
        info->pdu_type = pdu_type;

        /* ignore frames from DS */
        if (dot11->from_ds())
                return false;

        switch (pdu_type) {
                case PDU::DOT11_RTS:
                        return parseRTS(info, pdu);

                case PDU::DOT11_DATA:
                        return parseData(info, pdu);
                case PDU::DOT11_QOS_DATA:
                        return parseQosData(info, pdu);

                case PDU::DOT11_PROBE_REQ:
                        return parseProbeReq(info, pdu);

                default:
                        return true;
        }
}
