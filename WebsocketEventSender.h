#ifndef _WEBSOCKET_EVENT_SENDER_H_
#define _WEBSOCKET_EVENT_SENDER_H_
#include "EventSender.h"

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/client.hpp>
typedef websocketpp::client<websocketpp::config::asio> wsclient;

using namespace websocketpp;

class WSEventSender : public EventSender
{
        public:
                WSEventSender();
                ~WSEventSender();
                int connect(std::string addr);
                virtual bool sendMessage(EventMessage &msg);

        private:
                wsclient websock_client;
                void on_message(connection_hdl hdl, wsclient::message_ptr msg);



};
#endif

