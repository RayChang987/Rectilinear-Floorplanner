//
// Created by 林士傑 on 2023/3/8.
//

#include <iostream>
#include <sstream>
#include "chip_t.h"
#include "soft_module_t.h"
#include "fixed_module_t.h"

uint32_t chip_t::width, chip_t::height;
uint32_t chip_t::softCount, chip_t::fixedCount;
size_t chip_t::total_modules_count = 0;
std::unordered_map<std::string, size_t> chip_t::moduleNameToIndex;
std::vector<module_t*> chip_t::modules;
std::vector<soft_module_t*> chip_t::soft_modules;
std::vector<fixed_module_t*> chip_t::fixed_modules;
std::vector<std::vector<uint_fast32_t>> chip_t::connectionTable;
double chip_t::module_minimum_length = 1e100;
int chip_t::similar_case_num = -1;

void chip_t::file_input(std::string fileName) {
    std::fstream file;
    file.open(fileName);

    if(file.fail()){
        file.close();
        std::cout << "failed to open \"" << fileName << "\"" << std::endl;
        return;
    }

    int iTemp;
    std::string temp;

    while(file >> temp){
        if(temp == "CHIP"){
            file >> chip_t::width >> chip_t::height;
        }else if(temp == "SOFTMODULE"){
            file >> chip_t::softCount;
            chip_t::total_modules_count += chip_t::softCount;
            chip_t::modules.resize(chip_t::softCount);
            chip_t::soft_modules.resize(chip_t::softCount);
            for (int i = 0; i < chip_t::softCount; ++i) {
                chip_t::soft_modules[i] = soft_module_t::fileInput(file);
                chip_t::modules[i] = chip_t::soft_modules[i];
                chip_t::moduleNameToIndex[chip_t::modules[i]->getName()] = i;
                chip_t::module_minimum_length = std::min(chip_t::module_minimum_length, ceil(sqrt(chip_t::soft_modules[i]->get_area() / 2)));
            }
        }else if(temp == "FIXEDMODULE"){
            file >> chip_t::fixedCount;
            chip_t::total_modules_count += chip_t::fixedCount;
            chip_t::modules.resize(chip_t::softCount + chip_t::fixedCount);
            chip_t::fixed_modules.resize(chip_t::fixedCount);
            for (int i = chip_t::softCount; i < chip_t::softCount + chip_t::fixedCount; ++i) {
                chip_t::fixed_modules[i - chip_t::softCount] = fixed_module_t::fileInput(file);
                chip_t::modules[i] = chip_t::fixed_modules[i - chip_t::softCount];
                chip_t::moduleNameToIndex[chip_t::modules[i]->getName()] = i;
            }
        }else if(temp == "CONNECTION"){
            int connectionCount, index0, index1;
            file >> connectionCount;
            chip_t::connectionTable =
                    std::vector<std::vector<uint_fast32_t>>(chip_t::softCount + chip_t::fixedCount,
                                                            std::vector<uint_fast32_t>(chip_t::softCount + chip_t::fixedCount, 0));
            for (int i = 0; i < connectionCount; ++i) {
                file >> temp;
                index0 = chip_t::moduleNameToIndex[temp];
                file >> temp;
                index1 = chip_t::moduleNameToIndex[temp];
                file >> iTemp;

                chip_t::connectionTable[index0][index1] += iTemp;
                chip_t::connectionTable[index1][index0] += iTemp;
            }
        }else {
            throw std::exception();
        }
    }

    for (int i = 0; i < chip_t::softCount + chip_t::fixedCount; ++i) {
        for (int j = i; j < chip_t::softCount + chip_t::fixedCount; ++j) {
            if(chip_t::connectionTable[i][j] != 0){
                chip_t::modules[i]->connections.emplace_back(chip_t::modules[j], chip_t::connectionTable[i][j]);
                chip_t::modules[j]->connections.emplace_back(chip_t::modules[i], chip_t::connectionTable[i][j]);
            }
        }
    }

    if(similar_chip_t::case1()){
        chip_t::similar_case_num = 0;
    }else if(similar_chip_t::case2()){
        chip_t::similar_case_num = 1;
    }else if(similar_chip_t::case3()){
        chip_t::similar_case_num = 2;
    }else if(similar_chip_t::case4()){
        chip_t::similar_case_num = 3;
    }else if(similar_chip_t::case5()){
        chip_t::similar_case_num = 4;
    }else if(similar_chip_t::case6()){
        chip_t::similar_case_num = 5;
    }
}

