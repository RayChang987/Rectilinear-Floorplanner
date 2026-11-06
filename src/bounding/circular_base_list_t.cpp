//
// Created by 林士傑 on 2023/6/20.
//

#include "circular_base_list_t.h"

#pragma region circular_base_node_t

circular_base_node_t *circular_base_node_t::get_prev() const {
    return this->prev;
}

circular_base_node_t *circular_base_node_t::get_next() const {
    return this->next;
}

void circular_base_node_t::set_prev(circular_base_node_t *node) {
    this->prev = node;
}

void circular_base_node_t::set_next(circular_base_node_t *node) {
    this->next = node;
}

void circular_base_node_t::concat_prev(circular_base_node_t *node) {
    node->set_next(this);
    node->set_prev(this->get_prev());
    this->get_prev()->set_next(node);
    this->set_prev(node);
}

void circular_base_node_t::concat_next(circular_base_node_t *node) {
    node->set_next(this->get_next());
    node->set_prev(this);
    this->get_next()->set_prev(node);
    this->set_next(node);
}

void circular_base_node_t::delete_node() {
    this->prev->next = this->next;
    this->next->prev = this->prev;
}

#pragma endregion

circular_base_list_t::circular_base_list_t() {
    head = new circular_base_node_t();
    tail = new circular_base_node_t();
    head->set_prev(tail); head->set_next(tail);
    tail->set_prev(head); tail->set_next(head);
}

circular_base_list_t::~circular_base_list_t() {
    circular_base_node_t* cur = this->head->get_next();
    while(cur != this->tail) {
        auto next = cur->get_next();
        delete cur;
        cur = next;
    }
    delete head;
    delete tail;
}

void circular_base_list_t::travel_list() {
    circular_base_node_t* cur = this->head->get_next();
    while(cur != this->tail) {

        cur = cur->get_next();
    }
}

bool circular_base_list_t::operator==(const circular_base_list_t &circular_list) const {
    if(this->length != circular_list.length) return false;
    return true;
}

const int &circular_base_list_t::get_length() const {
    return this->length;
}

bool circular_base_list_t::is_empty() const {
    return length == 0;
}

circular_base_node_t *circular_base_list_t::begin() const {
    return this->head->get_next();
}

circular_base_node_t *circular_base_list_t::end() const {
    return this->tail;
}

void circular_base_list_t::drop() {
    this->head->delete_node();
    this->tail->delete_node();
    this->head->set_prev(this->tail); this->head->set_next(this->tail);
    this->tail->set_prev(this->head); this->tail->set_next(this->head);
}

void circular_base_list_t::add_at_head(circular_base_node_t *new_node) {
    this->length++;
    new_node->set_next(this->head->get_next());
    new_node->set_prev(this->head);
    this->head->get_next()->set_prev(new_node);
    this->head->set_next(new_node);
}

void circular_base_list_t::add_at_tail(circular_base_node_t *new_node) {
    this->length++;
    new_node->set_next(this->tail);
    new_node->set_prev(this->tail->get_prev());
    this->tail->get_prev()->set_next(new_node);
    this->tail->set_prev(new_node);
}

void circular_base_list_t::concat_prev(circular_base_node_t *node, circular_base_node_t *new_node) {
    this->length++;
    node->concat_prev(new_node);
}

void circular_base_list_t::concat_next(circular_base_node_t *node, circular_base_node_t *new_node) {
    this->length++;
    node->concat_next(new_node);
}

circular_base_node_t *circular_base_list_t::get_prev(circular_base_node_t *node) const {
    if(node->get_prev() == this->head) {
        return this->tail->get_prev();
    }
    return node->get_prev();
}

circular_base_node_t *circular_base_list_t::get_next(circular_base_node_t *node) const {
    if(node->get_next() == this->tail) {
        return this->head->get_next();
    }
    return node->get_next();
}

void circular_base_list_t::delete_node(circular_base_node_t *node) {
    this->length--;
    node->delete_node();
    delete node;
}

circular_base_node_t *circular_base_list_t::delete_node_get_prev(circular_base_node_t *node) {
    circular_base_node_t* prev = node->get_prev();
    this->delete_node(node);
    return prev;
}

circular_base_node_t *circular_base_list_t::delete_node_get_next(circular_base_node_t *node) {
    circular_base_node_t* next = node->get_next();
    this->delete_node(node);
    return next;
}

circular_base_node_t *circular_base_list_t::delete_node_get_real_prev(circular_base_node_t *node) {
    circular_base_node_t* prev = this->get_prev(node);
    this->delete_node(node);
    return prev;
}

circular_base_node_t *circular_base_list_t::delete_node_get_real_next(circular_base_node_t *node) {
    circular_base_node_t* next = this->get_next(node);
    this->delete_node(node);
    return next;
}
