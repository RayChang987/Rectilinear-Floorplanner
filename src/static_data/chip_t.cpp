//
// Created by 林士傑 on 2023/3/8.
//

#include "chip_t.h"

#include <algorithm>
#include <exception>
#include <iostream>
#include <map>
#include <set>
#include <sstream>

#include "fixed_module_t.h"
#include "mcnc/yal_reader_t.h"
#include "mcnc_old/mcnc_old_reader_t.h"
#include "soft_module_t.h"

std::string chip_t::file_name;
uint32_t chip_t::width, chip_t::height;
int chip_t::width_bias, chip_t::height_bias;
uint32_t chip_t::softCount, chip_t::fixedCount;
size_t chip_t::total_modules_count = 0;
std::unordered_map<std::string, size_t> chip_t::moduleNameToIndex;
std::vector<module_t*> chip_t::modules;
std::vector<soft_module_t*> chip_t::soft_modules;
std::vector<fixed_module_t*> chip_t::fixed_modules;
std::vector<std::vector<uint_fast32_t>> chip_t::connectionTable;
std::vector<multi_net_t*> chip_t::multi_nets;
yal_reader_t chip_t::yal_reader;
mcnc_old_reader_t chip_t::mcnc_old_reader;
double chip_t::module_minimum_length = 1e100;
int chip_t::similar_case_num = -1;
std::unordered_map<module_t*, int> module_to_id_m;

std::string chip_t::set_file_name(std::string file_name) {
    return (chip_t::file_name = file_name);
}

std::string chip_t::get_file_name() { return chip_t::file_name; }

void chip_t::file_input(std::string fileName, file_type_t file_type,
                        std::string fileName1, std::string fileName2) {
    if (file_type == chip_t::file_type_t::iccad_pd) {
        pd_file_input(fileName);
    } else if (file_type == chip_t::file_type_t::mcnc) {
        mcnc_file_input(fileName);
    } else if (file_type == chip_t::file_type_t::old_mcnc) {
        chip_t::mcnc_old_file_input(fileName, fileName1, fileName2);
    }
}

void chip_t::file_save(std::string fileName, file_type_t file_type,
                       std::string fileName1, std::string fileName2) {
    if (file_type == chip_t::file_type_t::iccad_pd) {
        std::cout << "iccad file save not supported yet.\n";
    } else if (file_type == chip_t::file_type_t::mcnc) {
        chip_t::yal_reader.file_saver(fileName);
    } else if (file_type == chip_t::file_type_t::old_mcnc) {
        std::cout << "old_mcnc file save not supported yet.\n";
    }
}

void chip_t::mcnc_file_input(std::string fileName) {
    std::fstream file;
    file.open(fileName);

    if (file.fail()) {
        file.close();
        std::cout << "failed to open \"" << fileName << "\"" << std::endl;
        return;
    }

    yal_reader.file_input(file);

    // soft_module
    for (int i = 0; i < yal_reader.modules.size() - 1; ++i) {
        if (yal_reader.modules[i].module_type ==
            mcnc_module_t::module_type_e::GENERAL) {
            soft_module_t* soft_module = new soft_module_t();
            soft_module->name = yal_reader.modules[i].name;
            soft_module->rect = yal_reader.modules[i].rect;
            soft_module->minArea = yal_reader.modules[i].rect.get_area();
            for (auto pin : yal_reader.modules[i].signals) {
                pin_t* soft_pin = new pin_t();
                soft_pin->name = pin.name;
                soft_pin->module_index = i;
                soft_pin->belong_module = soft_module;
                soft_pin->relative_position =
                    pin.position - soft_module->get_left_lower();
                soft_module->pins.push_back(soft_pin);
            }
            soft_module->rect =
                rect_t(vec2d_t(0, 0), soft_module->rect.get_size());
            chip_t::moduleNameToIndex[soft_module->getName()] =
                chip_t::modules.size();
            chip_t::softCount++;
            chip_t::soft_modules.push_back(soft_module);
            chip_t::modules.push_back(soft_module);
        } else if (yal_reader.modules[i].module_type ==
                   mcnc_module_t::module_type_e::PAD) {
            // fix
        } else {
            std::cout << yal_reader.modules[i].module_type << " : ";
            std::cout << "ERROROROROROROR" << __FILE__ << " " << __LINE__
                      << "\n";
        }
    }

    // fix_module
    for (int i = 0; i < yal_reader.modules.size() - 1; ++i) {
        if (yal_reader.modules[i].module_type ==
            mcnc_module_t::module_type_e::PAD) {
            fixed_module_t* fix_module = new fixed_module_t();
            fix_module->name = yal_reader.modules[i].name;
            fix_module->xCoord =
                yal_reader.modules[i].rect.get_left_lower().get_x();
            fix_module->yCoord =
                yal_reader.modules[i].rect.get_left_lower().get_y();
            fix_module->width = yal_reader.modules[i].rect.get_size().get_x();
            fix_module->height = yal_reader.modules[i].rect.get_size().get_y();
            fix_module->rect = new rect_t(yal_reader.modules[i].rect);
            for (auto pin : yal_reader.modules[i].signals) {
                pin_t* soft_pin = new pin_t();
                soft_pin->name = pin.name;
                soft_pin->module_index = i;
                soft_pin->belong_module = fix_module;
                soft_pin->relative_position =
                    pin.position - fix_module->get_left_lower();
                fix_module->pins.push_back(soft_pin);
            }
            chip_t::moduleNameToIndex[fix_module->getName()] =
                chip_t::modules.size();
            chip_t::fixedCount++;
            chip_t::fixed_modules.push_back(fix_module);
            chip_t::modules.push_back(fix_module);
        } else if (yal_reader.modules[i].module_type ==
                   mcnc_module_t::module_type_e::GENERAL) {
            // soft
        } else {
            std::cout << yal_reader.modules[i].module_type << " : ";
            std::cout << "ERROROROROROROR" << __FILE__ << " " << __LINE__
                      << "\n";
        }
    }

    // board
    auto chip = yal_reader.modules.back();
    chip_t::width = chip.rect.get_size().get_x();
    chip_t::height = chip.rect.get_size().get_y();
    chip_t::width_bias = chip.rect.get_left_lower().get_x();
    chip_t::height_bias = chip.rect.get_left_lower().get_y();
    fixed_module_t* fixed_module = new fixed_module_t();
    fixed_module->rect = new rect_t(vec2d_t(0, 0), vec2d_t(0, 0));
    for (auto pin : chip.signals) {
        pin_t* fix_pin = new pin_t();
        fix_pin->name = pin.name;
        fix_pin->module_index = yal_reader.modules.size() - 1;
        fix_pin->belong_module = fixed_module;
        fix_pin->relative_position =
            pin.position - fixed_module->get_left_lower();
        fixed_module->pins.push_back(fix_pin);
    }
    chip_t::moduleNameToIndex[fixed_module->getName()] =
        yal_reader.modules.size() - 1;
    chip_t::fixed_modules.push_back(fixed_module);
    chip_t::modules.push_back(fixed_module);

    chip_t::total_modules_count =
        chip_t::get_soft_modules().size() + chip_t::get_fixed_modules().size();

    // connection
    std::map<std::string, std::set<pin_t*>> all_nets;
    for (int i = 0; i < chip.network.size(); ++i) {
        for (int j = 0; j < chip.network[i].signals.size(); ++j) {
            if (chip.network[i].signals[j] == "") continue;
            all_nets[chip.network[i].signals[j]].insert(
                chip_t::modules[chip_t::moduleNameToIndex.at(
                                    chip.network[i].module_name)]
                    ->pins[j]);
        }
    }
    for (auto net : all_nets) {
        multi_net_t* multi_net = new multi_net_t();
        multi_net->name = net.first;
        multi_net->pins =
            std::vector<pin_t*>(net.second.begin(), net.second.end());
        for (auto pin : multi_net->pins) {
            pin->connect_net = multi_net;
        }
        chip_t::multi_nets.push_back(multi_net);
    }

    if (chip_t::file_name == "ami33.yal") {
        chip_t::similar_case_num = 10;
    } else if (chip_t::file_name == "ami49.yal") {
        chip_t::similar_case_num = 11;
    } else if (chip_t::file_name == "apte.yal") {
        chip_t::similar_case_num = 12;
    } else if (chip_t::file_name == "hp.yal") {
        chip_t::similar_case_num = 13;
    } else if (chip_t::file_name == "xerox.yal") {
        chip_t::similar_case_num = 14;
    }
}

