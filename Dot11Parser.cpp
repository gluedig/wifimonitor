#include "Dot11Parser.h"
using namespace Tins;

Dot11Parser::~Dot11Parser()
{
	ap_set.clear();

}

void Dot11Parser::parseBeacon(ClientInfo *info, const PDU &pdu)
{
	const Dot11Beacon *beacon = pdu.find_pdu<Dot11Beacon>();
	if (!beacon)
		return;
	
	info->mac = beacon->addr3();
	if (!ap_set.count(info->mac)) {
		std::cout << "New AP MAC: " << info->mac << " SSID: " << beacon->ssid() << " RSSI: " << (int)info->rssi << std::endl;

	}
	ap_set.insert(info->mac);
}

void Dot11Parser::parseRTS(ClientInfo *info, const PDU &pdu)
{
	const Dot11RTS *rts = pdu.find_pdu<Dot11RTS>();
	if (!rts)
		return;
	
	info->mac = rts->target_addr();
	if (ap_set.count(info->mac))
		return;
	info->interesting = true;

}

void Dot11Parser::parseData(ClientInfo *info, const PDU &pdu)
{
	const Dot11Data *data = pdu.find_pdu<Dot11Data>();
	if (!data)
		return;

	info->mac = data->addr3();
	if (ap_set.count(info->mac))
		return;
	info->interesting = true;

}

void Dot11Parser::parseProbeReq(ClientInfo *info, const PDU &pdu)
{
	const Dot11ProbeRequest *probe = pdu.find_pdu<Dot11ProbeRequest>();
	if (!probe)
		return;
	info->mac = probe->addr2();
	info->interesting = true;
	info->asked_SSID = probe->ssid();
}

bool Dot11Parser::parse(ClientInfo *info, const PDU &pdu)
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
			parseBeacon(info, pdu);
			break;

		case PDU::DOT11_RTS:
			parseRTS(info, pdu);
			break;

		case PDU::DOT11_DATA:
		case PDU::DOT11_QOS_DATA:
			parseData(info, pdu);
			break;
	
		case PDU::DOT11_PROBE_REQ:
			parseProbeReq(info, pdu);
			break;
		
		default:
			break;
	}
	
	return true;
}
