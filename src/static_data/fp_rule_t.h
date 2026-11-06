#ifndef INC_2023PD_FP_RULE_T_H
#define INC_2023PD_FP_RULE_T_H
class fp_rule_t {
   public:
    static double lower_ratio;
    static double upper_ratio;

    static double get_lower_ratio();
    static double get_upper_ratio();
};

#endif
