#include "fp_rule_t.h"

double fp_rule_t::lower_ratio = 0.5f;
double fp_rule_t::upper_ratio = 2;

double fp_rule_t::get_lower_ratio() { return fp_rule_t::lower_ratio; }
double fp_rule_t::get_upper_ratio() { return fp_rule_t::upper_ratio; }