void chip_t::mcnc_old_file_input(std::string blocks_file_name,
                                 std::string nets_file_name,
                                 std::string fixeds_file_name) {
    std::fstream blocks_file;
    blocks_file.open(blocks_file_name);

    if (blocks_file.fail()) {
        blocks_file.close();
        std::cout << "failed to open \"" << blocks_file_name << "\""
                  << std::endl;
        return;
    }

    std::fstream nets_file;
    nets_file.open(nets_file_name);

    if (nets_file.fail()) {
        nets_file.close();
        std::cout << "failed to open \"" << nets_file_name << "\"" << std::endl;
        return;
    }

    std::fstream fixeds_file;
    fixeds_file.open(fixeds_file_name);

    if (fixeds_file.fail()) {
        fixeds_file.close();
        std::cout << "failed to open \"" << fixeds_file_name << "\""
                  << std::endl;
        return;
    }

    chip_t::mcnc_old_reader.file_input(blocks_file, nets_file, fixeds_file);
    // mcnc_old_reader_t mor;
    // mor.file_input(blocks_file, nets_file);

    // soft_module
    for (int i = 0; i < chip_t::mcnc_old_reader.modules.size(); ++i) {
        soft_module_t* soft_module = new soft_module_t();
        soft_module->name = chip_t::mcnc_old_reader.modules[i]->name;
        soft_module->minArea = chip_t::mcnc_old_reader.modules[i]->min_area;
        for (auto pin : chip_t::mcnc_old_reader.modules[i]->pins) {
            pin_t* soft_pin = new pin_t();
            soft_pin->name = pin;
            soft_pin->module_index = i;
            soft_pin->belong_module = soft_module;
            soft_pin->relative_position = {0, 0};
            soft_module->pins.push_back(soft_pin);
        }
        chip_t::moduleNameToIndex[soft_module->getName()] =
            chip_t::modules.size();
        chip_t::softCount++;
        chip_t::soft_modules.push_back(soft_module);
        chip_t::modules.push_back(soft_module);
    }

    // fix_module
    for (int i = 0; i < chip_t::mcnc_old_reader.fix_modules.size(); ++i) {
        fixed_module_t* fix_module = new fixed_module_t();
        fix_module->name = chip_t::mcnc_old_reader.fix_modules[i]->name;
        fix_module->xCoord = chip_t::mcnc_old_reader.fix_modules[i]
                                 ->rect.get_left_lower()
                                 .get_x();
        fix_module->yCoord = chip_t::mcnc_old_reader.fix_modules[i]
                                 ->rect.get_left_lower()
                                 .get_y();
        fix_module->width =
            chip_t::mcnc_old_reader.fix_modules[i]->rect.get_size().get_x();
        fix_module->height =
            chip_t::mcnc_old_reader.fix_modules[i]->rect.get_size().get_y();
        fix_module->rect =
            new rect_t(chip_t::mcnc_old_reader.fix_modules[i]->rect);
        for (auto pin : chip_t::mcnc_old_reader.fix_modules[i]->pins) {
            pin_t* fix_pin = new pin_t();
            fix_pin->name = pin;
            fix_pin->module_index = i;
            fix_pin->belong_module = fix_module;
            fix_pin->relative_position = {0, 0};
            fix_module->pins.push_back(fix_pin);
        }
        chip_t::moduleNameToIndex[fix_module->getName()] =
            chip_t::modules.size();
        chip_t::fixedCount++;
        chip_t::fixed_modules.push_back(fix_module);
        chip_t::modules.push_back(fix_module);
    }

    chip_t::total_modules_count =
        chip_t::get_soft_modules().size() + chip_t::get_fixed_modules().size();

    // connection
    int netI = 0;
    std::map<std::string, std::set<pin_t*>> all_nets;
    for (auto connection : chip_t::mcnc_old_reader.connections) {
        std::string netName = "N" + std::to_string(netI++);
        for (auto modules : connection->modules) {
            all_nets[netName].insert(
                chip_t::modules[chip_t::moduleNameToIndex.at(modules->name)]
                    ->pins[0]);
        }
    }
    for (auto net : all_nets) {
        multi_net_t* multi_net = new multi_net_t();
        multi_net->name = net.first;
        multi_net->pins =
            std::vector<pin_t*>(net.second.begin(), net.second.end());
        for (auto pin : multi_net->pins) {
            pin->connect_net = multi_net;
        }
        chip_t::multi_nets.push_back(multi_net);
    }
    chip_t::width = chip_t::mcnc_old_reader.chipw;
    chip_t::height = chip_t::mcnc_old_reader.chiph;

    blocks_file.close();
    nets_file.close();

    if (chip_t::file_name == "ami33") {
        chip_t::similar_case_num = 15;
    } else if (chip_t::file_name == "ami49") {
        chip_t::similar_case_num = 16;
    } else if (chip_t::file_name == "apte") {
        chip_t::similar_case_num = 17;
    } else if (chip_t::file_name == "hp") {
        chip_t::similar_case_num = 18;
    } else if (chip_t::file_name == "xerox") {
        chip_t::similar_case_num = 19;
    }
}

