
#ifndef TESTCASE_CHANGER_T_H
#define TESTCASE_CHANGER_T_H

#include <string>
#include "static_data/mcnc/mcnc_module_t.h"

class testcase_changer_t {
     static mcnc_module_t& get_mcnc_module_byname(std::string name);
     static void add_module(mcnc_module_t module);
public:
     static void read();
     static void read_from_ntuplace();
     static void add_module(std::string module_name, mcnc_module_t::module_type_e type, rect_t rect);
     static void modify_module(std::string name, void(*func)(mcnc_module_t& module));
};

#endif //TESTCASE_CHANGER_T_H




// cc_11  W=     3146.000000 H=     1826.000000 Area=  5744596.000000
//           X=        0.000000 Y=     3652.000000
// cc_12  W=     3146.000000 H=     1826.000000 Area=  5744596.000000
//           X=        0.000000 Y=     1826.000000
// cc_13  W=     3146.000000 H=     1826.000000 Area=  5744596.000000
//           X=     3146.000000 Y=        0.000000
// cc_14  W=     3146.000000 H=     1826.000000 Area=  5744596.000000
//           X=        0.000000 Y=        0.000000
// cc_21  W=     3186.000000 H=     1832.000000 Area=  5836752.000000
//           X=     3186.000000 Y=     3146.000000
// cc_22  W=     3186.000000 H=     1832.000000 Area=  5836752.000000
//           X=        0.000000 Y=     5478.000000
// cc_23  W=     3186.000000 H=     1832.000000 Area=  5836752.000000
//           X=     3186.000000 Y=     6332.000000
// cc_24  W=     3186.000000 H=     1832.000000 Area=  5836752.000000
//           X=        0.000000 Y=     7310.000000
// clk  W=      826.000000 H=      286.000000 Area=   236236.000000
//      X=     3186.000000 Y=     9518.000000