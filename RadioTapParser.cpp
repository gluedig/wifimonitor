#include "RadioTapParser.h"

bool RadioTapParser::parse(ClientInfo *info, const Tins::PDU &pdu)
{
        if (pdu.pdu_type() != Tins::PDU::RADIOTAP)
                return false;

        const Tins::RadioTap *rt = pdu.find_pdu<Tins::RadioTap>();
        if (!rt)
                return false;

        if (rt->flags() & Tins::RadioTap::FAILED_FCS)
                return false;

        if (!(rt->present() & Tins::RadioTap::DBM_SIGNAL))
                return false;

        info->rssi = rt->dbm_signal();
        info->pdu_type = pdu.pdu_type();
        return true;
}