void chip_t::pd_file_input(std::string fileName) {
    std::fstream file;
    file.open(fileName);

    if (file.fail()) {
        file.close();
        std::cout << "failed to open \"" << fileName << "\"" << std::endl;
        return;
    }
    int iTemp;
    std::string temp;

    while (file >> temp) {
        if (temp == "CHIP") {
            file >> chip_t::width >> chip_t::height;
        } else if (temp == "SOFTMODULE") {
            file >> chip_t::softCount;
            chip_t::total_modules_count += chip_t::softCount;
            chip_t::modules.resize(chip_t::softCount);
            chip_t::soft_modules.resize(chip_t::softCount);
            for (int i = 0; i < chip_t::softCount; ++i) {
                chip_t::soft_modules[i] = soft_module_t::fileInput(file);
                chip_t::modules[i] = chip_t::soft_modules[i];
                chip_t::moduleNameToIndex[chip_t::modules[i]->getName()] = i;
                chip_t::module_minimum_length = std::min(
                    chip_t::module_minimum_length,
                    ceil(sqrt(chip_t::soft_modules[i]->get_area() / 2)));

                pin_t* pin = new pin_t();
                pin->belong_module = chip_t::soft_modules[i];
                pin->module_index = i;
                pin->name = chip_t::soft_modules[i]->getName() + "_pin";
                pin->relative_position = vec2d_t(0, 0);
                chip_t::soft_modules[i]->pins.push_back(pin);
            }
        } else if (temp == "FIXEDMODULE") {
            file >> chip_t::fixedCount;
            chip_t::total_modules_count += chip_t::fixedCount;
            chip_t::modules.resize(chip_t::softCount + chip_t::fixedCount);
            chip_t::fixed_modules.resize(chip_t::fixedCount);
            for (int i = chip_t::softCount;
                 i < chip_t::softCount + chip_t::fixedCount; ++i) {
                chip_t::fixed_modules[i - chip_t::softCount] =
                    fixed_module_t::fileInput(file);
                chip_t::modules[i] =
                    chip_t::fixed_modules[i - chip_t::softCount];
                chip_t::moduleNameToIndex[chip_t::modules[i]->getName()] = i;

                pin_t* pin = new pin_t();
                pin->belong_module =
                    chip_t::fixed_modules[i - chip_t::softCount];
                pin->module_index = i;
                pin->name =
                    chip_t::fixed_modules[i - chip_t::softCount]->getName() +
                    "_pin";
                pin->relative_position = vec2d_t(0, 0);
                chip_t::fixed_modules[i - chip_t::softCount]->pins.push_back(
                    pin);
            }
        } else if (temp == "CONNECTION") {
            int connectionCount, index0, index1, nodeCount;
            file >> connectionCount;
            chip_t::connectionTable = std::vector<std::vector<uint_fast32_t>>(
                chip_t::softCount + chip_t::fixedCount,
                std::vector<uint_fast32_t>(
                    chip_t::softCount + chip_t::fixedCount, 0));
            for (int i = 0; i < connectionCount; ++i) {
                multi_net_t* multi_net = new multi_net_t();
                file >> nodeCount;
                for (int i = 0; i < nodeCount; ++i) {
                    file >> temp;
                    index0 = chip_t::moduleNameToIndex[temp];
                    multi_net->pins.push_back(
                        chip_t::get_modules()[index0]->pins[0]);
                    chip_t::get_modules()[index0]->pins[0]->connect_net =
                        multi_net;
                }
                file >> iTemp;
                multi_net->weight = iTemp;
                chip_t::multi_nets.push_back(multi_net);
            }

        } else {
            throw std::exception();
        }
    }

    if (similar_chip_t::case1()) {
        chip_t::similar_case_num = 0;
    } else if (similar_chip_t::case2()) {
        chip_t::similar_case_num = 1;
    } else if (similar_chip_t::case3()) {
        chip_t::similar_case_num = 2;
    } else if (similar_chip_t::case4()) {
        chip_t::similar_case_num = 3;
    } else if (similar_chip_t::case5()) {
        chip_t::similar_case_num = 4;
    } else if (similar_chip_t::case6()) {
        chip_t::similar_case_num = 5;
    } else if (similar_chip_t::case7()) {
        chip_t::similar_case_num = 6;
    } else if (similar_chip_t::case8()) {
        chip_t::similar_case_num = 7;
    } else if (similar_chip_t::case9()) {
        chip_t::similar_case_num = 8;
    } else if (similar_chip_t::case10()) {
        chip_t::similar_case_num = 9;
    }
}

int chip_t::get_similar_case_num() { return chip_t::similar_case_num; }

const size_t chip_t::get_total_module_n() {
    return chip_t::total_modules_count;
}

const uint32_t chip_t::get_width() { return chip_t::width; }

const uint32_t chip_t::get_height() { return chip_t::height; }

const std::vector<module_t*>& chip_t::get_modules() { return chip_t::modules; }

const std::vector<soft_module_t*>& chip_t::get_soft_modules() {
    return chip_t::soft_modules;
}

const std::vector<fixed_module_t*>& chip_t::get_fixed_modules() {
    return chip_t::fixed_modules;
}

const std::vector<std::vector<uint_fast32_t>>& chip_t::get_connection_table() {
    return chip_t::connectionTable;
}

