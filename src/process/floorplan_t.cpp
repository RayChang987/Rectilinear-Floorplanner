//
// Created by 林士傑 on 2023/3/27.
//

#include "floorplan_t.h"
#include "fp_evaluator_t.h"
#include<string>

using std::string;
//for debug
#include<iostream>

using std::endl;
using std::cout;



unordered_map<const module_t*, int> floorplan_t::module_to_bd_soft_rect_i_m;
unordered_map<const module_t*, int> floorplan_t::module_to_bd_fixed_rect_i_m;
vector<vector<vec2d_t>> floorplan_t::soft_area_to_w_h_m;
uint32_t floorplan_t::min_w_h[2];
void floorplan_t::init() {
    uint16_t  soft_rect_n = chip_t::get_soft_modules().size();
    uint16_t fixed_rect_n = chip_t::get_fixed_modules().size();

    const std::vector<soft_module_t*>& soft_modules = chip_t::get_soft_modules();

    const std::vector<fixed_module_t*>& fixed_modules = chip_t::get_fixed_modules();

    soft_area_to_w_h_m.resize(soft_rect_n);
    for (int i = 0; i < soft_rect_n; ++i) {
        soft_area_to_w_h_m[i] = find_w_h(soft_modules[i]->get_area());
        floorplan_t::module_to_bd_soft_rect_i_m[soft_modules[i]] = i;
    }
}

floorplan_t::floorplan_t() {
    wirelength = 0;
    const std::vector<soft_module_t*>& soft_modules = chip_t::get_soft_modules();
    const std::vector<fixed_module_t*>& fixed_modules = chip_t::get_fixed_modules();

    //load and make the soft module (undefined shape)
    soft_is_placed.resize(soft_modules.size());
    soft_area_to_w_h_m.resize(soft_modules.size());
    for (int i = 0; i < soft_modules.size(); ++i) {
        soft_is_placed[i] = false;
        bounding_rectangle_t bd = soft_modules[i]->make_bd();
        soft_area_to_w_h_m[i] = find_w_h(soft_modules[i]->get_area());

        floorplan_t::module_to_bd_soft_rect_i_m[soft_modules[i]] = i;
        soft_rects.push_back(bd);
    }


    //load and place the fixed modules
    fixed_rects.reserve(fixed_modules.size());
    for (int i = 0; i < fixed_modules.size(); ++i) {
        bounding_rectangle_t bd = fixed_modules[i]->make_bd();
        floorplan_t::module_to_bd_fixed_rect_i_m[fixed_modules[i]] = i;
        fixed_rects.push_back(bd);
        bool success = polygon_forest.add_rect(fixed_rects[i]);
        if ( success== false) {  //if the fixed modules can't be placed, must be wrong.
            fp_status = fail_on_placing_fixed_modules;
            cout << "fail on placing fixed modules" << endl;
            return;
        }
    }


    cal_soft_deg();
    score = fp_evaluator_t::get_score(*this);

}


double floorplan_t::bd_distance(const bounding_rectangle_t& a, const bounding_rectangle_t& b){
	rect_t rect_a = a.getRect();
	rect_t rect_b = b.getRect();
	vec2d_t dis = rect_a.get_center() - rect_b.get_center();
	return fabs(dis.get_x()) + fabs(dis.get_y()); //must be fabs
}
double floorplan_t::VE_calculator(const bounding_rectangle_t& bd_rect,pair<const module_t* const, const int> neighbor){
	const module_t* neighbor_module = neighbor.first;
	const bounding_rectangle_t* neighbor_rect;
	int neighbor_rect_i = -1;
	double sum = 0;
	
	if (module_to_bd_soft_rect_i_m.count(neighbor_module)) {
		neighbor_rect_i = module_to_bd_soft_rect_i_m[neighbor_module];
		if (soft_is_placed[neighbor_rect_i] == true) { //if the module is not placed, skip this wirelength
			sum += bd_distance(soft_rects[neighbor_rect_i], bd_rect);
		}
	}
	if (module_to_bd_fixed_rect_i_m.count(neighbor_module)) {
		neighbor_rect_i = module_to_bd_fixed_rect_i_m[neighbor_module];
		sum += bd_distance(fixed_rects[neighbor_rect_i], bd_rect);
	}
	return sum*static_cast<double>(neighbor.second);
}

