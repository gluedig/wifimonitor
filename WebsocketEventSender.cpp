#include "WebsocketEventSender.h"

WSEventSender::WSEventSender()
{
        websock_client.set_message_handler(bind(&WSEventSender::on_message,this, _1, _2));
        websock_client.init_asio();
}

WSEventSender::~WSEventSender()
{
        websock_client.stop();

}

int WSEventSender::connect(std::string address)
{
        websocketpp::lib::error_code ec;
        wsclient::connection_ptr con = websock_client.get_connection(address, ec);
        if (ec) {
                websock_client.get_alog().write(websocketpp::log::alevel::app,ec.message());
        }

        websock_client.connect(con);
        websock_client.run();
}

bool WSEventSender::sendMessage(EventMessage &msg)
{

}

void WSEventSender::on_message(connection_hdl hdl, wsclient::message_ptr msg)
{

}