const std::vector<multi_net_t*>& chip_t::get_multi_nets() {
    return chip_t::multi_nets;
}

const std::unordered_map<std::string, size_t>&
chip_t::get_name_to_index_mapping() {
    return chip_t::moduleNameToIndex;
}

const double& chip_t::get_module_minimum_length() {
    return chip_t::module_minimum_length;
}

void chip_t::decorate_serialize_testcase() {
    std::cout << "auto width = chip_t::width;\n";
    std::cout << "auto height = chip_t::height;\n";
    std::cout << "auto softCount = chip_t::softCount;\n";
    std::cout << "auto fixedCount = chip_t::fixedCount;\n";
    std::cout << "auto total_modules_count = chip_t::total_modules_count;\n";
    std::cout << "auto modules = chip_t::modules.size();\n";
    std::cout << "auto soft_modules = chip_t::soft_modules.size();\n";
    std::cout << "auto fixed_modules = chip_t::fixed_modules.size();\n";

    std::cout << "std::vector<std::string> soft_name_vec = {";
    for (int i = 0; i < chip_t::soft_modules.size(); ++i) {
        if (i != 0) std::cout << ", ";
        std::cout << "\"" << chip_t::soft_modules[i]->getName() << "\"";
    }
    std::cout << "};\n";

    std::cout << "std::vector<int> soft_area_vec = {";
    for (int i = 0; i < chip_t::soft_modules.size(); ++i) {
        if (i != 0) std::cout << ", ";
        std::cout << chip_t::soft_modules[i]->get_area();
    }
    std::cout << "};\n";

    std::cout << "std::vector<std::string> fix_name_vec = {";
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i) {
        if (i != 0) std::cout << ", ";
        std::cout << "\"" << chip_t::fixed_modules[i]->getName() << "\"";
    }
    std::cout << "};\n";

    std::cout << "std::vector<std::vector<int>> fix_coord_vec = {";
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i) {
        if (i != 0) std::cout << ", ";
        std::cout << "{";
        std::cout << chip_t::fixed_modules[i]->get_left_lower().get_x() << ", "
                  << chip_t::fixed_modules[i]->get_left_lower().get_y() << ", "
                  << chip_t::fixed_modules[i]->get_size().get_x() << ", "
                  << chip_t::fixed_modules[i]->get_size().get_y();
        std::cout << "}";
    }
    std::cout << "};\n";

    std::cout << "if( width != " << chip_t::width << " ) return false;\n"
              << "if( height != " << chip_t::height << " ) return false;\n"
              << "if( softCount != " << chip_t::softCount
              << " ) return false;\n"
              << "if( fixedCount != " << chip_t::fixedCount
              << " ) return false;\n"
              << "if( total_modules_count != " << chip_t::total_modules_count
              << " ) return false;\n"
              << "if( modules != " << chip_t::modules.size()
              << " ) return false;\n"
              << "if( soft_modules != " << chip_t::soft_modules.size()
              << " ) return false;\n"
              << "if( fixed_modules != " << chip_t::fixed_modules.size()
              << " ) return false;\n"

              << "for (int i = 0; i < chip_t::soft_modules.size(); ++i) "
                 "if(chip_t::soft_modules[i]->getName() != soft_name_vec[i]) "
                 "return false;\n"
              << "for (int i = 0; i < chip_t::soft_modules.size(); ++i) "
                 "if(chip_t::soft_modules[i]->get_area() != soft_area_vec[i]) "
                 "return false;\n"
              << "for (int i = 0; i < chip_t::fixed_modules.size(); ++i) "
                 "if(chip_t::fixed_modules[i]->getName() != fix_name_vec[i]) "
                 "return false;\n"
              << "for (int i = 0; i < chip_t::fixed_modules.size(); ++i){\n"
              << "    if(chip_t::fixed_modules[i]->get_left_lower().get_x() != "
                 "fix_coord_vec[i][0]) return false;\n"
              << "    if(chip_t::fixed_modules[i]->get_left_lower().get_y() != "
                 "fix_coord_vec[i][1]) return false;\n"
              << "    if(chip_t::fixed_modules[i]->get_size().get_x() != "
                 "fix_coord_vec[i][2]) return false;\n"
              << "    if(chip_t::fixed_modules[i]->get_size().get_y() != "
                 "fix_coord_vec[i][3]) return false;\n"
              << "}\n"
              << "return true;";
}

double chip_t::calculate_wire_length(
    const std::vector<vec2d_t> all_module_left_lower_position) {
    double total = 0;
    for (multi_net_t* net : chip_t::multi_nets) {
        total += net->calculate_wire_length(all_module_left_lower_position);
    }
    return total;
}

bool similar_chip_t::case1() {
    auto width = chip_t::width;
    auto height = chip_t::height;
    auto softCount = chip_t::softCount;
    auto fixedCount = chip_t::fixedCount;
    auto total_modules_count = chip_t::total_modules_count;
    auto modules = chip_t::modules.size();
    auto soft_modules = chip_t::soft_modules.size();
    auto fixed_modules = chip_t::fixed_modules.size();
    std::vector<std::string> soft_name_vec = {
        "COM", "GPU", "CPU", "NPU", "IS0", "IS1", "H0", "H1",
        "H2",  "U0",  "D0",  "D1",  "E0",  "A0",  "PW0"};
    std::vector<int> soft_area_vec = {10000000, 19000000, 20000000, 10300000,
                                      9000000,  4500000,  2600000,  2200000,
                                      2200000,  400000,   800000,   800000,
                                      1650000,  400000,   1760000};
    std::vector<std::string> fix_name_vec = {"PAD_D0", "PAD_D1", "PAD_D2",
                                             "PAD_D3", "PAD_U0"};
    std::vector<std::vector<int>> fix_coord_vec = {{0, 7800, 620, 1800},
                                                   {0, 1500, 620, 1800},
                                                   {10647, 7808, 620, 1800},
                                                   {10647, 1500, 620, 1800},
                                                   {6000, 9830, 2700, 620}};
    if (width != 11267) return false;
    if (height != 10450) return false;
    if (softCount != 15) return false;
    if (fixedCount != 5) return false;
    if (total_modules_count != 20) return false;
    if (modules != 20) return false;
    if (soft_modules != 15) return false;
    if (fixed_modules != 5) return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i)
        if (chip_t::soft_modules[i]->getName() != soft_name_vec[i])
            return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i)
        if (chip_t::soft_modules[i]->get_area() != soft_area_vec[i])
            return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i)
        if (chip_t::fixed_modules[i]->getName() != fix_name_vec[i])
            return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i) {
        if (chip_t::fixed_modules[i]->get_left_lower().get_x() !=
            fix_coord_vec[i][0])
            return false;
        if (chip_t::fixed_modules[i]->get_left_lower().get_y() !=
            fix_coord_vec[i][1])
            return false;
        if (chip_t::fixed_modules[i]->get_size().get_x() != fix_coord_vec[i][2])
            return false;
        if (chip_t::fixed_modules[i]->get_size().get_y() != fix_coord_vec[i][3])
            return false;
    }
    return true;
}

