#ifndef _WEBSOCKET_EVENT_SENDER_H_
#define _WEBSOCKET_EVENT_SENDER_H_
#include "EventSender.h"
#include <easylogging++.h>

#include <jansson.h>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/client.hpp>
typedef websocketpp::client<websocketpp::config::asio> wsclient;

using namespace websocketpp;

class WSEventSender : public EventSender
{
        public:
                WSEventSender();
                virtual ~WSEventSender();
                int connect(std::string addr);
                virtual bool sendMessage(EventMessage &msg);
                virtual void set_watch_callback(mac_clbk_fnct);
                virtual void set_unwatch_callback(mac_clbk_fnct);

        private:
                std::unique_ptr<wsclient> ws_clt;
                wsclient::connection_ptr ws_con;
                std::string ws_url;
                int mon_id;
                mac_clbk_fnct watch_callback;
                mac_clbk_fnct unwatch_callback;

                void on_message(connection_hdl hdl, wsclient::message_ptr msg);
                void on_open(connection_hdl hdl);
                void on_close(connection_hdl hdl);
                void on_fail(connection_hdl hdl);
                void handle_connect();
                std::vector<std::string>* handle_macs_list(json_t *root);
};
#endif

