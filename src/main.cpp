#include <iostream>
#include <fstream>
#include <iomanip>

#include "static_data/chip_t.h"
#include "process/solver_t.h"
#include "process/floorplan_t.h"
#include "output_data/output_handler_t.h"
#include "output_data/output_utility_t.h"


int main(/*arg*/){
    std::string input_file_name = "";
    std::string output_file_name = "";

    input_file_name = "../../testcase/Case01.txt";

    chip_t::file_input(input_file_name);//fstream

    solver_t solver;
    solver.run();

    floorplan_t fp = solver.get_best_fp();
    output_handler_t::set_FP(fp);

    std::fstream output_file;
    output_file.open(output_file_name, std::ios::out | std::ios::trunc);

    output_file << std::setiosflags(std::ios_base::fixed) << std::setprecision(1);

    output_file << "HPWL " << output_handler_t::HPWL() << std::endl;

    output_file << "SOFTMODULE " << chip_t::get_soft_modules().size();

    for (output_utility_t& output_utility : output_handler_t::to_real()) {
        output_file << output_utility.to_string();
    }
    output_file.close();

    fgetc(stdin);
    std::cout << "press any key to end the program" << std::endl;
}