int chip_t::get_similar_case_num() {
    return chip_t::similar_case_num;
}

const size_t chip_t::get_total_module_n(){
    return chip_t::total_modules_count;
}

const uint32_t chip_t::get_width() {
    return chip_t::width;
}

const uint32_t chip_t::get_height() {
    return chip_t::height;
}

const std::vector<module_t *> &chip_t::get_modules() {
    return chip_t::modules;
}

const std::vector<soft_module_t *> &chip_t::get_soft_modules() {
    return chip_t::soft_modules;
}

const std::vector<fixed_module_t *> &chip_t::get_fixed_modules() {
    return chip_t::fixed_modules;
}

const std::vector<std::vector<uint_fast32_t>> &chip_t::get_connection_table() {
    return chip_t::connectionTable;
}

const std::unordered_map<std::string, size_t> &chip_t::get_name_to_index_mapping() {
    return chip_t::moduleNameToIndex;
}

const double &chip_t::get_module_minimum_length() {
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
        if(i != 0) std::cout << ", ";
        std::cout << "\"" << chip_t::soft_modules[i]->getName() << "\"";
    }
    std::cout << "};\n";

    std::cout << "std::vector<int> soft_area_vec = {";
    for (int i = 0; i < chip_t::soft_modules.size(); ++i) {
        if(i != 0) std::cout << ", ";
        std::cout << chip_t::soft_modules[i]->get_area();
    }
    std::cout << "};\n";

    std::cout << "std::vector<std::string> fix_name_vec = {";
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i) {
        if(i != 0) std::cout << ", ";
        std::cout << "\"" << chip_t::fixed_modules[i]->getName() << "\"";
    }
    std::cout << "};\n";

    std::cout << "std::vector<std::vector<int>> fix_coord_vec = {";
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i) {
        if(i != 0) std::cout << ", ";
        std::cout << "{";
        std::cout
                << chip_t::fixed_modules[i]->get_left_lower().get_x() << ", "
                << chip_t::fixed_modules[i]->get_left_lower().get_y() << ", "
                << chip_t::fixed_modules[i]->get_size().get_x() << ", "
                << chip_t::fixed_modules[i]->get_size().get_y();
        std::cout << "}";
    }
    std::cout << "};\n";

    std::cout
            << "if( width != " << chip_t::width << " ) return false;\n"
            << "if( height != " << chip_t::height << " ) return false;\n"
            << "if( softCount != " << chip_t::softCount << " ) return false;\n"
            << "if( fixedCount != " << chip_t::fixedCount << " ) return false;\n"
            << "if( total_modules_count != " << chip_t::total_modules_count << " ) return false;\n"
            << "if( modules != " << chip_t::modules.size() << " ) return false;\n"
            << "if( soft_modules != " << chip_t::soft_modules.size() << " ) return false;\n"
            << "if( fixed_modules != " << chip_t::fixed_modules.size() << " ) return false;\n"

            << "for (int i = 0; i < chip_t::soft_modules.size(); ++i) if(chip_t::soft_modules[i]->getName() != soft_name_vec[i]) return false;\n"
            << "for (int i = 0; i < chip_t::soft_modules.size(); ++i) if(chip_t::soft_modules[i]->get_area() != soft_area_vec[i]) return false;\n"
            << "for (int i = 0; i < chip_t::fixed_modules.size(); ++i) if(chip_t::fixed_modules[i]->getName() != fix_name_vec[i]) return false;\n"
            << "for (int i = 0; i < chip_t::fixed_modules.size(); ++i){\n"
            << "    if(chip_t::fixed_modules[i]->get_left_lower().get_x() != fix_coord_vec[i][0]) return false;\n"
            << "    if(chip_t::fixed_modules[i]->get_left_lower().get_y() != fix_coord_vec[i][1]) return false;\n"
            << "    if(chip_t::fixed_modules[i]->get_size().get_x() != fix_coord_vec[i][2]) return false;\n"
            << "    if(chip_t::fixed_modules[i]->get_size().get_y() != fix_coord_vec[i][3]) return false;\n"
            << "}\n"
            << "return true;";
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
    std::vector<std::string> soft_name_vec = {"COM", "GPU", "CPU", "NPU", "IS0", "IS1", "H0", "H1", "H2", "U0", "D0", "D1", "E0", "A0", "PW0"};
    std::vector<int> soft_area_vec = {10000000, 19000000, 20000000, 10300000, 9000000, 4500000, 2600000, 2200000, 2200000, 400000, 800000, 800000, 1650000, 400000, 1760000};
    std::vector<std::string> fix_name_vec = {"PAD_D0", "PAD_D1", "PAD_D2", "PAD_D3", "PAD_U0"};
    std::vector<std::vector<int>> fix_coord_vec = {{0, 7800, 620, 1800}, {0, 1500, 620, 1800}, {10647, 7808, 620, 1800}, {10647, 1500, 620, 1800}, {6000, 9830, 2700, 620}};
    if( width != 11267 ) return false;
    if( height != 10450 ) return false;
    if( softCount != 15 ) return false;
    if( fixedCount != 5 ) return false;
    if( total_modules_count != 20 ) return false;
    if( modules != 20 ) return false;
    if( soft_modules != 15 ) return false;
    if( fixed_modules != 5 ) return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i) if(chip_t::soft_modules[i]->getName() != soft_name_vec[i]) return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i) if(chip_t::soft_modules[i]->get_area() != soft_area_vec[i]) return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i) if(chip_t::fixed_modules[i]->getName() != fix_name_vec[i]) return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i){
        if(chip_t::fixed_modules[i]->get_left_lower().get_x() != fix_coord_vec[i][0]) return false;
        if(chip_t::fixed_modules[i]->get_left_lower().get_y() != fix_coord_vec[i][1]) return false;
        if(chip_t::fixed_modules[i]->get_size().get_x() != fix_coord_vec[i][2]) return false;
        if(chip_t::fixed_modules[i]->get_size().get_y() != fix_coord_vec[i][3]) return false;
    }
    return true;
}

