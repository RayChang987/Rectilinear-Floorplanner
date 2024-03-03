//
// Created by 林士傑 on 2023/6/9.
//

#include "polygon_module_t.h"
#include "static_data/module_t.h"
#include <iostream>

#include <utility>

polygon_module_t::polygon_module_t(const bounding_rectangle_t& bounding_rect) :
        module_rect(bounding_rect),
        max_area(bounding_rect.getRect().get_area())
{}

polygon_module_t::polygon_module_t(const polygon_module_t &r):
        module_rect(r.module_rect),
        max_area(r.max_area),
        area_from_where(r.area_from_where),
        connections(r.connections)
{}

polygon_module_t polygon_module_t::operator=(const polygon_module_t &r) {
    return polygon_module_t(r);
}

const rect_t &polygon_module_t::get_bounding_rect() const {
    return this->module_rect.getRect();
}

const bounding_rectangle_t& polygon_module_t::get_module_bounding_rectangle() const {
    return module_rect;
}

void polygon_module_t::fix_max_area(int area) {
    this->max_area += area;
}

void polygon_module_t::add_connection(polygon_module_t* connection) {
    this->connections.push_back(connection);
}

void polygon_module_t::set_area(std::set<polygon_module_t*> area_keys) {
    if (area_keys.size() == 1){
        //todo maybe not put this
        this->area_from_where[*area_keys.begin()] = this->module_rect.getLinkModule()->get_area();
    }else{
        for(auto key : area_keys){
            this->area_from_where[key] = 0;
        }
    }
}

std::map<polygon_module_t*, int32_t> polygon_module_t::set_overlap_take(polygon_module_t* requester, int32_t area) {
    std::map<polygon_module_t*, int32_t> copy;
    copy[requester] = 0;
    for (auto item : this->area_from_where) {
        copy[item.first] = 0;
    }

    int totalArea = 0;
    for(auto item : this->area_from_where){
        totalArea += item.second;
    }
    area = totalArea - (this->max_area - area);
    if (area <= 0) return copy;

    for (auto item : this->area_from_where) {
        auto value = std::min(area, item.second);
        this->area_from_where[item.first] -= value;
        copy[item.first] = value;
        area -= value;
    }
    return copy;
}

void polygon_module_t::set_area_from_where(const std::map<polygon_module_t*, int32_t> &areaFromWhere) {
    area_from_where = areaFromWhere;
}

const std::map<polygon_module_t*, int32_t>& polygon_module_t::get_area_from_where() {
    return this->area_from_where;
}

void polygon_module_t::fix_area_reset() {
    this->area_from_where.begin()->second = 0;
}

int polygon_module_t::fix_area(polygon_module_t* robber, int value) {
    this->if_found = true;
    if (this->area_from_where.find(robber) == this->area_from_where.end()){
        this->if_found = false;
        return value;
    }
    this->area_from_where[robber] += value;
    int now_area = 0;
    for (auto item : this->area_from_where){
        now_area += item.second;
    }
    int need_area = now_area - this->max_area;
    if (need_area <= 0){
        this->if_found = false;
        return 0;
    }

    for (auto other : this->area_from_where) {
        for (auto other_conn : this->connections) {
            if (other_conn->if_found) continue;
            if (need_area <= 0) {
                this->if_found = false;
                return 0;
            }
            int32_t need = std::min((int32_t)need_area, other.second);
            if (need == 0) continue;
            int left = other_conn->fix_area(other.first, need);
            int move = need - left;
            this->area_from_where[other.first] -= move;
            need_area -= move;
        }
    }
    this->area_from_where[robber] -= need_area;
    this->if_found = false;
    return need_area;
}
