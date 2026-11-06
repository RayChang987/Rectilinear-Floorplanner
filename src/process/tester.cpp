#include "tester.h"

#include "process/case_table_t.h"
#include "process/sequence_pair/sequence_pair_t.h"
#include "static_data/io_handler_t.h"

tester_t::tester_t() {
    sequence_pair_t::init();  // class initialization
}

void tester_t::test_sp() {
    const int case_id = chip_t::get_similar_case_num();
    if (case_id == -1) {
        logger_t::error(
            {"This is not a default test case, can't load a predetermined "
             "sequence pair. "});
        return;
    }
    case_table_t case_table;
    sequence_pair_t SP;
    SP.v_sequence = case_table.cases[case_id][0];
    SP.h_sequence = case_table.cases[case_id][1];
    SP.set_is_in_seq(1);
    // Either a rectilinear or a rectangular floorplan
    if (!SP.find_position_allow_illegal_process()) {
        logger_t::error({"Unable to initialize"});
        return;
    }
    SP.get_wirelength();
    SP.to_rectilinear();
    const size_t illegal_n = SP.get_illegal_modules().size();
    if (illegal_n > 0) {
        const bool legal = SP.legalize_rectilinear();
        logger_t::info({"Repaired " + std::to_string(illegal_n) + " illegal module(s): " +
                        std::to_string(SP.get_illegal_modules().size()) + " left" +
                        (legal ? "" : " (could not be repaired)")});
    }
    SP.plot_rectilinear();
    SP.check_rectilinear_result(true);
    SP.print_inline();
}