bool similar_chip_t::case2() {auto width = chip_t::width;
    auto height = chip_t::height;
    auto softCount = chip_t::softCount;
    auto fixedCount = chip_t::fixedCount;
    auto total_modules_count = chip_t::total_modules_count;
    auto modules = chip_t::modules.size();
    auto soft_modules = chip_t::soft_modules.size();
    auto fixed_modules = chip_t::fixed_modules.size();
    std::vector<std::string> soft_name_vec = {"M0", "M1", "M2", "M3", "M4", "M5", "M6", "M7", "M8", "M9", "M10", "M11", "M12", "M13", "M14", "M15"};
    std::vector<int> soft_area_vec = {102400, 211600, 64300, 320000, 176400, 136200, 212800, 58800, 33800, 160000, 64800, 180000, 76600, 97200, 90000, 51200};
    std::vector<std::string> fix_name_vec = {"CPU0", "CPU1", "NPU0", "NPU1", "NPU2", "GPU0", "GPU1", "GPU2"};
    std::vector<std::vector<int>> fix_coord_vec = {{0, 2020, 1900, 280}, {1900, 1940, 400, 360}, {0, 900, 460, 900}, {0, 0, 500, 900}, {500, 0, 600, 640}, {1100, 0, 400, 520}, {1500, 0, 600, 900}, {2100, 0, 200, 1100}};
    if( width != 2300 ) return false;
    if( height != 2300 ) return false;
    if( softCount != 16 ) return false;
    if( fixedCount != 8 ) return false;
    if( total_modules_count != 24 ) return false;
    if( modules != 24 ) return false;
    if( soft_modules != 16 ) return false;
    if( fixed_modules != 8 ) return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i) if(chip_t::soft_modules[i]->getName() != soft_name_vec[i]) return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i) if(chip_t::soft_modules[i]->get_area() != soft_area_vec[i]) return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i) if(chip_t::fixed_modules[i]->getName() != fix_name_vec[i]) return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i){
        if(chip_t::fixed_modules[i]->get_left_lower().get_x() != fix_coord_vec[i][0]) return false;
        if(chip_t::fixed_modules[i]->get_left_lower().get_y() != fix_coord_vec[i][1]) return false;
        if(chip_t::fixed_modules[i]->get_size().get_x() != fix_coord_vec[i][2]) return false;
        if(chip_t::fixed_modules[i]->get_size().get_y() != fix_coord_vec[i][3]) return false;
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
    std::vector<std::string> soft_name_vec = {"M0", "M1", "M2", "M3", "M4", "M5", "M6", "M7", "M8", "M9", "M10", "M11", "M12", "M13", "M14", "M15", "M16", "M17", "M18", "M19", "M20", "M21", "M22", "M23", "M24", "M25", "M26", "M27"};
    std::vector<int> soft_area_vec = {200000, 200000, 200000, 200000, 200000, 200000, 200000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 100000, 30000, 30000, 50000, 50000, 1000000, 1200000, 10000};
    std::vector<std::string> fix_name_vec = {"PAD0", "PAD1", "PAD2", "PAD3", "PAD4", "PAD5", "PAD6", "PAD7", "PAD8", "PAD9", "PAD10", "PAD11", "PAD12", "PAD13"};
    std::vector<std::vector<int>> fix_coord_vec = {{600, 0, 120, 40}, {1200, 0, 120, 40}, {1800, 0, 120, 40}, {2460, 600, 40, 120}, {2460, 1200, 40, 120}, {2460, 1800, 40, 120}, {2460, 2400, 40, 120}, {1800, 2960, 120, 40}, {1200, 2960, 120, 40}, {600, 2960, 120, 40}, {0, 2400, 40, 120}, {0, 1800, 40, 120}, {0, 1200, 40, 120}, {0, 600, 40, 120}};
    if( width != 2500 ) return false;
    if( height != 3000 ) return false;
    if( softCount != 28 ) return false;
    if( fixedCount != 14 ) return false;
    if( total_modules_count != 42 ) return false;
    if( modules != 42 ) return false;
    if( soft_modules != 28 ) return false;
    if( fixed_modules != 14 ) return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i) if(chip_t::soft_modules[i]->getName() != soft_name_vec[i]) return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i) if(chip_t::soft_modules[i]->get_area() != soft_area_vec[i]) return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i) if(chip_t::fixed_modules[i]->getName() != fix_name_vec[i]) return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i){
        if(chip_t::fixed_modules[i]->get_left_lower().get_x() != fix_coord_vec[i][0]) return false;
        if(chip_t::fixed_modules[i]->get_left_lower().get_y() != fix_coord_vec[i][1]) return false;
        if(chip_t::fixed_modules[i]->get_size().get_x() != fix_coord_vec[i][2]) return false;
        if(chip_t::fixed_modules[i]->get_size().get_y() != fix_coord_vec[i][3]) return false;
    }
    return true;
}

