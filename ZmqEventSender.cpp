#include <assert.h>
#include "ZmqEventSender.h"

ZmqEventSender::ZmqEventSender()
{
        ctx = zctx_new();
        beacon_ctx = zbeacon_new(BEACON_PORT);
        zbeacon_set_interval(beacon_ctx, BEACON_INTERVAL);
}

ZmqEventSender::~ZmqEventSender()
{
        if (beacon_ctx)
                zbeacon_destroy(&beacon_ctx);
        if(zmq_pub_sock)
                zsocket_destroy(ctx, zmq_pub_sock);
        zctx_destroy(&ctx);
}

int ZmqEventSender::bind(std::string address)
{
        zmq_pub_sock = zsocket_new(ctx, ZMQ_PUB);
        assert(zmq_pub_sock);
        int port = zsocket_bind (zmq_pub_sock, address.c_str());
        assert(port);
        char sock_endpoint[256];
        size_t endpoint_size = sizeof(sock_endpoint);
        zmq_getsockopt(zmq_pub_sock, ZMQ_LAST_ENDPOINT, sock_endpoint, &endpoint_size);
        std::cerr << "ZmqEventSender bound to: " << sock_endpoint << " [" << address << "]" << std::endl;

        char beacon[256];
        sprintf(beacon, "EVENT_SRC:%s", sock_endpoint);
        std::cerr << "ZmqEventSender sending EVENT_SRC beacons from port ";
        std::cerr << BEACON_PORT << " every " << BEACON_INTERVAL << " ms <" << beacon << ">\n";
        zbeacon_publish(beacon_ctx, (byte *)beacon, strlen(beacon));

        return port;
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

