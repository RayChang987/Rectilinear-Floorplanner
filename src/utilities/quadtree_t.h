//
// Created by 林士傑 on 2023/4/28.
//

#ifndef ICCAD2023PD_QUADTREE_T_H
#define ICCAD2023PD_QUADTREE_T_H

#include <iostream>
#include <type_traits>
#include <utility>
#include <vector>
#include <functional>
#include "rect_t.h"
#include "utilities/box_t.h"

template <typename T>
class quadtree_t {
    static_assert(std::is_base_of_v<box_t,T>);

    class quadtree_node_t {
        friend class quadtree_t<T>;
        rect_t range;
        std::vector<std::reference_wrapper<T>> elements;
        // 0 : upper_left, 1 : upper_right, 2 : lower_left, 3 : lower_right
        // 0 1
        // 2 3
        std::vector<quadtree_node_t*> children = std::vector<quadtree_node_t*>(4, nullptr);
        quadtree_node_t();
        quadtree_node_t(rect_t range);
        void split();
        int get_rect_region(const rect_t& rect);
        bool is_leaf() const;
        void add_value(T& value);
        void search_collision(std::vector<std::reference_wrapper<T>>& vec, const T& value) const;
        void print(int index);
    };

    const static int max_element_count = 5;
    const static int max_depth = 5;
    quadtree_node_t* rt = nullptr;
    std::vector<T> values;
    quadtree_t<T>() = default;
public:
    quadtree_t<T>(const rect_t& range);
    void add_value(const T& value);
    const std::vector<T>& get_values() const;
    std::vector<T>& get_values_ref();
    std::vector<std::reference_wrapper<T>> collision_value(const T& value) const;
    void print() const;
};

//region quadtree_node

template<typename T>
quadtree_t<T>::quadtree_node_t::quadtree_node_t() = default;

template<typename T>
quadtree_t<T>::quadtree_node_t::quadtree_node_t(rect_t range) : range(std::move(range)) {

}

template<typename T>
void quadtree_t<T>::quadtree_node_t::split() {
    children[0] = new quadtree_t<T>::quadtree_node_t(rect_t(vec2d_t(range.get_left_lower().get_x(), range.get_center().get_y()), vec2d_t(range.get_size().get_half_x(), range.get_size().get_half_y())));
    children[1] = new quadtree_t<T>::quadtree_node_t(rect_t(vec2d_t(range.get_center().get_x(), range.get_center().get_y()), vec2d_t(range.get_size().get_half_x(), range.get_size().get_half_y())));
    children[2] = new quadtree_t<T>::quadtree_node_t(rect_t(vec2d_t(range.get_left_lower().get_x(), range.get_left_lower().get_y()), vec2d_t(range.get_size().get_half_x(), range.get_size().get_half_y())));
    children[3] = new quadtree_t<T>::quadtree_node_t(rect_t(vec2d_t(range.get_center().get_x(), range.get_left_lower().get_y()), vec2d_t(range.get_size().get_half_x(), range.get_size().get_half_y())));

    for(int i = elements.size() - 1; i >= 0; --i){
        int region = this->get_rect_region(elements[i].get().get_bounding_rect());
        if(region != -1){
            children[region]->add_value(elements[i]);
            elements.erase(elements.begin() + i);
        }
    }
}

template<typename T>
int quadtree_t<T>::quadtree_node_t::get_rect_region(const rect_t &rect) {
    if(rect.get_right_upper().get_y() < this->range.get_center().get_y()){
        if(rect.get_right_upper().get_x() < this->range.get_center().get_x()){
            return 2;
        }else if(rect.get_left_lower().get_x() >= this->range.get_center().get_x()){
            return 3;
        }
    }else if(rect.get_left_lower().get_y() >= this->range.get_center().get_y()){
        if(rect.get_right_upper().get_x() < this->range.get_center().get_x()){
            return 0;
        }else if(rect.get_left_lower().get_x() >= this->range.get_center().get_x()){
            return 1;
        }
    }
    return -1;
}