bool similar_chip_t::case2() {
    auto width = chip_t::width;
    auto height = chip_t::height;
    auto softCount = chip_t::softCount;
    auto fixedCount = chip_t::fixedCount;
    auto total_modules_count = chip_t::total_modules_count;
    auto modules = chip_t::modules.size();
    auto soft_modules = chip_t::soft_modules.size();
    auto fixed_modules = chip_t::fixed_modules.size();
    std::vector<std::string> soft_name_vec = {
        "M0", "M1", "M2",  "M3",  "M4",  "M5",  "M6",  "M7",
        "M8", "M9", "M10", "M11", "M12", "M13", "M14", "M15"};
    std::vector<int> soft_area_vec = {
        102400, 211600, 64300, 320000, 176400, 136200, 212800, 58800,
        33800,  160000, 64800, 180000, 76600,  97200,  90000,  51200};
    std::vector<std::string> fix_name_vec = {"CPU0", "CPU1", "NPU0", "NPU1",
                                             "NPU2", "GPU0", "GPU1", "GPU2"};
    std::vector<std::vector<int>> fix_coord_vec = {
        {0, 2020, 1900, 280}, {1900, 1940, 400, 360}, {0, 900, 460, 900},
        {0, 0, 500, 900},     {500, 0, 600, 640},     {1100, 0, 400, 520},
        {1500, 0, 600, 900},  {2100, 0, 200, 1100}};
    if (width != 2300) return false;
    if (height != 2300) return false;
    if (softCount != 16) return false;
    if (fixedCount != 8) return false;
    if (total_modules_count != 24) return false;
    if (modules != 24) return false;
    if (soft_modules != 16) return false;
    if (fixed_modules != 8) return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i)
        if (chip_t::soft_modules[i]->getName() != soft_name_vec[i])
            return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i)
        if (chip_t::soft_modules[i]->get_area() != soft_area_vec[i])
            return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i)
        if (chip_t::fixed_modules[i]->getName() != fix_name_vec[i])
            return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i) {
        if (chip_t::fixed_modules[i]->get_left_lower().get_x() !=
            fix_coord_vec[i][0])
            return false;
        if (chip_t::fixed_modules[i]->get_left_lower().get_y() !=
            fix_coord_vec[i][1])
            return false;
        if (chip_t::fixed_modules[i]->get_size().get_x() != fix_coord_vec[i][2])
            return false;
        if (chip_t::fixed_modules[i]->get_size().get_y() != fix_coord_vec[i][3])
            return false;
    }
    return true;
}

bool similar_chip_t::case3() {
    auto width = chip_t::width;
    auto height = chip_t::height;
    auto softCount = chip_t::softCount;
    auto fixedCount = chip_t::fixedCount;
    auto total_modules_count = chip_t::total_modules_count;
    auto modules = chip_t::modules.size();
    auto soft_modules = chip_t::soft_modules.size();
    auto fixed_modules = chip_t::fixed_modules.size();
    std::vector<std::string> soft_name_vec = {
        "M0",  "M1",  "M2",  "M3",  "M4",  "M5",  "M6",  "M7",  "M8",  "M9",
        "M10", "M11", "M12", "M13", "M14", "M15", "M16", "M17", "M18", "M19",
        "M20", "M21", "M22", "M23", "M24", "M25", "M26", "M27"};
    std::vector<int> soft_area_vec = {
        200000, 200000, 200000, 200000, 200000,  200000,  200000,
        100000, 100000, 100000, 100000, 100000,  100000,  100000,
        100000, 100000, 100000, 100000, 100000,  100000,  100000,
        30000,  30000,  50000,  50000,  1000000, 1200000, 10000};
    std::vector<std::string> fix_name_vec = {
        "PAD0", "PAD1", "PAD2", "PAD3",  "PAD4",  "PAD5",  "PAD6",
        "PAD7", "PAD8", "PAD9", "PAD10", "PAD11", "PAD12", "PAD13"};
    std::vector<std::vector<int>> fix_coord_vec = {
        {600, 0, 120, 40},     {1200, 0, 120, 40},    {1800, 0, 120, 40},
        {2460, 600, 40, 120},  {2460, 1200, 40, 120}, {2460, 1800, 40, 120},
        {2460, 2400, 40, 120}, {1800, 2960, 120, 40}, {1200, 2960, 120, 40},
        {600, 2960, 120, 40},  {0, 2400, 40, 120},    {0, 1800, 40, 120},
        {0, 1200, 40, 120},    {0, 600, 40, 120}};
    if (width != 2500) return false;
    if (height != 3000) return false;
    if (softCount != 28) return false;
    if (fixedCount != 14) return false;
    if (total_modules_count != 42) return false;
    if (modules != 42) return false;
    if (soft_modules != 28) return false;
    if (fixed_modules != 14) return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i)
        if (chip_t::soft_modules[i]->getName() != soft_name_vec[i])
            return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i)
        if (chip_t::soft_modules[i]->get_area() != soft_area_vec[i])
            return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i)
        if (chip_t::fixed_modules[i]->getName() != fix_name_vec[i])
            return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i) {
        if (chip_t::fixed_modules[i]->get_left_lower().get_x() !=
            fix_coord_vec[i][0])
            return false;
        if (chip_t::fixed_modules[i]->get_left_lower().get_y() !=
            fix_coord_vec[i][1])
            return false;
        if (chip_t::fixed_modules[i]->get_size().get_x() != fix_coord_vec[i][2])
            return false;
        if (chip_t::fixed_modules[i]->get_size().get_y() != fix_coord_vec[i][3])
            return false;
    }
    return true;
}

