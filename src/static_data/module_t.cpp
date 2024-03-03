//
// Created by 林士傑 on 2023/3/8.
//

#include "module_t.h"

const std::string &module_t::getName() const {
    return name;
}

const std::vector<std::pair<const module_t *const, const int>> &module_t::getConnections() const {
    return connections;
}
