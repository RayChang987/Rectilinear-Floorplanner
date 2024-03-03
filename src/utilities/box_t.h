//
// Created by 林士傑 on 2023/4/28.
//

#ifndef ICCAD2023PD_BOX_T_H
#define ICCAD2023PD_BOX_T_H

class rect_t;

class box_t {
public:
    virtual const rect_t& get_bounding_rect() const = 0;
};

#endif //ICCAD2023PD_BOX_T_H