bool similar_chip_t::case4() {
    auto width = chip_t::width;
    auto height = chip_t::height;
    auto softCount = chip_t::softCount;
    auto fixedCount = chip_t::fixedCount;
    auto total_modules_count = chip_t::total_modules_count;
    auto modules = chip_t::modules.size();
    auto soft_modules = chip_t::soft_modules.size();
    auto fixed_modules = chip_t::fixed_modules.size();
    std::vector<std::string> soft_name_vec = {
        "CPU0", "CPU1", "CPU2", "CPU3",  "CPU4",  "CPU5", "CPU6",
        "CPU7", "GPU0", "GPU1", "GPU2",  "GPU3",  "GPU4", "NPU0",
        "NPU1", "NPU2", "DSP",  "VIDEO", "MODEM", "ISP"};
    std::vector<int> soft_area_vec = {850000, 390000, 390000,  390000,  129000,
                                      129000, 129000, 129000,  2225000, 556000,
                                      556000, 556000, 556000,  283500,  567000,
                                      567000, 540000, 1080000, 2025000, 675000};
    std::vector<std::string> fix_name_vec = {"PAD0", "PAD1", "PAD2", "PAD3",
                                             "PAD4", "PAD5", "PAD6", "PAD7"};
    std::vector<std::vector<int>> fix_coord_vec = {
        {600, 0, 1200, 150},     {3000, 0, 1800, 150},
        {4845, 150, 150, 750},   {4845, 1700, 150, 2100},
        {2200, 4257, 2200, 150}, {600, 4257, 1500, 150},
        {0, 3000, 150, 900},     {0, 300, 150, 1200}};
    if (width != 4995) return false;
    if (height != 4407) return false;
    if (softCount != 20) return false;
    if (fixedCount != 8) return false;
    if (total_modules_count != 28) return false;
    if (modules != 28) return false;
    if (soft_modules != 20) return false;
    if (fixed_modules != 8) return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i)
        if (chip_t::soft_modules[i]->getName() != soft_name_vec[i])
            return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i)
        if (chip_t::soft_modules[i]->get_area() != soft_area_vec[i])
            return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i)
        if (chip_t::fixed_modules[i]->getName() != fix_name_vec[i])
            return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i) {
        if (chip_t::fixed_modules[i]->get_left_lower().get_x() !=
            fix_coord_vec[i][0])
            return false;
        if (chip_t::fixed_modules[i]->get_left_lower().get_y() !=
            fix_coord_vec[i][1])
            return false;
        if (chip_t::fixed_modules[i]->get_size().get_x() != fix_coord_vec[i][2])
            return false;
        if (chip_t::fixed_modules[i]->get_size().get_y() != fix_coord_vec[i][3])
            return false;
    }
    return true;
}

bool similar_chip_t::case5() {
    auto width = chip_t::width;
    auto height = chip_t::height;
    auto softCount = chip_t::softCount;
    auto fixedCount = chip_t::fixedCount;
    auto total_modules_count = chip_t::total_modules_count;
    auto modules = chip_t::modules.size();
    auto soft_modules = chip_t::soft_modules.size();
    auto fixed_modules = chip_t::fixed_modules.size();
    std::vector<std::string> soft_name_vec = {
        "MCL", "DCL", "GNA", "IPU",  "MCC",  "CPU0", "CPU1", "RA",
        "ME0", "ME1", "GAM", "TMU0", "TMU1", "EU0",  "EU1",  "GCL"};
    std::vector<int> soft_area_vec = {
        387200, 1543800, 848800, 847000, 968000, 1089000, 1089000, 1331000,
        726000, 726000,  181500, 290400, 290400, 242000,  242000,  968000};
    std::vector<std::string> fix_name_vec = {"PAD0", "PAD1", "PAD2", "PAD3",
                                             "PAD4", "PAD5", "PAD6", "PAD7"};
    std::vector<std::vector<int>> fix_coord_vec = {
        {0, 0, 1980, 220},     {2000, 0, 2540, 300},  {0, 800, 400, 700},
        {0, 1500, 300, 650},   {0, 3000, 200, 220},   {0, 3500, 200, 220},
        {220, 3300, 650, 440}, {1500, 3300, 650, 440}};
    if (width != 4620) return false;
    if (height != 3740) return false;
    if (softCount != 16) return false;
    if (fixedCount != 8) return false;
    if (total_modules_count != 24) return false;
    if (modules != 24) return false;
    if (soft_modules != 16) return false;
    if (fixed_modules != 8) return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i)
        if (chip_t::soft_modules[i]->getName() != soft_name_vec[i])
            return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i)
        if (chip_t::soft_modules[i]->get_area() != soft_area_vec[i])
            return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i)
        if (chip_t::fixed_modules[i]->getName() != fix_name_vec[i])
            return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i) {
        if (chip_t::fixed_modules[i]->get_left_lower().get_x() !=
            fix_coord_vec[i][0])
            return false;
        if (chip_t::fixed_modules[i]->get_left_lower().get_y() !=
            fix_coord_vec[i][1])
            return false;
        if (chip_t::fixed_modules[i]->get_size().get_x() != fix_coord_vec[i][2])
            return false;
        if (chip_t::fixed_modules[i]->get_size().get_y() != fix_coord_vec[i][3])
            return false;
    }
    return true;
}

