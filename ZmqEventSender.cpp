#include <assert.h>
#include "ZmqEventSender.h"

ZmqEventSender::ZmqEventSender()
{
        ctx = zctx_new();
}

ZmqEventSender::~ZmqEventSender()
{
        if(zmq_pub_sock)
                zsocket_destroy(ctx, zmq_pub_sock);
        zctx_destroy(&ctx);
}

int ZmqEventSender::bind(std::string address)
{
        zmq_pub_sock = zsocket_new(ctx, ZMQ_PUB);
        assert(zmq_pub_sock);
        return zsocket_bind (zmq_pub_sock, address.c_str());
}

bool ZmqEventSender::sendMessage(EventMessage &msg)
{
        if (zmq_pub_sock) {
                int rc;
                std::string msg_str = msg.serialize();
                size_t msg_size = msg_str.size();
                char *buf = (char *)malloc(msg_size);
                memcpy(buf, msg_str.c_str(), msg_size);
                zframe_t *frame = zframe_new (buf, msg_size);
                rc = zframe_send (&frame, zmq_pub_sock, 0);
                free(buf);
                assert (rc == 0);
                return true;
        }
        return false;


}
