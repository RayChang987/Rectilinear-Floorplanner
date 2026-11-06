//
// Created by 林士傑 on 2023/6/20.
//
#ifndef BOUNDINGLINE_CIRCULAR_BASE_LIST_T_H
#define BOUNDINGLINE_CIRCULAR_BASE_LIST_T_H

#include <vector>
#include <iostream>

#pragma region circular_base_node_t

class circular_base_list_t;

class circular_base_node_t {
protected:
    friend class circular_base_list_t;
    circular_base_node_t* prev = this;
    circular_base_node_t* next = this;
    circular_base_node_t() = default;

    virtual circular_base_node_t* get_prev() const;

    virtual circular_base_node_t* get_next() const;

    virtual void set_prev(circular_base_node_t* node);

    virtual void set_next(circular_base_node_t* node);

    void concat_prev(circular_base_node_t* node);

    void concat_next(circular_base_node_t* node);

    virtual void delete_node();
public:
};

#pragma endregion

#pragma region circular_base_list_t

class circular_base_list_t {
protected:
    circular_base_node_t* head = nullptr;
    circular_base_node_t* tail = nullptr;
    int length = 0;

    circular_base_node_t* begin() const;

    circular_base_node_t* end() const;

    void drop();

    void add_at_head(circular_base_node_t* new_node);

    void add_at_tail(circular_base_node_t* new_node);

    void concat_prev(circular_base_node_t* node, circular_base_node_t* new_node);

    void concat_next(circular_base_node_t* node, circular_base_node_t* new_node);

    virtual circular_base_node_t* get_prev(circular_base_node_t* node) const;

    virtual circular_base_node_t* get_next(circular_base_node_t* node) const;

    void delete_node(circular_base_node_t* node);

    circular_base_node_t* delete_node_get_prev(circular_base_node_t* node);

    circular_base_node_t* delete_node_get_next(circular_base_node_t* node);

    circular_base_node_t* delete_node_get_real_prev(circular_base_node_t* node);

    circular_base_node_t* delete_node_get_real_next(circular_base_node_t* node);
public:
    circular_base_list_t();
    ~circular_base_list_t();

    void travel_list();

    bool operator==(const circular_base_list_t& circular_list) const;

    const int& get_length() const;

    bool is_empty() const;
};

#pragma endregion

#endif //BOUNDINGLINE_CIRCULAR_BASE_LIST_T_H
