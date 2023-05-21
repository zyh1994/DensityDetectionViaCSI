#ifndef __PING_H__
#define __PING_H__

#include <string>

struct NetworkInterface {
    std::string name;
    std::string hostAddr;
    std::string netmask;
};

// Get the first non-loopback interface
extern void get_first_non_loopback_interface(NetworkInterface* result) ;

#endif