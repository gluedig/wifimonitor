#ifndef _ZMQ_EVENT_SENDER_H_
#define _ZMQ_EVENT_SENDER_H_
#include <czmq.h>
#include "EventSender.h"

#define BEACON_PORT 5555
#define BEACON_INTERVAL 5000

class ZmqEventSender : public EventSender
{
        public:
                ZmqEventSender();
                ~ZmqEventSender();
                int bind(std::string addr);
                virtual bool sendMessage(EventMessage &msg);

        private:
                zctx_t *ctx;
                void *zmq_pub_sock;
                zbeacon_t *beacon_ctx;
};
#endif
