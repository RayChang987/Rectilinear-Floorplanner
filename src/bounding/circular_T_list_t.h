//
// Created by 林士傑 on 2023/7/21.
//

#ifndef BOUNDINGLINE_CIRCULAR_T_LIST_T_H
#define BOUNDINGLINE_CIRCULAR_T_LIST_T_H

#include "circular_base_list_t.h"

template<class T>
class circular_T_list_t;

#pragma region circular_T_node_t

template<class T>
class circular_T_node_t : public circular_base_node_t {
    friend class circular_T_list_t<T>;
    T data;

    circular_T_node_t() = default;
    circular_T_node_t(const T& data);

    void concat_prev(const T& data);

    void concat_next(const T& data);

    void delete_node() override;
public:

    const T& get_data() const;

    T& get_date_ref();

    void set_data(const T& data);

    circular_T_node_t<T>* get_prev() const override;

    circular_T_node_t<T>* get_next() const override;

    void set_prev(circular_T_node_t<T>* node);

    void set_next(circular_T_node_t<T>* node);
};

template<class T>
circular_T_node_t<T>::circular_T_node_t(const T &data) {
    this->data = data;
}

template<class T>
const T &circular_T_node_t<T>::get_data() const {
    return this->data;
}

template<class T>
T &circular_T_node_t<T>::get_date_ref() {
    return this->data;
}

template<class T>
void circular_T_node_t<T>::set_data(const T &data) {
    this->data = data;
}

template<class T>
void circular_T_node_t<T>::set_prev(circular_T_node_t<T> *node) {
    circular_base_node_t::set_prev(node);
}

template<class T>
void circular_T_node_t<T>::set_next(circular_T_node_t<T> *node) {
    circular_base_node_t::set_next(node);
}

template<class T>
circular_T_node_t<T> *circular_T_node_t<T>::get_prev() const {
    return static_cast<circular_T_node_t<T>*>(circular_base_node_t::prev);
}

template<class T>
circular_T_node_t<T> *circular_T_node_t<T>::get_next() const {
    return static_cast<circular_T_node_t<T>*>(circular_base_node_t::next);
}

template<class T>
void circular_T_node_t<T>::delete_node() {
    circular_base_node_t::delete_node();
}

template<class T>
void circular_T_node_t<T>::concat_prev(const T &data) {
    circular_base_node_t::concat_prev(new circular_T_node_t<T>(data));
}

template<class T>
void circular_T_node_t<T>::concat_next(const T &data) {
    circular_base_node_t::concat_next(new circular_T_node_t<T>(data));
}

#pragma endregion

#pragma region circular_T_list_t

template<class T>
class circular_T_list_t : public circular_base_list_t {
public:
    circular_T_node_t<T>* get_head() const;

    circular_T_node_t<T>* get_tail() const;

    circular_T_node_t<T>* get_prev(circular_T_node_t<T> *node) const;

    circular_T_node_t<T>* get_next(circular_T_node_t<T> *node) const;

    circular_T_list_t();

    circular_T_list_t(const circular_T_list_t& circular_T_list);

    bool operator==(const circular_T_list_t& circular_T_list) const;

    bool operator!=(const circular_T_list_t& circular_T_list) const;

    circular_T_list_t<T> operator=(const circular_T_list_t<T>& circular_T_list);

    void drop();

    circular_T_node_t<T>* begin() const;

    circular_T_node_t<T>* end() const;

    void add_at_head(const T& value);

    void add_at_tail(const T& value);

    void concat_prev(circular_T_node_t<T>* node, const T& data);

    void concat_next(circular_T_node_t<T>* node, const T& data);

    void delete_node(circular_T_node_t<T>* node);

    void for_each(void (*func)(circular_T_node_t<T>* node)) const;

    template<class T1>
    void for_each_tuple(const T1 tins, void (*func)(circular_T_node_t<T>* node, const T1 t)) const;

    void for_each_pair(void (*func)(circular_T_node_t<T>* node0, circular_T_node_t<T>* node1)) const;

    template<class T1>
    T1 for_each_iterate(T1 (*func)(circular_T_node_t<T>* node), T1 t = T1()) const;

    template<class T1>
    T1 for_each_pair_iterate(T1 (*func)(circular_T_node_t<T>* node, circular_T_node_t<T>* node1), T1 t = T1()) const;

    circular_T_node_t<T>* delete_node_get_prev(circular_T_node_t<T>* node);

    circular_T_node_t<T>* delete_node_get_next(circular_T_node_t<T>* node);

    circular_T_node_t<T>* delete_node_get_real_prev(circular_T_node_t<T>* node);

    circular_T_node_t<T>* delete_node_get_real_next(circular_T_node_t<T>* node);

    void print_reverse() const;

    void print() const;

    void print_reverse_list() const;

    std::vector<T> to_vector_reverse() const;

    std::vector<circular_T_node_t<T>*> to_node_vector_reverse() const;