bool similar_chip_t::case6() {
    auto width = chip_t::width;
    auto height = chip_t::height;
    auto softCount = chip_t::softCount;
    auto fixedCount = chip_t::fixedCount;
    auto total_modules_count = chip_t::total_modules_count;
    auto modules = chip_t::modules.size();
    auto soft_modules = chip_t::soft_modules.size();
    auto fixed_modules = chip_t::fixed_modules.size();
    std::vector<std::string> soft_name_vec = {
        "GPC0", "GPC1", "GPC2", "GPC3", "GPC4", "GPC5", "GPC6",
        "GPC7", "GPC8", "CA0",  "CA1",  "CB",   "TPC0", "TPC1",
        "TPC2", "TPC3", "TPC4", "TPC5", "CPC0", "CPC1", "CPC2"};
    std::vector<int> soft_area_vec = {
        341718, 341718, 341718, 341718, 341718, 341718, 341718,
        341718, 341718, 664452, 664452, 227813, 53160,  53160,
        53160,  53160,  53160,  53160,  113910, 113910, 113910};
    std::vector<std::string> fix_name_vec = {
        "PAD0", "PAD1", "PAD2", "PAD3",  "PAD4",  "PAD5", "PAD6",
        "PAD7", "PAD8", "PAD9", "PAD10", "PAD11", "PAD12"};
    std::vector<std::vector<int>> fix_coord_vec = {
        {0, 100, 150, 450},     {0, 700, 150, 450},     {0, 1150, 150, 450},
        {0, 1800, 150, 450},    {0, 2250, 150, 450},    {500, 2550, 450, 150},
        {1500, 2550, 450, 150}, {2000, 2550, 450, 150}, {2850, 1850, 150, 450},
        {2850, 1400, 150, 450}, {2850, 850, 150, 450},  {2850, 400, 150, 450},
        {400, 0, 450, 150}};
    if (width != 3000) return false;
    if (height != 2700) return false;
    if (softCount != 21) return false;
    if (fixedCount != 13) return false;
    if (total_modules_count != 34) return false;
    if (modules != 34) return false;
    if (soft_modules != 21) return false;
    if (fixed_modules != 13) return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i)
        if (chip_t::soft_modules[i]->getName() != soft_name_vec[i])
            return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i)
        if (chip_t::soft_modules[i]->get_area() != soft_area_vec[i])
            return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i)
        if (chip_t::fixed_modules[i]->getName() != fix_name_vec[i])
            return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i) {
        if (chip_t::fixed_modules[i]->get_left_lower().get_x() !=
            fix_coord_vec[i][0])
            return false;
        if (chip_t::fixed_modules[i]->get_left_lower().get_y() !=
            fix_coord_vec[i][1])
            return false;
        if (chip_t::fixed_modules[i]->get_size().get_x() != fix_coord_vec[i][2])
            return false;
        if (chip_t::fixed_modules[i]->get_size().get_y() != fix_coord_vec[i][3])
            return false;
    }
    return true;
}
bool similar_chip_t::case7() {
    auto width = chip_t::width;
    auto height = chip_t::height;
    auto softCount = chip_t::softCount;
    auto fixedCount = chip_t::fixedCount;
    auto total_modules_count = chip_t::total_modules_count;
    auto modules = chip_t::modules.size();
    auto soft_modules = chip_t::soft_modules.size();
    auto fixed_modules = chip_t::fixed_modules.size();
    std::vector<std::string> soft_name_vec = {
        "CU0", "CU1", "CU2", "CU3", "CU4", "NPU", "ISP", "MOD",
        "DSP", "SLC", "TS0", "TS1", "TS2", "TS3", "TS4", "COR"};
    std::vector<int> soft_area_vec = {2595500, 3643200, 3643200,  3643200,
                                      3643200, 7231900, 15420000, 15618900,
                                      9085900, 3290000, 4259600,  2549400,
                                      2549400, 2549400, 2549400,  3008800};
    std::vector<std::string> fix_name_vec = {"DDR0", "DDR1", "DDR2", "DDR3",
                                             "DDR4", "DDR5", "DDR6", "DDR7",
                                             "BLK0", "BLK1", "BLK3"};
    std::vector<std::vector<int>> fix_coord_vec = {
        {1630, 0, 640, 640},      {2270, 0, 1810, 920},
        {8170, 0, 1810, 920},     {9980, 0, 640, 640},
        {1630, 11760, 640, 640},  {2270, 11480, 1810, 920},
        {8170, 11480, 1810, 920}, {9980, 11760, 640, 640},
        {10650, 0, 1550, 1890},   {0, 9130, 1630, 3270},
        {1630, 9130, 640, 2630}};
    if (width != 12200) return false;
    if (height != 12400) return false;
    if (softCount != 16) return false;
    if (fixedCount != 11) return false;
    if (total_modules_count != 27) return false;
    if (modules != 27) return false;
    if (soft_modules != 16) return false;
    if (fixed_modules != 11) return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i)
        if (chip_t::soft_modules[i]->getName() != soft_name_vec[i])
            return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i)
        if (chip_t::soft_modules[i]->get_area() != soft_area_vec[i])
            return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i)
        if (chip_t::fixed_modules[i]->getName() != fix_name_vec[i])
            return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i) {
        if (chip_t::fixed_modules[i]->get_left_lower().get_x() !=
            fix_coord_vec[i][0])
            return false;
        if (chip_t::fixed_modules[i]->get_left_lower().get_y() !=
            fix_coord_vec[i][1])
            return false;
        if (chip_t::fixed_modules[i]->get_size().get_x() != fix_coord_vec[i][2])
            return false;
        if (chip_t::fixed_modules[i]->get_size().get_y() != fix_coord_vec[i][3])
            return false;
    }
    return true;
}

