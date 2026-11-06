//
// Created by 林士傑 on 2023/6/22.
//

#ifndef BOUNDINGLINE_UNIT_TESTER_H
#define BOUNDINGLINE_UNIT_TESTER_H

// #include "vec2d_t.h"
#include "utilities/line_t.h"
#include "utility"
#include "bounding_line_t.h"
#include "circular_base_list_t.h"
#include "circular_T_list_t.h"
#include "bounding_line_handler_t.h"
#include "bounding/puzzle_solver_t.h"
#include "plugin/visualizer_t.h"

class unit_tester {
    //region vec2d_t

    // bool vec2d_t_add0();

    // bool vec2d_t_add1();

    // bool vec2d_t_add_equal();

    //endregion

    //region line_t

    bool line_t_constructor0();

    bool line_t_constructor1();

    bool line_t_constructor2();

    bool line_t_constructor3();

    bool line_t_constructor4();

    bool line_t_constructor5();

    bool line_t_is_vertical0();

    bool line_t_is_vertical1();

    bool line_t_is_parallel0();

    bool line_t_is_parallel1();

    bool line_t_is_intersect0();

    bool line_t_is_intersect1();

    bool line_t_is_touch0();

    bool line_t_is_touch1();

    bool line_t_parallel_cut_reverse0();

    bool line_t_parallel_cut_reverse1();

    bool line_t_parallel_cut_reverse2();

    bool line_t_parallel_cut_reverse3();

    //endregion

    //region bounding_line_t

    bool bounding_line_t_merge0();

    bool bounding_line_t_merge1();

    bool bounding_line_t_merge2();

    bool bounding_line_t_merge3();

    bool bounding_line_t_merge4();

    bool bounding_line_t_merge5();

    bool bounding_line_t_merge6();

    bool bounding_line_t_merge7();

    bool bounding_line_t_merge8();

    bool bounding_line_t_merge9();

    bool bounding_line_t_merge10();

    bool bounding_line_t_merge11();

    //endregion

    //region circular_base_list_t

    bool circular_base_list_t_concat_next0();

    bool circular_base_list_t_concat_next1();

    bool circular_base_list_t_concat_next2();

    bool circular_base_list_t_concat_next3();

    bool circular_base_list_t_concat_next4();

    bool circular_base_list_t_concat_next5();

    bool circular_base_list_t_add_at_tail0();

    bool circular_base_list_t_add_at_tail1();

    bool circular_base_list_t_add_at_head0();

    bool circular_base_list_t_add_at_head1();

    bool circular_base_list_t_delete_node_get_next0();

    bool circular_base_list_t_delete_node_get_next1();

    bool circular_base_list_t_delete_node_get_next2();

    bool circular_base_list_t_delete_node_get_next3();

    bool circular_base_list_t_delete_node_get_next4();

    bool circular_base_list_t_delete_node_get_next5();

    bool circular_base_list_t_delete_node_get_next6();

    bool circular_base_list_t_to_vector_reverse0();

    bool circular_base_list_t_to_vector_reverse1();

    bool circular_base_list_t_to_vector_reverse2();

    bool circular_base_list_t_to_vector0();

    bool circular_base_list_t_to_vector1();

    bool circular_base_list_t_to_vector2();

    bool circular_base_list_t_to_vector_multi_round0();

    bool circular_base_list_t_to_vector_multi_round1();

    bool circular_base_list_t_to_vector_multi_round2();

    bool circular_base_list_t_memory_control();
    //endregion

    //region bounding_line_handler_t
    //endregion

public:
    void runner();

    void main();
};


#endif //BOUNDINGLINE_UNIT_TESTER_H
