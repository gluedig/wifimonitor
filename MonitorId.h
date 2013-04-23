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
                        return 1;
                }
        private:
                MonitorId() {};
                MonitorId(MonitorId const &);
                void operator=(MonitorId const &);

};
#endif