void floorplan_t::calculate_wirelength(){
	if (fp_status == fail_on_placing_fixed_modules) {
		return;
	}
	double sum = 0;
	for (int i = 0; i < fixed_rects.size(); ++i) {
		const module_t* fixed_module = fixed_rects[i].getLinkModule();
		const vector<std::pair<const module_t* const, const int>> neighbors 
			= (fixed_module)->getConnections();
		for (const auto neighbor : neighbors) {
			sum += VE_calculator(fixed_rects[i], neighbor);
		}
	}
	for (int i = 0; i < soft_rects.size(); ++i) {
		const module_t* soft_module = soft_rects[i].getLinkModule();
		if (soft_is_placed[i] == false) { continue; }
		const vector<std::pair<const module_t* const, const int>> neighbors
			= (soft_module)->getConnections();
		for (auto& neighbor : neighbors) {
			sum += VE_calculator(soft_rects[i], neighbor);
		}
	}
	
	wirelength = sum / 2;
}
void floorplan_t::print_info(bool position){
	if (fp_status == fail_on_placing_fixed_modules) {
		cout << "fail on placing fixed modules" << endl;
		return;
	}
    if(position){
        cout << "fixed module : " << endl;
        for (int i = 0; i < fixed_rects.size(); ++i) {
            cout << i << " : " << fixed_rects[i].getLinkModule()->getName() << endl;
        }

        cout << "soft module : " << endl;
        for (int i = 0; i < soft_rects.size(); ++i) {
            cout << i << " : " <<soft_rects[i].getLinkModule()->getName()<<" ";

            if(soft_is_placed[i]){
                cout<<"placed at "<<"{"<<soft_rects[i].getRect().get_center().get_x()<<", "
                <<soft_rects[i].getRect().get_center().get_y()<<"}"<<endl;
            }
            else{
                cout<<"not placed"<<endl;
            }
        }
    }

	get_wirelength();
	cout << "current wirelength : " << wirelength << endl;
    cout<<"current score : "<<score<<endl;
}
double floorplan_t::get_wirelength()
{
	calculate_wirelength();
	return wirelength;
}



bool floorplan_t::place_soft_module(size_t i, vec2d_t lower_left_pos,vec2d_t size) {
    if(soft_is_placed[i]){return false;}
	rect_t target_rect(lower_left_pos,size);
	//const soft_module_t* const target_module = dynamic_cast<const soft_module_t*>(soft_rects[i].getLinkModule());
	soft_module_t* target_module = (soft_module_t*)(soft_rects[i].getLinkModule()); //due to the const->non-const :(
	auto result = target_module->make_bd(target_rect);
	bounding_rectangle_t target_bd = result.first;
	bool success = false;

	if (result.second == true) {
        //cout<< target_rect.get_left_lower().get_x()<<" "<<target_rect.get_left_lower().get_y()<<" "<<target_rect.get_size().get_x()<<" "<<target_rect.get_size().get_y()<<endl;
		success = polygon_forest.add_rect(target_bd);
        int temp = 0;
        //cout<<success<<endl;
        //cout<< success<<endl;
	}
	if (success) {
		soft_is_placed[i] = true;
        soft_rects[i] = target_bd; //error here
	}
    else{
        soft_is_placed[i] = false;
    }
    score = fp_evaluator_t::get_score(*this);
	return success;
}

const size_t floorplan_t::get_soft_rect_n() const
{
	return soft_rects.size();
}

vector<int> floorplan_t::get_unplaced_id() {
    vector<int> ret;
    for(int i = 0; i<soft_rects.size(); ++i){
        if(soft_is_placed[i]==0){
            ret.push_back(i);
        }
    }
    return ret;
}


vector<vec2d_t> floorplan_t::find_w_h(uint32_t area){
    if(area<1){return {{0, 0}};}
    if(area==1){return {{1,1}};}
    uint32_t  from = sqrt(area/2);
    vector<vec2d_t> ret;

    for(uint32_t i = from; i<= sqrt(area); ++i){ //i will be the short edge
        if(area%i==0){
            int x = i, y = area/i;
            if(x*2>=y){
                ret.push_back({x, y});
                if(x!=y){
                    ret.push_back({y, x});
                }

            }
        }
    }
    if(ret.size()==0){
        int x = ceil(sqrt(area));
        return {{x,x}};
    }
    return ret;
}


const double floorplan_t::get_score() const{
    return score;
}

const vector<uint32_t>& floorplan_t::get_soft_deg() {
    return soft_deg;
}

void floorplan_t::GUI_validation() {
    vector<bounding_rectangle_t> bd;
    vector<bounding_rectangle_t> placed_soft;
    bd.insert(bd.end(), this->fixed_rects.begin(), this->fixed_rects.end());
    //cout<<bd.size()<<endl;
    for(int i = 0; i<soft_rects.size(); ++i){
        if(soft_is_placed[i]){
            //cout<<soft_rects[i].getRect().get_size()<<" "<<soft_rects[i].getRect().get_center()<<endl;
            placed_soft.push_back(soft_rects[i]);
        }
    }
    bd.insert(bd.end(), placed_soft.begin(), placed_soft.end());
    visualizer_t::set_window_name("fp");
    visualizer_t::show_fp(bd);
    //std::cout<<"Press any key to continue"<<std::endl;
    //fgetc(stdin);
}

void floorplan_t::cal_soft_deg() {
    soft_deg.resize(soft_rects.size());
    for(int i = 0; i<soft_rects.size(); ++i){
        uint32_t deg = 0;
        const std::vector<std::pair<const module_t *const, const int>> neighbors = soft_rects[i].getLinkModule()->getConnections();
        for(auto& neighbor:neighbors){
            deg+=neighbor.second;
        }
        soft_deg[i] = deg;
    }
}

polygon_forest_t floorplan_t::get_polygon_forest() {
    return this->polygon_forest;
}

const vector<bool> & floorplan_t::get_soft_is_placed() const{
    return soft_is_placed;
}

const size_t floorplan_t::get_fixed_rect_n() const {
    return fixed_rects.size();
}
