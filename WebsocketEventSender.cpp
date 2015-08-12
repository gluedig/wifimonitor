#include "WebsocketEventSender.h"
#include "MonitorId.h"

WSEventSender::WSEventSender()
{
        watch_callback = unwatch_callback = NULL;
        ws_clt = std::unique_ptr<wsclient>(new wsclient());
        ws_clt->set_message_handler(bind(&WSEventSender::on_message, this, std::placeholders::_1, std::placeholders::_2));
        ws_clt->set_open_handler(bind(&WSEventSender::on_open, this, std::placeholders::_1));
        ws_clt->set_close_handler(bind(&WSEventSender::on_close, this, std::placeholders::_1));
        ws_clt->set_fail_handler(bind(&WSEventSender::on_fail, this, std::placeholders::_1));
        ws_clt->clear_access_channels(websocketpp::log::alevel::all);
        ws_clt->set_access_channels(websocketpp::log::alevel::app);

        ws_clt->init_asio();
}

WSEventSender::~WSEventSender()
{
        LOG(DEBUG) << "stopping";
        ws_clt->stop();
}


int WSEventSender::connect(std::string address)
{
        ws_url = address;
        mon_id = MonitorId::getInstance().getId();
        handle_connect();
        ws_clt->run();
}

void WSEventSender::handle_connect()
{
        LOG(INFO) << "connecting to: " << ws_url << " as monitor: " << mon_id;

        websocketpp::lib::error_code ec;
        ws_con = ws_clt->get_connection(ws_url, ec);
        if (ec) {
                LOG(ERROR) << "error: " << ec.message();
        }

        ws_clt->connect(ws_con);
}

bool WSEventSender::sendMessage(EventMessage &msg)
{
        std::string msg_str = msg.serialize();
        ws_con->send(msg_str);
        return true;
}

void WSEventSender::on_message(connection_hdl hdl, wsclient::message_ptr msg)
{
        LOG(DEBUG) << "raw message: " << msg->get_payload();
        json_t *root, *msg_type;
        json_error_t error;
        root = json_loads(msg->get_payload().c_str(), 0, &error);
        if (!root) {
                LOG(ERROR) << "cannot decode msg json: " <<  error.text << " at " << error.line;
                json_decref(root);
                return;
        }
        msg_type = json_object_get(root, "msg");
        if (!json_is_string(msg_type))
        {
                LOG(ERROR) << "cannot decode msg";
                json_decref(root);
                return;
        }
        std::string _msg = std::string(json_string_value(msg_type));
        LOG(DEBUG) << "message: " << _msg;
        if (_msg.compare("watch") == 0) {
                std::vector<std::string> *macs = handle_macs_list(root);
                for (std::vector<std::string>::iterator it = macs->begin() ; it != macs->end(); ++it) {
                        if (watch_callback) {
                                watch_callback(*it);
                        }
                }
                delete macs;
        }
        else if (_msg.compare("unwatch") == 0) {
                std::vector<std::string> *macs = handle_macs_list(root);
                for (std::vector<std::string>::iterator it = macs->begin() ; it != macs->end(); ++it) {
                        if (unwatch_callback) {
                                unwatch_callback(*it);
                        }
                }
                delete macs;
        }
        else
        {
                LOG(ERROR) << "unknown message";
        }

        json_decref(root);
}

std::vector<std::string>* WSEventSender::handle_macs_list(json_t *root)
{
        std::vector<std::string> *macs = new std::vector<std::string>();
        json_t *macs_arr;
        macs_arr = json_object_get(root, "macs");
        if (!json_is_array(macs_arr)) {
                LOG(ERROR) << "cannot decode macs array";
                return macs;
        }
        for (int i=0; i < json_array_size(macs_arr); i++) {
                json_t *mac_obj = json_array_get(macs_arr, i);
                if(!json_is_string(mac_obj)) {
                        LOG(ERROR) << "macs entry: " << i << " not valid";
                        continue;
                }
                macs->push_back(std::string(json_string_value(mac_obj)));
        }

        return macs;
}

void WSEventSender::on_open(connection_hdl hdl)
{
        LOG(INFO) << "connected";
        wsclient::connection_ptr con = ws_clt->get_con_from_hdl(hdl);
        json_t *root = json_object();
        json_object_set_new(root, "mon_id", json_integer(mon_id));
        json_object_set_new(root, "msg", json_string("connect"));
        char * dump = json_dumps(root, 0);
        json_decref(root);
        if (dump) {
                std::string out = std::string(dump);
                free(dump);
                ws_con->send(out);
        }
}

void WSEventSender::on_close(connection_hdl hdl)
{
        LOG(WARNING) << "closed";
        handle_connect();
}

void WSEventSender::on_fail(connection_hdl hdl)
{
        LOG(WARNING) << "failed";
        sleep(10);
        handle_connect();
}

void WSEventSender::set_watch_callback(mac_clbk_fnct fnct)
{
        LOG(DEBUG) << "";
        watch_callback = fnct;
}

void WSEventSender::set_unwatch_callback(mac_clbk_fnct fnct)
{
        LOG(DEBUG) << "";
        unwatch_callback = fnct;
}