    std::vector<T> to_vector() const;

    std::vector<circular_T_node_t<T>*> to_node_vector() const;

    std::vector<T> to_vector_multi_round(int round) const;
};

template <class T>
inline circular_T_node_t<T> *circular_T_list_t<T>::get_head() const {
    return static_cast<circular_T_node_t<T>*>(circular_base_list_t::head);
}

template <class T>
inline circular_T_node_t<T> *circular_T_list_t<T>::get_tail() const {
    return static_cast<circular_T_node_t<T>*>(circular_base_list_t::tail);
}

template <class T>
inline circular_T_node_t<T> *circular_T_list_t<T>::get_prev(circular_T_node_t<T> *node) const {
    return static_cast<circular_T_node_t<T>*>(circular_base_list_t::get_prev(node));
}

template <class T>
inline circular_T_node_t<T> *circular_T_list_t<T>::get_next(circular_T_node_t<T> *node) const {
    return static_cast<circular_T_node_t<T>*>(circular_base_list_t::get_next(node));
}

template <class T>
circular_T_list_t<T>::circular_T_list_t() : circular_base_list_t() {
}
template <class T>
inline circular_T_list_t<T>::circular_T_list_t(const circular_T_list_t &circular_T_list) {
    // std::cout << "copy contr\n";
    circular_T_node_t<T>* cur = circular_T_list.get_head()->get_next();
    while(cur != circular_T_list.tail) {
        this->add_at_tail(cur->get_data());
        cur = cur->get_next();
    }
}

template<class T>
bool circular_T_list_t<T>::operator==(const circular_T_list_t<T> &circular_T_list) const {
    if(this->get_length() != circular_T_list.get_length()) return false;
    auto this_node = this->get_head()->get_next();
    auto neighbor_node = circular_T_list.get_head()->get_next();
    for (int i = 0; i < this->length; ++i) {
        if(this_node->get_data() != neighbor_node->get_data()) return false;
        this_node = this_node->get_next();
        neighbor_node = neighbor_node->get_next();
    }
    return true;
}

template <class T>
inline bool circular_T_list_t<T>::operator!=(const circular_T_list_t &circular_T_list) const {
    return !this->operator==(circular_T_list);
}

template <class T>
inline circular_T_list_t<T> circular_T_list_t<T>::operator=(const circular_T_list_t<T> &circular_T_list) {
    // std::cout << "hi copy\n";
    while(this->begin() != this->get_tail()) {
        this->delete_node(this->begin());
    }
    circular_T_node_t<T>* cur = circular_T_list.begin();
    while(cur != circular_T_list.end()) {
        this->add_at_tail(cur->get_data());
        cur = cur->get_next();
    }
    return *this;
}

template <class T>
inline void circular_T_list_t<T>::drop() {
    circular_base_list_t::drop();
}

template <class T>
inline circular_T_node_t<T> *circular_T_list_t<T>::begin() const {
    return static_cast<circular_T_node_t<T>*>(circular_base_list_t::begin());
}

template <class T>
inline circular_T_node_t<T> *circular_T_list_t<T>::end() const {
    return static_cast<circular_T_node_t<T>*>(circular_base_list_t::end());
}

template<class T>
void circular_T_list_t<T>::add_at_head(const T &value) {
    circular_base_list_t::add_at_head(new circular_T_node_t<T>(value));
}

template<class T>
void circular_T_list_t<T>::add_at_tail(const T &value) {
    circular_base_list_t::add_at_tail(new circular_T_node_t<T>(value));
}

template<class T>
void circular_T_list_t<T>::concat_prev(circular_T_node_t<T> *node, const T &data) {
    circular_base_list_t::concat_prev(node, new circular_T_node_t<T>(data));
}

template<class T>
void circular_T_list_t<T>::concat_next(circular_T_node_t<T> *node, const T &data) {
    circular_base_list_t::concat_next(node, new circular_T_node_t<T>(data));
}

template<class T>
void circular_T_list_t<T>::delete_node(circular_T_node_t<T> *node) {
    circular_base_list_t::delete_node(node);
}

template <class T>
inline void circular_T_list_t<T>::for_each(void (*func)(circular_T_node_t<T> *node)) const {
    circular_T_node_t<T>* cur = this->get_head();
    while(cur = cur->get_next(), cur != this->get_tail()) {
        func(cur);
    }
}

template <class T>
inline void circular_T_list_t<T>::for_each_pair(void (*func)(circular_T_node_t<T> *node0, circular_T_node_t<T> *node1)) const {
    circular_T_node_t<T>* cur = this->get_head();
    while(cur = cur->get_next(), cur->get_next() != this->get_tail()) {
        func(cur, cur->get_next());
    }
    func(this->get_tail()->get_prev(), this->begin());
}

template<class T>
circular_T_node_t<T> *circular_T_list_t<T>::delete_node_get_prev(circular_T_node_t<T> *node) {
    return static_cast<circular_T_node_t<T>*>(circular_base_list_t::delete_node_get_prev(node));
}