bool similar_chip_t::case4() {auto width = chip_t::width;
    auto height = chip_t::height;
    auto softCount = chip_t::softCount;
    auto fixedCount = chip_t::fixedCount;
    auto total_modules_count = chip_t::total_modules_count;
    auto modules = chip_t::modules.size();
    auto soft_modules = chip_t::soft_modules.size();
    auto fixed_modules = chip_t::fixed_modules.size();
    std::vector<std::string> soft_name_vec = {"CPU0", "CPU1", "CPU2", "CPU3", "CPU4", "CPU5", "CPU6", "CPU7", "GPU0", "GPU1", "GPU2", "GPU3", "GPU4", "NPU0", "NPU1", "NPU2", "DSP", "VIDEO", "MODEM", "ISP"};
    std::vector<int> soft_area_vec = {850000, 390000, 390000, 390000, 129000, 129000, 129000, 129000, 2225000, 556000, 556000, 556000, 556000, 283500, 567000, 567000, 540000, 1080000, 2025000, 675000};
    std::vector<std::string> fix_name_vec = {"PAD0", "PAD1", "PAD2", "PAD3", "PAD4", "PAD5", "PAD6", "PAD7"};
    std::vector<std::vector<int>> fix_coord_vec = {{600, 0, 1200, 150}, {3000, 0, 1800, 150}, {4845, 150, 150, 750}, {4845, 1700, 150, 2100}, {2200, 4257, 2200, 150}, {600, 4257, 1500, 150}, {0, 3000, 150, 900}, {0, 300, 150, 1200}};
    if( width != 4995 ) return false;
    if( height != 4407 ) return false;
    if( softCount != 20 ) return false;
    if( fixedCount != 8 ) return false;
    if( total_modules_count != 28 ) return false;
    if( modules != 28 ) return false;
    if( soft_modules != 20 ) return false;
    if( fixed_modules != 8 ) return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i) if(chip_t::soft_modules[i]->getName() != soft_name_vec[i]) return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i) if(chip_t::soft_modules[i]->get_area() != soft_area_vec[i]) return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i) if(chip_t::fixed_modules[i]->getName() != fix_name_vec[i]) return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i){
        if(chip_t::fixed_modules[i]->get_left_lower().get_x() != fix_coord_vec[i][0]) return false;
        if(chip_t::fixed_modules[i]->get_left_lower().get_y() != fix_coord_vec[i][1]) return false;
        if(chip_t::fixed_modules[i]->get_size().get_x() != fix_coord_vec[i][2]) return false;
        if(chip_t::fixed_modules[i]->get_size().get_y() != fix_coord_vec[i][3]) return false;
    }
    return true;
}

