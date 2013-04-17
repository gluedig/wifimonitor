#ifndef _PARSER_H_
#define _PARSER_H_
#include <tins/tins.h>

#include "ClientInfo.h"

class Parser {
public:
	virtual bool parse (ClientInfo *, const Tins::PDU &) = 0;
};

#endif

