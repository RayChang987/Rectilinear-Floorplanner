#ifndef MCNC_OLD_CONNECTION_T_H
#define MCNC_OLD_CONNECTION_T_H

#include <vector>

class mcnc_old_moudle_t;

class mcnc_old_connection_t {
public:
    std::vector<mcnc_old_moudle_t*> modules;
};

#endif