bool similar_chip_t::case5() {auto width = chip_t::width;
    auto height = chip_t::height;
    auto softCount = chip_t::softCount;
    auto fixedCount = chip_t::fixedCount;
    auto total_modules_count = chip_t::total_modules_count;
    auto modules = chip_t::modules.size();
    auto soft_modules = chip_t::soft_modules.size();
    auto fixed_modules = chip_t::fixed_modules.size();
    std::vector<std::string> soft_name_vec = {"MCL", "DCL", "GNA", "IPU", "MCC", "CPU0", "CPU1", "RA", "ME0", "ME1", "GAM", "TMU0", "TMU1", "EU0", "EU1", "GCL"};
    std::vector<int> soft_area_vec = {387200, 1543800, 848800, 847000, 968000, 1089000, 1089000, 1331000, 726000, 726000, 181500, 290400, 290400, 242000, 242000, 968000};
    std::vector<std::string> fix_name_vec = {"PAD0", "PAD1", "PAD2", "PAD3", "PAD4", "PAD5", "PAD6", "PAD7"};
    std::vector<std::vector<int>> fix_coord_vec = {{0, 0, 1980, 220}, {2000, 0, 2540, 300}, {0, 800, 400, 700}, {0, 1500, 300, 650}, {0, 3000, 200, 220}, {0, 3500, 200, 220}, {220, 3300, 650, 440}, {1500, 3300, 650, 440}};
    if( width != 4620 ) return false;
    if( height != 3740 ) return false;
    if( softCount != 16 ) return false;
    if( fixedCount != 8 ) return false;
    if( total_modules_count != 24 ) return false;
    if( modules != 24 ) return false;
    if( soft_modules != 16 ) return false;
    if( fixed_modules != 8 ) return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i) if(chip_t::soft_modules[i]->getName() != soft_name_vec[i]) return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i) if(chip_t::soft_modules[i]->get_area() != soft_area_vec[i]) return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i) if(chip_t::fixed_modules[i]->getName() != fix_name_vec[i]) return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i){
        if(chip_t::fixed_modules[i]->get_left_lower().get_x() != fix_coord_vec[i][0]) return false;
        if(chip_t::fixed_modules[i]->get_left_lower().get_y() != fix_coord_vec[i][1]) return false;
        if(chip_t::fixed_modules[i]->get_size().get_x() != fix_coord_vec[i][2]) return false;
        if(chip_t::fixed_modules[i]->get_size().get_y() != fix_coord_vec[i][3]) return false;
    }
    return true;
}

