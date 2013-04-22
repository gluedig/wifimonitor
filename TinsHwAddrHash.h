#ifndef _TINS_HWADDR_HASH_H_
#define _TINS_HWADDR_HASH_H_
namespace std
{
template<> struct hash<Tins::Dot11::address_type> {
        public:
                size_t operator()(const Tins::Dot11::address_type &s) const {
                        return std::hash<std::string>()(s.to_string());
                }
};
}
#endif
