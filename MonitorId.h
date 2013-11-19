#ifndef _MONITOR_ID_H_
#define _MONITOR_ID_H_
class MonitorId
{
        public:
                static MonitorId &getInstance() {
                        static MonitorId instance;
                        return instance;
                }
                int getId() {
                        return id;
                }
                int setId(int _id) {
                        id = _id;
                }
        private:
                MonitorId() : id(0) {};
                MonitorId(MonitorId const &);
                void operator=(MonitorId const &);
                int id;

};
#endif