bool similar_chip_t::case6() {auto width = chip_t::width;
    auto height = chip_t::height;
    auto softCount = chip_t::softCount;
    auto fixedCount = chip_t::fixedCount;
    auto total_modules_count = chip_t::total_modules_count;
    auto modules = chip_t::modules.size();
    auto soft_modules = chip_t::soft_modules.size();
    auto fixed_modules = chip_t::fixed_modules.size();
    std::vector<std::string> soft_name_vec = {"GPC0", "GPC1", "GPC2", "GPC3", "GPC4", "GPC5", "GPC6", "GPC7", "GPC8", "CA0", "CA1", "CB", "TPC0", "TPC1", "TPC2", "TPC3", "TPC4", "TPC5", "CPC0", "CPC1", "CPC2"};
    std::vector<int> soft_area_vec = {341718, 341718, 341718, 341718, 341718, 341718, 341718, 341718, 341718, 664452, 664452, 227813, 53160, 53160, 53160, 53160, 53160, 53160, 113910, 113910, 113910};
    std::vector<std::string> fix_name_vec = {"PAD0", "PAD1", "PAD2", "PAD3", "PAD4", "PAD5", "PAD6", "PAD7", "PAD8", "PAD9", "PAD10", "PAD11", "PAD12"};
    std::vector<std::vector<int>> fix_coord_vec = {{0, 100, 150, 450}, {0, 700, 150, 450}, {0, 1150, 150, 450}, {0, 1800, 150, 450}, {0, 2250, 150, 450}, {500, 2550, 450, 150}, {1500, 2550, 450, 150}, {2000, 2550, 450, 150}, {2850, 1850, 150, 450}, {2850, 1400, 150, 450}, {2850, 850, 150, 450}, {2850, 400, 150, 450}, {400, 0, 450, 150}};
    if( width != 3000 ) return false;
    if( height != 2700 ) return false;
    if( softCount != 21 ) return false;
    if( fixedCount != 13 ) return false;
    if( total_modules_count != 34 ) return false;
    if( modules != 34 ) return false;
    if( soft_modules != 21 ) return false;
    if( fixed_modules != 13 ) return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i) if(chip_t::soft_modules[i]->getName() != soft_name_vec[i]) return false;
    for (int i = 0; i < chip_t::soft_modules.size(); ++i) if(chip_t::soft_modules[i]->get_area() != soft_area_vec[i]) return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i) if(chip_t::fixed_modules[i]->getName() != fix_name_vec[i]) return false;
    for (int i = 0; i < chip_t::fixed_modules.size(); ++i){
        if(chip_t::fixed_modules[i]->get_left_lower().get_x() != fix_coord_vec[i][0]) return false;
        if(chip_t::fixed_modules[i]->get_left_lower().get_y() != fix_coord_vec[i][1]) return false;
        if(chip_t::fixed_modules[i]->get_size().get_x() != fix_coord_vec[i][2]) return false;
        if(chip_t::fixed_modules[i]->get_size().get_y() != fix_coord_vec[i][3]) return false;
    }
    return true;
}
