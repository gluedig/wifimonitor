#ifndef _RADIOTAP_PARSER_H_
#define _RADIOTAP_PARSER_H_
#include "Parser.h"

class RadioTapParser : public Parser {
public:	
	virtual bool parse(ClientInfo * info, const Tins::PDU &pdu);
};

#endif
