#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

#include "plugin/argumentParser.h"
#include "process/functional/random_helper.h"
#include "process/solver_t.h"
#include "process/tester.h"
#include "static_data/chip_t.h"
#include "static_data/io_handler_t.h"

namespace {

void usage(int /*argc*/, char* argv[]) {
    std::cerr
        << "Usage: " << argv[0] << " -i <input> [-o <output dir>] [-s <seed>] [-t]\n"
        << "  -i <input>       input file (ICCAD 2023 Problem D format)\n"
        << "  -o <output dir>  directory of check.txt (default: .)\n"
        << "  -s <seed>        random seed (default: random)\n"
        << "  -t               run the predetermined sequence pair of a known\n"
        << "                   test case instead of the solver\n";
}

}  // namespace

int main(int argc, char** argv) {
    argumentParser argument;
    try {
        argument = argumentParser(argc, argv, usage);
    } catch (std::exception& e) {
        logger_t::error({e.what()});
        return 1;
    }
    if (!argument.has("i")) {
        usage(argc, argv);
        return 1;
    }

    io_handler_t::input_file_name = argument.get("i");
    chip_t::file_input(io_handler_t::input_file_name,
                       chip_t::file_type_t::iccad_pd);
    if (argument.has("o")) {
        io_handler_t::output_checker_file_name = argument.get("o");
    }
    if (argument.has("s")) {
        random_helper::set_seed(
            static_cast<uint32_t>(std::stoul(argument.get("s"))));
    } else {
        random_helper::set_seed();
    }
    logger_t::info({"Random seed: " + std::to_string(random_helper::get_seed())});

    if (argument.has("t")) {
        tester_t tester;
        tester.test_sp();
        logger_t::action({"Press any key to end the program"});
        fgetc(stdin);
    } else {
        solver_t solver;
        solver.run();
    }
    return 0;
}