template<typename T>
bool quadtree_t<T>::quadtree_node_t::is_leaf() const {
    return children[0] == nullptr;
}

template<typename T>
void quadtree_t<T>::quadtree_node_t::add_value(T& value) {
    if(this->is_leaf()){
        this->elements.push_back(value);
        if(this->elements.size() >= quadtree_t::max_element_count){
            this->split();
        }
    }else{
        int region = get_rect_region(value.get_bounding_rect());
        if(region == -1)
            this->elements.push_back(value);
        else
            this->children[region]->add_value(value);
    }
}

template<typename T>
void quadtree_t<T>::quadtree_node_t::search_collision(std::vector<std::reference_wrapper<T>> &vec, const T &value) const {
    for (auto rects : this->elements) {
        if(value.get_bounding_rect().is_collision(rects.get().get_bounding_rect())){
            vec.push_back(rects);
        }
    }
    if(is_leaf()) return;

    if(value.get_bounding_rect().get_left_lower().get_x() < this->range.get_center().get_x() &&
       value.get_bounding_rect().get_right_upper().get_y() >= this->range.get_center().get_y()){
        this->children[0]->search_collision(vec, value);
    }

    if(value.get_bounding_rect().get_right_upper().get_x() >= this->range.get_center().get_x() &&
            value.get_bounding_rect().get_right_upper().get_y() >= this->range.get_center().get_y()){
        this->children[1]->search_collision(vec, value);
    }

    if(value.get_bounding_rect().get_left_lower().get_x() < this->range.get_center().get_x() &&
       value.get_bounding_rect().get_left_lower().get_y() < this->range.get_center().get_y()){
        this->children[2]->search_collision(vec, value);
    }

    if(value.get_bounding_rect().get_right_upper().get_x() >= this->range.get_center().get_x() &&
       value.get_bounding_rect().get_left_lower().get_y() < this->range.get_center().get_y()){
        this->children[3]->search_collision(vec, value);
    }
}

template<typename T>
void quadtree_t<T>::quadtree_node_t::print(int index) {
    for(int i = 0; i < elements.size(); i++){
        for(int j = 0; j < index; j++){
            std::cout << "\t";
        }
        std::cout << elements[i].get().get_bounding_rect() << std::endl;
    }
    if(is_leaf()) return;

    for(int j = 0; j < index; j++){
        std::cout << "\t";
    }
    std::cout << "lu : " << this->children[0]->range << "\n";
    this->children[0]->print(index + 1);

    for(int j = 0; j < index; j++){
        std::cout << "\t";
    }
    std::cout << "ru : " << this->children[1]->range << "\n";
    this->children[1]->print(index + 1);

    for(int j = 0; j < index; j++){
        std::cout << "\t";
    }
    std::cout << "ll : " << this->children[2]->range << "\n";
    this->children[2]->print(index + 1);

    for(int j = 0; j < index; j++){
        std::cout << "\t";
    }
    std::cout << "rl : " << this->children[3]->range << "\n";
    this->children[3]->print(index + 1);
}

//endregion

//region quadtree

template<typename T>
quadtree_t<T>::quadtree_t(const rect_t &range){
    this->rt = new quadtree_node_t(range);
    this->values.reserve(200);
}

template<typename T>
void quadtree_t<T>::add_value(const T& value) {
    this->values.push_back(value);
    this->rt->add_value(this->values.back());
}

template<typename T>
const std::vector<T>& quadtree_t<T>::get_values() const{
    return this->values;
}

template<typename T>
std::vector<T> &quadtree_t<T>::get_values_ref(){
    return this->values;
}

template<typename T>
std::vector<std::reference_wrapper<T>> quadtree_t<T>::collision_value(const T &value) const {
    std::vector<std::reference_wrapper<T>> vec;
    this->rt->search_collision(vec, value);
    return vec;
}

template<typename T>
void quadtree_t<T>::print() const {
    this->rt->print(0);
}

//endregion

#endif //ICCAD2023PD_QUADTREE_T_H
