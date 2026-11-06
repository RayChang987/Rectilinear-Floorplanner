#ifndef ICCAD2023PD_TESTER_H
#define ICCAD2023PD_TESTER_H

// Runs the rectilinear flow on a predetermined sequence pair (case_table_t)
// of a known test case, without the annealing.
class tester_t {
   public:
    tester_t();
    void test_sp();
};

#endif  // ICCAD2023PD_TESTER_H
