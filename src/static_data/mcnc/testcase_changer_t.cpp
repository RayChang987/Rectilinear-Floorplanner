#include "testcase_changer_t.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include "utilities/rect_t.h"
#include "static_data/chip_t.h"
#include "static_data/mcnc/yal_reader_t.h"
#include "static_data/mcnc/mcnc_module_t.h"

mcnc_module_t &testcase_changer_t::get_mcnc_module_byname(std::string name) {
    for(auto& m : chip_t::yal_reader.modules) {
        if(m.name == name) {
            return m;
        }
    }
    mcnc_module_t m;
    return m;
}

void testcase_changer_t::read()
{
    std::string final_result = "";
    std::stringstream result;
    std::string temp;
    while(std::cin >> temp) {
        double w, h, x, y;
        result.str("");
        result << "MODULE " + temp + ";\n TYPE PAD;\n DIMENSIONS ";
        std::cin >> temp >> w >> temp >> h >> temp >> x >> temp >> x >> temp >> y;
        auto p = rect_t({x, y}, {w, h}).to_bounding_point();
        result << std::fixed << std::setprecision(0) << p[1].get_x() << " " << p[1].get_y() << " ";
        result << p[2].get_x() << " " << p[2].get_y() << " ";
        result << p[3].get_x() << " " << p[3].get_y() << " ";
        result << p[0].get_x() << " " << p[0].get_y() << ";\n";

        final_result += result.str() + "\n";
    }
    std::cout << final_result;
}

void testcase_changer_t::read_from_ntuplace() {
    std::string temp;
    while(std::cin >> temp) {
        double w, h, x, y;
        std::string name = temp;
        std::cin >> temp >> w >> temp >> h >> temp >> x >> temp >> x >> temp >> y;
        testcase_changer_t::get_mcnc_module_byname(name).rect = rect_t({x, y}, {w, h});
    }
}

void testcase_changer_t::add_module(mcnc_module_t module) {
    chip_t::yal_reader.modules.insert(chip_t::yal_reader.modules.end() - 1, module);

    module_network_t net = module_network_t();
    net.instance_name = "C_" + std::to_string(chip_t::yal_reader.modules.back().network.size());
    net.module_name = module.name;
    net.signals.push_back("");
    chip_t::yal_reader.modules.back().network.push_back(net);
}

void testcase_changer_t::add_module(std::string module_name, mcnc_module_t::module_type_e type, rect_t rect) {
    mcnc_module_t module;
    module.name = module_name;
    module.module_type = type;
    module.rect = rect;
    testcase_changer_t::add_module(module);
}

void testcase_changer_t::modify_module(std::string name, void (*func)(mcnc_module_t &module)) {
    func(testcase_changer_t::get_mcnc_module_byname(name));
}
