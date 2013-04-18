#ifndef _ZMQ_EVENT_SENDER_H_
#define _ZMQ_EVENT_SENDER_H_
#include <czmq.h>
#include "EventSender.h"

class ZmqEventSender : public EventSender
{
        public:
                ZmqEventSender();
                ~ZmqEventSender();
                void start(std::string addr);
                virtual bool sendMessage(EventMessage &msg);

        private:
                zctx_t *ctx;
};
#endif