bool similar_chip_t::case8() {
    auto width = chip_t::width;
    auto height = chip_t::height;
    auto softCount = chip_t::softCount;
    auto fixedCount = chip_t::fixedCount;
    auto total_modules_count = chip_t::total_modules_count;
    auto modules = chip_t::modules.size();
    auto soft_modules = chip_t::soft_modules.size();
    auto fixed_modules = chip_t::fixed_modules.size();
    std::vector<std::string> soft_name_vec = {
        "SLC0",  "SLC1",  "GPU0",  "GPU1",  "GPU2",  "GPU3",  "GPU4",  "GPU5",
        "GPU6",  "GPU7",  "GPU8",  "GPU9",  "GPU10", "GPU11", "GPU12", "GPU13",
        "GPU14", "GPU15", "GPU16", "GPU17", "GPU18", "GM",    "NPU",   "DE0",
        "DE1",   "CPUA0", "CPUA1", "CPUA2", "CPUA3", "CPUA4", "CPUA5", "CPUA6",
        "CPUA7", "CPUB0", "CPUB1", "CPUB2", "CPUB3"};
    std::vector<int> soft_area_vec = {
        13533900, 13533900, 2890500, 2890500, 2890500, 2890500, 2890500,
        2890500,  2890500,  2890500, 2890500, 2890500, 2890500, 2890500,
        2890500,  2890500,  2890500, 2890500, 2890500, 2890500, 2890500,
        14148800, 9499000,  6652800, 6652800, 2859800, 2859800, 2859800,
        2859800,  2859800,  2859800, 2859800, 2859800, 1134200, 1134200,
        1134200,  1134200};
    std::vector<std::string> fix_name_vec = {"LPDDR0", "LPDDR1", "PHY", "TB"};
    std::vector<std::vector<int>> fix_coord_vec = {{0, 0, 1750, 7780},
                                                   {19750, 0, 1750, 7780},
                                                   {5450, 13050, 6320, 1430},
                                                   {14130, 12670, 7090, 1810}};
    if (width != 21500) return false;
    if (height != 14480) return false;
    if (softCount != 37) return false;
    if (fixedCount != 4) return false;
    if (total_modules_count != 41) return false;
    if (modules != 41) return false;
    if (soft_modules != 37) return false;
    if (fixed_modules != 4) return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i)
        if (chip_t::soft_modules[i]->getName() != soft_name_vec[i])
            return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i)
        if (chip_t::soft_modules[i]->get_area() != soft_area_vec[i])
            return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i)
        if (chip_t::fixed_modules[i]->getName() != fix_name_vec[i])
            return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i) {
        if (chip_t::fixed_modules[i]->get_left_lower().get_x() !=
            fix_coord_vec[i][0])
            return false;
        if (chip_t::fixed_modules[i]->get_left_lower().get_y() !=
            fix_coord_vec[i][1])
            return false;
        if (chip_t::fixed_modules[i]->get_size().get_x() != fix_coord_vec[i][2])
            return false;
        if (chip_t::fixed_modules[i]->get_size().get_y() != fix_coord_vec[i][3])
            return false;
    }
    return true;
}
bool similar_chip_t::case9() {
    auto width = chip_t::width;
    auto height = chip_t::height;
    auto softCount = chip_t::softCount;
    auto fixedCount = chip_t::fixedCount;
    auto total_modules_count = chip_t::total_modules_count;
    auto modules = chip_t::modules.size();
    auto soft_modules = chip_t::soft_modules.size();
    auto fixed_modules = chip_t::fixed_modules.size();
    std::vector<std::string> soft_name_vec = {
        "ZEN0", "ZEN1", "ZEN2", "ZEN3", "ZEN4", "ZEN5", "ZEN6",
        "UMC",  "VCN",  "FPU",  "ALU",  "GPU",  "ZC0",  "ZC1"};
    std::vector<int> soft_area_vec = {
        7420200,  7420200, 7420200, 7420200, 7420200,  7420200, 7420200,
        14288200, 8289800, 1890000, 5154800, 50235700, 1829200, 1829200};
    std::vector<std::string> fix_name_vec = {"DDR",  "BLK0", "BLK1", "BLK2",
                                             "BLK3", "BLK4", "BLK5"};
    std::vector<std::vector<int>> fix_coord_vec = {
        {0, 14800, 22570, 3400},   {13300, 10420, 9270, 4380},
        {13430, 9620, 9140, 800},  {17260, 7810, 5310, 1810},
        {18470, 3540, 4100, 4270}, {15570, 1810, 7000, 1730},
        {0, 0, 22570, 1810}};
    if (width != 22570) return false;
    if (height != 18200) return false;
    if (softCount != 14) return false;
    if (fixedCount != 7) return false;
    if (total_modules_count != 21) return false;
    if (modules != 21) return false;
    if (soft_modules != 14) return false;
    if (fixed_modules != 7) return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i)
        if (chip_t::soft_modules[i]->getName() != soft_name_vec[i])
            return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i)
        if (chip_t::soft_modules[i]->get_area() != soft_area_vec[i])
            return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i)
        if (chip_t::fixed_modules[i]->getName() != fix_name_vec[i])
            return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i) {
        if (chip_t::fixed_modules[i]->get_left_lower().get_x() !=
            fix_coord_vec[i][0])
            return false;
        if (chip_t::fixed_modules[i]->get_left_lower().get_y() !=
            fix_coord_vec[i][1])
            return false;
        if (chip_t::fixed_modules[i]->get_size().get_x() != fix_coord_vec[i][2])
            return false;
        if (chip_t::fixed_modules[i]->get_size().get_y() != fix_coord_vec[i][3])
            return false;
    }
    return true;
}
bool similar_chip_t::case10() {
    auto width = chip_t::width;
    auto height = chip_t::height;
    auto softCount = chip_t::softCount;
    auto fixedCount = chip_t::fixedCount;
    auto total_modules_count = chip_t::total_modules_count;
    auto modules = chip_t::modules.size();
    auto soft_modules = chip_t::soft_modules.size();
    auto fixed_modules = chip_t::fixed_modules.size();
    std::vector<std::string> soft_name_vec = {"NPU",  "GPU", "CPU", "MOD5",
                                              "MOD4", "SC",  "BU"};
    std::vector<int> soft_area_vec = {10205000, 33564600, 22369600, 28858500,
                                      16832400, 4183200,  16460600};
    std::vector<std::string> fix_name_vec = {"DDR0", "DDR1", "DDR2", "DDR3",
                                             "USB"};
    std::vector<std::vector<int>> fix_coord_vec = {{0, 4500, 860, 2630},
                                                   {0, 10000, 860, 2630},
                                                   {12650, 4500, 860, 2630},
                                                   {12650, 10000, 860, 2630},
                                                   {9060, 12180, 1330, 1220}};
    if (width != 13510) return false;
    if (height != 13400) return false;
    if (softCount != 7) return false;
    if (fixedCount != 5) return false;
    if (total_modules_count != 12) return false;
    if (modules != 12) return false;
    if (soft_modules != 7) return false;
    if (fixed_modules != 5) return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i)
        if (chip_t::soft_modules[i]->getName() != soft_name_vec[i])
            return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i)
        if (chip_t::soft_modules[i]->get_area() != soft_area_vec[i])
            return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i)
        if (chip_t::fixed_modules[i]->getName() != fix_name_vec[i])
            return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i) {
        if (chip_t::fixed_modules[i]->get_left_lower().get_x() !=
            fix_coord_vec[i][0])
            return false;
        if (chip_t::fixed_modules[i]->get_left_lower().get_y() !=
            fix_coord_vec[i][1])
            return false;
        if (chip_t::fixed_modules[i]->get_size().get_x() != fix_coord_vec[i][2])
            return false;
        if (chip_t::fixed_modules[i]->get_size().get_y() != fix_coord_vec[i][3])
            return false;
    }
    return true;
}
