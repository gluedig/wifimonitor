#ifndef _EVENT_SENDER_H_
#define _EVENT_SENDER_H_
#include "EventMessage.h"

//typedef void (*mac_clbk_fnct)(std::string);
typedef std::function<void(std::string)> mac_clbk_fnct;
class EventSender
{
        public:
                virtual ~EventSender(){};
                virtual bool sendMessage(EventMessage &) = 0;
                virtual int connect(std::string) = 0;
                virtual void set_watch_callback(mac_clbk_fnct) = 0;
                virtual void set_unwatch_callback(mac_clbk_fnct) = 0;
};
#endif
