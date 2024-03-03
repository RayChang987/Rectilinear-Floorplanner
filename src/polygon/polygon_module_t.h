//
// Created by 林士傑 on 2023/6/9.
//

#ifndef ICCAD2023PD_POLYGON_MODULE_T_H
#define ICCAD2023PD_POLYGON_MODULE_T_H

#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <set>
#include "utilities/bounding_rectangle_t.h"

class polygon_module_t : public box_t{
public:
    bounding_rectangle_t module_rect;

    int32_t max_area;
    std::map<polygon_module_t*, int32_t> area_from_where;
    std::vector<polygon_module_t*> connections;

    bool if_found = false;

public:

    explicit polygon_module_t(const bounding_rectangle_t& bounding_rect);

    polygon_module_t(const polygon_module_t& polygon_module);
    polygon_module_t operator=(const polygon_module_t& r);

    const rect_t &get_bounding_rect() const override;

    const bounding_rectangle_t& get_module_bounding_rectangle() const;

    void add_connection(polygon_module_t* connection);

    void set_area(std::set<polygon_module_t*> area_keys);

    void fix_max_area(int area);

    std::map<polygon_module_t*, int32_t> set_overlap_take(polygon_module_t* requester, int32_t area);

    void set_area_from_where(const std::map<polygon_module_t*, int32_t> &areaFromWhere);

    const std::map<polygon_module_t*, int32_t>& get_area_from_where();

    void fix_area_reset();

    int fix_area(polygon_module_t* robber, int value);

};


#endif //ICCAD2023PD_POLYGON_MODULE_T_H
