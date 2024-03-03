
//
// Created by 林士傑 on 2023/3/9.
//

#ifndef INC_2023PD_VEC2D_T_H
#define INC_2023PD_VEC2D_T_H

#include <cmath>
#include <ostream>

class vec2d_t {
    friend std::ostream& operator<<(std::ostream& os, const vec2d_t& vec);
    friend std::ostream& operator<<(std::ostream& os, const vec2d_t&& vec);
    double x, y;
public:
    static vec2d_t default_position;

    vec2d_t();

    explicit vec2d_t(const double& cons);

    vec2d_t(const vec2d_t&);

    vec2d_t operator=(const vec2d_t&);

    vec2d_t(const double& x, const double& y);

    void set_x(const double& x);

    void set_y(const double& y);

    void set_xy(const double& x, const double& y);

    double get_x() const;

    double get_y() const;

    double get_half_x() const;

    double get_half_y() const;

    double get_length() const;

    double get_area() const;

    vec2d_t operator+(const vec2d_t& vec2D) const;

    vec2d_t& operator+=(const vec2d_t& vec2D);

    vec2d_t& operator+=(const double& add);

    vec2d_t add_const(const double& add) const;

    vec2d_t operator-(const vec2d_t& vec2D) const;

    vec2d_t& operator-=(const vec2d_t& vec2D);

    vec2d_t& operator-=(const double& sub);

    vec2d_t sub_const(const double& sub) const;

    vec2d_t operator*(const double& mul) const;

    vec2d_t normal_multi(const vec2d_t& vec2D) const;

    vec2d_t operator/(const double& div) const;

    vec2d_t normal_divide(const vec2d_t& vec2D) const;

    bool operator==(const vec2d_t& vec2D) const;

    bool operator!=(const vec2d_t& vec2D) const;
};

std::ostream& operator<<(std::ostream& os, const vec2d_t& vec);
std::ostream& operator<<(std::ostream& os, const vec2d_t&& vec);

#endif //INC_2023PD_VEC2D_T_H