template<class T>
circular_T_node_t<T>* circular_T_list_t<T>::delete_node_get_next(circular_T_node_t<T>* node) {
    return static_cast<circular_T_node_t<T>*>(circular_base_list_t::delete_node_get_next(node));
}

template <class T>
inline circular_T_node_t<T> *circular_T_list_t<T>::delete_node_get_real_prev(circular_T_node_t<T> *node) {
    return static_cast<circular_T_node_t<T>*>(circular_base_list_t::delete_node_get_real_prev(node));
}

template <class T>
inline circular_T_node_t<T> *circular_T_list_t<T>::delete_node_get_real_next(circular_T_node_t<T> *node) {
    return static_cast<circular_T_node_t<T>*>(circular_base_list_t::delete_node_get_real_next(node));
}

template<class T>
void circular_T_list_t<T>::print_reverse() const {
    circular_T_node_t<T>* cur = this->get_tail()->get_prev();
    while(cur != this->get_head()) {
        std::cout << cur->get_data() << " <- ";
        cur = cur->get_next();
    }
    std::cout << "\n";
}

template<class T>
void circular_T_list_t<T>::print() const {
    circular_T_node_t<T>* cur = this->get_head()->get_next();
    while(cur != this->get_tail()) {
        std::cout << cur->get_data() << " -> ";
        cur = cur->get_next();
    }
    std::cout << "\n";
}

template<class T>
void circular_T_list_t<T>::print_reverse_list() const {
    auto node = this->get_root()->get_prev();
    circular_T_node_t<T>* cur = this->get_tail()->get_prev();
    while(cur->get_prev() != this->get_head()){
        std::cout << cur->get_prev()->get_data() << " next -> " << cur->get_data() << "\n";
        cur = cur->get_prev();
    }
}

template<class T>
std::vector<T> circular_T_list_t<T>::to_vector_reverse() const {
    std::vector<T> vec;
    circular_T_node_t<T>* cur = this->get_tail()->get_prev();
    while(cur != this->get_head()) {
        vec.push_back(cur->get_data());
        cur = cur->get_prev();
    }
    return vec;
}

template<class T>
std::vector<circular_T_node_t<T> *> circular_T_list_t<T>::to_node_vector_reverse() const {
    std::vector<circular_T_node_t<T>*> vec;
    circular_T_node_t<T>* cur = this->get_tail()->get_prev();
    while(cur != this->get_head()) {
        vec.push_back(cur);
        cur = cur->get_prev();
    }
    return vec;
}

template<class T>
std::vector<T> circular_T_list_t<T>::to_vector() const {
    std::vector<T> vec;
    circular_T_node_t<T>* cur = this->get_head()->get_next();
    while(cur != this->get_tail()) {
        vec.push_back(cur->get_data());
        cur = cur->get_next();
    }
    return vec;
}

template<class T>
std::vector<circular_T_node_t<T> *> circular_T_list_t<T>::to_node_vector() const {
    std::vector<circular_T_node_t<T> *> vec;
    circular_T_node_t<T>* cur = this->get_head()->get_next();
    while(cur != this->get_tail()) {
        vec.push_back(cur);
        cur = cur->get_next();
    }
    return vec;
}

template<class T>
std::vector<T> circular_T_list_t<T>::to_vector_multi_round(int round) const {
    std::vector<T> vec;
    for (int i = 0; i < round; ++i) {
        circular_T_node_t<T>* cur = this->get_head()->get_next();
        while(cur != this->get_tail()) {
            vec.push_back(cur->get_data());
            cur = cur->get_next();
        }
    }
    return vec;
}

template <class T>
template <class T1>
inline void circular_T_list_t<T>::for_each_tuple(const T1 tins, void (*func)(circular_T_node_t<T> *node, const T1 t)) const {
    circular_T_node_t<T>* cur = this->get_head();
    while(cur = cur->get_next(), cur != this->get_tail()) {
        func(cur, tins);
    }
}

template <class T>
template <class T1>
inline T1 circular_T_list_t<T>::for_each_iterate(T1 (*func)(circular_T_node_t<T> *node), T1 t) const {
    circular_T_node_t<T>* cur = this->get_head();
    while(cur = cur->get_next(), cur != this->get_tail()) {
        t += func(cur);
    }
    return t;
}

template <class T>
template <class T1>
inline T1 circular_T_list_t<T>::for_each_pair_iterate(T1 (*func)(circular_T_node_t<T> *node, circular_T_node_t<T> *node1), T1 t) const {
    circular_T_node_t<T>* cur = this->get_head();
    while(cur = cur->get_next(), cur->get_next() != this->get_tail()) {
        t += func(cur, cur->get_next());
    }
    t += func(this->get_tail()->get_prev(), this->begin());
    return t;
}
#pragma endregion

#endif //BOUNDINGLINE_CIRCULAR_T_LIST_T_H
