//
// Created by 林士傑 on 2023/3/9.
//

#include <iostream>
#include "vec2d_t.h"

vec2d_t vec2d_t::default_position = vec2d_t(-1, -1);

vec2d_t::vec2d_t() : x(default_position.get_x()), y(default_position.get_y()) {}

vec2d_t::vec2d_t(const double &cons) : x(cons), y(cons) {}

vec2d_t::vec2d_t(const vec2d_t &vec2d) : x(vec2d.x), y(vec2d.y) {}

vec2d_t vec2d_t::operator=(const vec2d_t &vec2d) {
    this->x = vec2d.get_x();
    this->y = vec2d.get_y();
    return vec2d_t(vec2d);
}

vec2d_t::vec2d_t(const double& x, const double& y) : x(x), y(y) {}


void vec2d_t::set_x(const double &x) {
    this->x = x;
}

void vec2d_t::set_y(const double &y) {
    this->y = y;
}

void vec2d_t::set_xy(const double &x, const double &y) {
    this->x = x;
    this->y = y;
}

double vec2d_t::get_x() const {
    return x;
}

double vec2d_t::get_y() const {
    return y;
}

double vec2d_t::get_half_x() const {
    return get_x() / 2.0;
}

double vec2d_t::get_half_y() const {
    return get_y() / 2.0;
}

double vec2d_t::get_length() const {
    return sqrt(get_x() * get_x() + get_y() * get_y());
}

double vec2d_t::get_area() const {
    return get_x() * get_y();
}

vec2d_t vec2d_t::operator+(const vec2d_t &vec2D) const {
    return {this->get_x() + vec2D.get_x(), this->get_y() + vec2D.get_y()};
}

vec2d_t &vec2d_t::operator+=(const vec2d_t &vec2D) {
    this->x += vec2D.get_x();
    this->y += vec2D.get_y();
    return *this;
}

vec2d_t &vec2d_t::operator+=(const double &add) {
    this->x += add;
    this->y += add;
    return *this;
}

vec2d_t vec2d_t::add_const(const double &add) const {
    return {this->get_x() + add, this->get_y() + add};
}

vec2d_t vec2d_t::operator-(const vec2d_t &vec2D) const {
    return {this->get_x() - vec2D.get_x(), this->get_y() - vec2D.get_y()};
}

vec2d_t &vec2d_t::operator-=(const vec2d_t &vec2D) {
    this->x -= vec2D.get_x();
    this->y -= vec2D.get_y();
    return *this;
}

vec2d_t &vec2d_t::operator-=(const double &sub) {
    this->x -= sub;
    this->y -= sub;
    return *this;
}

vec2d_t vec2d_t::sub_const(const double &sub) const {
    return {this->get_x() - sub, this->get_y() - sub};
}

vec2d_t vec2d_t::operator*(const double& mul) const {
    return {this->get_x() * mul, this->get_y() * mul};
}

vec2d_t vec2d_t::normal_multi(const vec2d_t &vec2D) const {
    return {this->get_x() * vec2D.get_x(), this->get_y() * vec2D.get_y()};
}

vec2d_t vec2d_t::operator/(const double& div) const {
    return {this->get_x() / div, this->get_y() / div};
}

vec2d_t vec2d_t::normal_divide(const vec2d_t &vec2D) const {
    return {this->get_x() / vec2D.get_x(), this->get_y() / vec2D.get_y()};
}

bool vec2d_t::operator==(const vec2d_t &vec2D) const {
    return fabs(this->get_x() - vec2D.get_x()) <= 1E-6 && fabs(this->get_y() - vec2D.get_y()) <= 1E-6;
}

bool vec2d_t::operator!=(const vec2d_t &vec2D) const {
    return fabs(this->get_x() - vec2D.get_x()) > 1E-6 || fabs(this->get_y() - vec2D.get_y()) > 1E-6;
}

std::ostream &operator<<(std::ostream &os, const vec2d_t &vec) {
    os << "{" << vec.x << ", " << vec.y << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os, const vec2d_t &&vec) {
    os << "{" << vec.x << ", " << vec.y << "}";
    return os;
}
