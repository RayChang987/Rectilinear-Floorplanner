//
// Created by 林士傑 on 2023/6/9.
//

#include "polygon_t.h"

#include <algorithm>
#include <iomanip>
#include "static_data/soft_module_t.h"

const std::vector<std::vector<std::vector<std::vector<int>>>> polygon_t::combination_list{{{}, {},                                            {},                                                                                                                                                                                                                                                                                               {}, {}, {}},
                                                                                          {{}, {{0}},                                         {},                                                                                                                                                                                                                                                                                               {}, {}, {}},
                                                                                          {{}, {{0}, {1}},                                    {{0, 1}},                                                                                                                                                                                                                                                                                         {}, {}, {}},
                                                                                          {{}, {{0}, {1}, {2}},                               {{0, 1}, {0, 2}, {1, 2}},                                                                                                                                                                                                                                                                         {{0, 1, 2}}, {}, {}},
                                                                                          {{}, {{0}, {1}, {2}, {3}},                          {{0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3}},                                                                                                                                                                                                                                                 {{0, 1, 2}, {0, 1, 3}, {0, 2, 3}, {1, 2, 3}}, {{0, 1, 2, 3}}, {}},
                                                                                          {{}, {{0}, {1}, {2}, {3}, {4}},                     {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}},                                                                                                                                                                                                                 {{0, 1, 2}, {0, 1, 3}, {0, 1, 4}, {0, 2, 3}, {0, 2, 4}, {0, 3, 4}, {1, 2, 3}, {1, 2, 4}, {1, 3, 4}, {2, 3, 4}}, {{0, 1, 2, 3}, {0, 1, 2, 4}, {0, 1, 3, 4}, {0, 2, 3, 4}, {1, 2, 3, 4}}, {{0, 1, 2, 3, 4}}},
                                                                                          {{}, {{0}, {1}, {2}, {3}, {4}, {5}},                {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {2, 3}, {2, 4}, {2, 5}, {3, 4}, {3, 5}, {4, 5}},                                                                                                                                                                         {{0, 1, 2}, {0, 1, 3}, {0, 1, 4}, {0, 1, 5}, {0, 2, 3}, {0, 2, 4}, {0, 2, 5}, {0, 3, 4}, {0, 3, 5}, {0, 4, 5}, {1, 2, 3}, {1, 2, 4}, {1, 2, 5}, {1, 3, 4}, {1, 3, 5}, {1, 4, 5}, {2, 3, 4}, {2, 3, 5}, {2, 4, 5}, {3, 4, 5}}, {{0, 1, 2, 3}, {0, 1, 2, 4}, {0, 1, 2, 5}, {0, 1, 3, 4}, {0, 1, 3, 5}, {0, 1, 4, 5}, {0, 2, 3, 4}, {0, 2, 3, 5}, {0, 2, 4, 5}, {0, 3, 4, 5}, {1, 2, 3, 4}, {1, 2, 3, 5}, {1, 2, 4, 5}, {1, 3, 4, 5}, {2, 3, 4, 5}}, {{0, 1, 2, 3, 4}, {0, 1, 2, 3, 5}, {0, 1, 2, 4, 5}, {0, 1, 3, 4, 5}, {0, 2, 3, 4, 5}, {1, 2, 3, 4, 5}}},
                                                                                          {{}, {{0}, {1}, {2}, {3}, {4}, {5}, {6}},           {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {2, 3}, {2, 4}, {2, 5}, {2, 6}, {3, 4}, {3, 5}, {3, 6}, {4, 5}, {4, 6}, {5, 6}},                                                                                                                         {{0, 1, 2}, {0, 1, 3}, {0, 1, 4}, {0, 1, 5}, {0, 1, 6}, {0, 2, 3}, {0, 2, 4}, {0, 2, 5}, {0, 2, 6}, {0, 3, 4}, {0, 3, 5}, {0, 3, 6}, {0, 4, 5}, {0, 4, 6}, {0, 5, 6}, {1, 2, 3}, {1, 2, 4}, {1, 2, 5}, {1, 2, 6}, {1, 3, 4}, {1, 3, 5}, {1, 3, 6}, {1, 4, 5}, {1, 4, 6}, {1, 5, 6}, {2, 3, 4}, {2, 3, 5}, {2, 3, 6}, {2, 4, 5}, {2, 4, 6}, {2, 5, 6}, {3, 4, 5}, {3, 4, 6}, {3, 5, 6}, {4, 5, 6}}, {{0, 1, 2, 3}, {0, 1, 2, 4}, {0, 1, 2, 5}, {0, 1, 2, 6}, {0, 1, 3, 4}, {0, 1, 3, 5}, {0, 1, 3, 6}, {0, 1, 4, 5}, {0, 1, 4, 6}, {0, 1, 5, 6}, {0, 2, 3, 4}, {0, 2, 3, 5}, {0, 2, 3, 6}, {0, 2, 4, 5}, {0, 2, 4, 6}, {0, 2, 5, 6}, {0, 3, 4, 5}, {0, 3, 4, 6}, {0, 3, 5, 6}, {0, 4, 5, 6}, {1, 2, 3, 4}, {1, 2, 3, 5}, {1, 2, 3, 6}, {1, 2, 4, 5}, {1, 2, 4, 6}, {1, 2, 5, 6}, {1, 3, 4, 5}, {1, 3, 4, 6}, {1, 3, 5, 6}, {1, 4, 5, 6}, {2, 3, 4, 5}, {2, 3, 4, 6}, {2, 3, 5, 6}, {2, 4, 5, 6}, {3, 4, 5, 6}}, {{0, 1, 2, 3, 4}, {0, 1, 2, 3, 5}, {0, 1, 2, 3, 6}, {0, 1, 2, 4, 5}, {0, 1, 2, 4, 6}, {0, 1, 2, 5, 6}, {0, 1, 3, 4, 5}, {0, 1, 3, 4, 6}, {0, 1, 3, 5, 6}, {0, 1, 4, 5, 6}, {0, 2, 3, 4, 5}, {0, 2, 3, 4, 6}, {0, 2, 3, 5, 6}, {0, 2, 4, 5, 6}, {0, 3, 4, 5, 6}, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 6}, {1, 2, 3, 5, 6}, {1, 2, 4, 5, 6}, {1, 3, 4, 5, 6}, {2, 3, 4, 5, 6}}},
                                                                                          {{}, {{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}},      {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {1, 7}, {2, 3}, {2, 4}, {2, 5}, {2, 6}, {2, 7}, {3, 4}, {3, 5}, {3, 6}, {3, 7}, {4, 5}, {4, 6}, {4, 7}, {5, 6}, {5, 7}, {6, 7}},                                                                 {{0, 1, 2}, {0, 1, 3}, {0, 1, 4}, {0, 1, 5}, {0, 1, 6}, {0, 1, 7}, {0, 2, 3}, {0, 2, 4}, {0, 2, 5}, {0, 2, 6}, {0, 2, 7}, {0, 3, 4}, {0, 3, 5}, {0, 3, 6}, {0, 3, 7}, {0, 4, 5}, {0, 4, 6}, {0, 4, 7}, {0, 5, 6}, {0, 5, 7}, {0, 6, 7}, {1, 2, 3}, {1, 2, 4}, {1, 2, 5}, {1, 2, 6}, {1, 2, 7}, {1, 3, 4}, {1, 3, 5}, {1, 3, 6}, {1, 3, 7}, {1, 4, 5}, {1, 4, 6}, {1, 4, 7}, {1, 5, 6}, {1, 5, 7}, {1, 6, 7}, {2, 3, 4}, {2, 3, 5}, {2, 3, 6}, {2, 3, 7}, {2, 4, 5}, {2, 4, 6}, {2, 4, 7}, {2, 5, 6}, {2, 5, 7}, {2, 6, 7}, {3, 4, 5}, {3, 4, 6}, {3, 4, 7}, {3, 5, 6}, {3, 5, 7}, {3, 6, 7}, {4, 5, 6}, {4, 5, 7}, {4, 6, 7}, {5, 6, 7}}, {{0, 1, 2, 3}, {0, 1, 2, 4}, {0, 1, 2, 5}, {0, 1, 2, 6}, {0, 1, 2, 7}, {0, 1, 3, 4}, {0, 1, 3, 5}, {0, 1, 3, 6}, {0, 1, 3, 7}, {0, 1, 4, 5}, {0, 1, 4, 6}, {0, 1, 4, 7}, {0, 1, 5, 6}, {0, 1, 5, 7}, {0, 1, 6, 7}, {0, 2, 3, 4}, {0, 2, 3, 5}, {0, 2, 3, 6}, {0, 2, 3, 7}, {0, 2, 4, 5}, {0, 2, 4, 6}, {0, 2, 4, 7}, {0, 2, 5, 6}, {0, 2, 5, 7}, {0, 2, 6, 7}, {0, 3, 4, 5}, {0, 3, 4, 6}, {0, 3, 4, 7}, {0, 3, 5, 6}, {0, 3, 5, 7}, {0, 3, 6, 7}, {0, 4, 5, 6}, {0, 4, 5, 7}, {0, 4, 6, 7}, {0, 5, 6, 7}, {1, 2, 3, 4}, {1, 2, 3, 5}, {1, 2, 3, 6}, {1, 2, 3, 7}, {1, 2, 4, 5}, {1, 2, 4, 6}, {1, 2, 4, 7}, {1, 2, 5, 6}, {1, 2, 5, 7}, {1, 2, 6, 7}, {1, 3, 4, 5}, {1, 3, 4, 6}, {1, 3, 4, 7}, {1, 3, 5, 6}, {1, 3, 5, 7}, {1, 3, 6, 7}, {1, 4, 5, 6}, {1, 4, 5, 7}, {1, 4, 6, 7}, {1, 5, 6, 7}, {2, 3, 4, 5}, {2, 3, 4, 6}, {2, 3, 4, 7}, {2, 3, 5, 6}, {2, 3, 5, 7}, {2, 3, 6, 7}, {2, 4, 5, 6}, {2, 4, 5, 7}, {2, 4, 6, 7}, {2, 5, 6, 7}, {3, 4, 5, 6}, {3, 4, 5, 7}, {3, 4, 6, 7}, {3, 5, 6, 7}, {4, 5, 6, 7}}, {{0, 1, 2, 3, 4}, {0, 1, 2, 3, 5}, {0, 1, 2, 3, 6}, {0, 1, 2, 3, 7}, {0, 1, 2, 4, 5}, {0, 1, 2, 4, 6}, {0, 1, 2, 4, 7}, {0, 1, 2, 5, 6}, {0, 1, 2, 5, 7}, {0, 1, 2, 6, 7}, {0, 1, 3, 4, 5}, {0, 1, 3, 4, 6}, {0, 1, 3, 4, 7}, {0, 1, 3, 5, 6}, {0, 1, 3, 5, 7}, {0, 1, 3, 6, 7}, {0, 1, 4, 5, 6}, {0, 1, 4, 5, 7}, {0, 1, 4, 6, 7}, {0, 1, 5, 6, 7}, {0, 2, 3, 4, 5}, {0, 2, 3, 4, 6}, {0, 2, 3, 4, 7}, {0, 2, 3, 5, 6}, {0, 2, 3, 5, 7}, {0, 2, 3, 6, 7}, {0, 2, 4, 5, 6}, {0, 2, 4, 5, 7}, {0, 2, 4, 6, 7}, {0, 2, 5, 6, 7}, {0, 3, 4, 5, 6}, {0, 3, 4, 5, 7}, {0, 3, 4, 6, 7}, {0, 3, 5, 6, 7}, {0, 4, 5, 6, 7}, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 6}, {1, 2, 3, 4, 7}, {1, 2, 3, 5, 6}, {1, 2, 3, 5, 7}, {1, 2, 3, 6, 7}, {1, 2, 4, 5, 6}, {1, 2, 4, 5, 7}, {1, 2, 4, 6, 7}, {1, 2, 5, 6, 7}, {1, 3, 4, 5, 6}, {1, 3, 4, 5, 7}, {1, 3, 4, 6, 7}, {1, 3, 5, 6, 7}, {1, 4, 5, 6, 7}, {2, 3, 4, 5, 6}, {2, 3, 4, 5, 7}, {2, 3, 4, 6, 7}, {2, 3, 5, 6, 7}, {2, 4, 5, 6, 7}, {3, 4, 5, 6, 7}}},
                                                                                          {{}, {{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}}, {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}, {0, 8}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {1, 7}, {1, 8}, {2, 3}, {2, 4}, {2, 5}, {2, 6}, {2, 7}, {2, 8}, {3, 4}, {3, 5}, {3, 6}, {3, 7}, {3, 8}, {4, 5}, {4, 6}, {4, 7}, {4, 8}, {5, 6}, {5, 7}, {5, 8}, {6, 7}, {6, 8}, {7, 8}}, {{0, 1, 2}, {0, 1, 3}, {0, 1, 4}, {0, 1, 5}, {0, 1, 6}, {0, 1, 7}, {0, 1, 8}, {0, 2, 3}, {0, 2, 4}, {0, 2, 5}, {0, 2, 6}, {0, 2, 7}, {0, 2, 8}, {0, 3, 4}, {0, 3, 5}, {0, 3, 6}, {0, 3, 7}, {0, 3, 8}, {0, 4, 5}, {0, 4, 6}, {0, 4, 7}, {0, 4, 8}, {0, 5, 6}, {0, 5, 7}, {0, 5, 8}, {0, 6, 7}, {0, 6, 8}, {0, 7, 8}, {1, 2, 3}, {1, 2, 4}, {1, 2, 5}, {1, 2, 6}, {1, 2, 7}, {1, 2, 8}, {1, 3, 4}, {1, 3, 5}, {1, 3, 6}, {1, 3, 7}, {1, 3, 8}, {1, 4, 5}, {1, 4, 6}, {1, 4, 7}, {1, 4, 8}, {1, 5, 6}, {1, 5, 7}, {1, 5, 8}, {1, 6, 7}, {1, 6, 8}, {1, 7, 8}, {2, 3, 4}, {2, 3, 5}, {2, 3, 6}, {2, 3, 7}, {2, 3, 8}, {2, 4, 5}, {2, 4, 6}, {2, 4, 7}, {2, 4, 8}, {2, 5, 6}, {2, 5, 7}, {2, 5, 8}, {2, 6, 7}, {2, 6, 8}, {2, 7, 8}, {3, 4, 5}, {3, 4, 6}, {3, 4, 7}, {3, 4, 8}, {3, 5, 6}, {3, 5, 7}, {3, 5, 8}, {3, 6, 7}, {3, 6, 8}, {3, 7, 8}, {4, 5, 6}, {4, 5, 7}, {4, 5, 8}, {4, 6, 7}, {4, 6, 8}, {4, 7, 8}, {5, 6, 7}, {5, 6, 8}, {5, 7, 8}, {6, 7, 8}}, {{0, 1, 2, 3}, {0, 1, 2, 4}, {0, 1, 2, 5}, {0, 1, 2, 6}, {0, 1, 2, 7}, {0, 1, 2, 8}, {0, 1, 3, 4}, {0, 1, 3, 5}, {0, 1, 3, 6}, {0, 1, 3, 7}, {0, 1, 3, 8}, {0, 1, 4, 5}, {0, 1, 4, 6}, {0, 1, 4, 7}, {0, 1, 4, 8}, {0, 1, 5, 6}, {0, 1, 5, 7}, {0, 1, 5, 8}, {0, 1, 6, 7}, {0, 1, 6, 8}, {0, 1, 7, 8}, {0, 2, 3, 4}, {0, 2, 3, 5}, {0, 2, 3, 6}, {0, 2, 3, 7}, {0, 2, 3, 8}, {0, 2, 4, 5}, {0, 2, 4, 6}, {0, 2, 4, 7}, {0, 2, 4, 8}, {0, 2, 5, 6}, {0, 2, 5, 7}, {0, 2, 5, 8}, {0, 2, 6, 7}, {0, 2, 6, 8}, {0, 2, 7, 8}, {0, 3, 4, 5}, {0, 3, 4, 6}, {0, 3, 4, 7}, {0, 3, 4, 8}, {0, 3, 5, 6}, {0, 3, 5, 7}, {0, 3, 5, 8}, {0, 3, 6, 7}, {0, 3, 6, 8}, {0, 3, 7, 8}, {0, 4, 5, 6}, {0, 4, 5, 7}, {0, 4, 5, 8}, {0, 4, 6, 7}, {0, 4, 6, 8}, {0, 4, 7, 8}, {0, 5, 6, 7}, {0, 5, 6, 8}, {0, 5, 7, 8}, {0, 6, 7, 8}, {1, 2, 3, 4}, {1, 2, 3, 5}, {1, 2, 3, 6}, {1, 2, 3, 7}, {1, 2, 3, 8}, {1, 2, 4, 5}, {1, 2, 4, 6}, {1, 2, 4, 7}, {1, 2, 4, 8}, {1, 2, 5, 6}, {1, 2, 5, 7}, {1, 2, 5, 8}, {1, 2, 6, 7}, {1, 2, 6, 8}, {1, 2, 7, 8}, {1, 3, 4, 5}, {1, 3, 4, 6}, {1, 3, 4, 7}, {1, 3, 4, 8}, {1, 3, 5, 6}, {1, 3, 5, 7}, {1, 3, 5, 8}, {1, 3, 6, 7}, {1, 3, 6, 8}, {1, 3, 7, 8}, {1, 4, 5, 6}, {1, 4, 5, 7}, {1, 4, 5, 8}, {1, 4, 6, 7}, {1, 4, 6, 8}, {1, 4, 7, 8}, {1, 5, 6, 7}, {1, 5, 6, 8}, {1, 5, 7, 8}, {1, 6, 7, 8}, {2, 3, 4, 5}, {2, 3, 4, 6}, {2, 3, 4, 7}, {2, 3, 4, 8}, {2, 3, 5, 6}, {2, 3, 5, 7}, {2, 3, 5, 8}, {2, 3, 6, 7}, {2, 3, 6, 8}, {2, 3, 7, 8}, {2, 4, 5, 6}, {2, 4, 5, 7}, {2, 4, 5, 8}, {2, 4, 6, 7}, {2, 4, 6, 8}, {2, 4, 7, 8}, {2, 5, 6, 7}, {2, 5, 6, 8}, {2, 5, 7, 8}, {2, 6, 7, 8}, {3, 4, 5, 6}, {3, 4, 5, 7}, {3, 4, 5, 8}, {3, 4, 6, 7}, {3, 4, 6, 8}, {3, 4, 7, 8}, {3, 5, 6, 7}, {3, 5, 6, 8}, {3, 5, 7, 8}, {3, 6, 7, 8}, {4, 5, 6, 7}, {4, 5, 6, 8}, {4, 5, 7, 8}, {4, 6, 7, 8}, {5, 6, 7, 8}}, {{0, 1, 2, 3, 4}, {0, 1, 2, 3, 5}, {0, 1, 2, 3, 6}, {0, 1, 2, 3, 7}, {0, 1, 2, 3, 8}, {0, 1, 2, 4, 5}, {0, 1, 2, 4, 6}, {0, 1, 2, 4, 7}, {0, 1, 2, 4, 8}, {0, 1, 2, 5, 6}, {0, 1, 2, 5, 7}, {0, 1, 2, 5, 8}, {0, 1, 2, 6, 7}, {0, 1, 2, 6, 8}, {0, 1, 2, 7, 8}, {0, 1, 3, 4, 5}, {0, 1, 3, 4, 6}, {0, 1, 3, 4, 7}, {0, 1, 3, 4, 8}, {0, 1, 3, 5, 6}, {0, 1, 3, 5, 7}, {0, 1, 3, 5, 8}, {0, 1, 3, 6, 7}, {0, 1, 3, 6, 8}, {0, 1, 3, 7, 8}, {0, 1, 4, 5, 6}, {0, 1, 4, 5, 7}, {0, 1, 4, 5, 8}, {0, 1, 4, 6, 7}, {0, 1, 4, 6, 8}, {0, 1, 4, 7, 8}, {0, 1, 5, 6, 7}, {0, 1, 5, 6, 8}, {0, 1, 5, 7, 8}, {0, 1, 6, 7, 8}, {0, 2, 3, 4, 5}, {0, 2, 3, 4, 6}, {0, 2, 3, 4, 7}, {0, 2, 3, 4, 8}, {0, 2, 3, 5, 6}, {0, 2, 3, 5, 7}, {0, 2, 3, 5, 8}, {0, 2, 3, 6, 7}, {0, 2, 3, 6, 8}, {0, 2, 3, 7, 8}, {0, 2, 4, 5, 6}, {0, 2, 4, 5, 7}, {0, 2, 4, 5, 8}, {0, 2, 4, 6, 7}, {0, 2, 4, 6, 8}, {0, 2, 4, 7, 8}, {0, 2, 5, 6, 7}, {0, 2, 5, 6, 8}, {0, 2, 5, 7, 8}, {0, 2, 6, 7, 8}, {0, 3, 4, 5, 6}, {0, 3, 4, 5, 7}, {0, 3, 4, 5, 8}, {0, 3, 4, 6, 7}, {0, 3, 4, 6, 8}, {0, 3, 4, 7, 8}, {0, 3, 5, 6, 7}, {0, 3, 5, 6, 8}, {0, 3, 5, 7, 8}, {0, 3, 6, 7, 8}, {0, 4, 5, 6, 7}, {0, 4, 5, 6, 8}, {0, 4, 5, 7, 8}, {0, 4, 6, 7, 8}, {0, 5, 6, 7, 8}, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 6}, {1, 2, 3, 4, 7}, {1, 2, 3, 4, 8}, {1, 2, 3, 5, 6}, {1, 2, 3, 5, 7}, {1, 2, 3, 5, 8}, {1, 2, 3, 6, 7}, {1, 2, 3, 6, 8}, {1, 2, 3, 7, 8}, {1, 2, 4, 5, 6}, {1, 2, 4, 5, 7}, {1, 2, 4, 5, 8}, {1, 2, 4, 6, 7}, {1, 2, 4, 6, 8}, {1, 2, 4, 7, 8}, {1, 2, 5, 6, 7}, {1, 2, 5, 6, 8}, {1, 2, 5, 7, 8}, {1, 2, 6, 7, 8}, {1, 3, 4, 5, 6}, {1, 3, 4, 5, 7}, {1, 3, 4, 5, 8}, {1, 3, 4, 6, 7}, {1, 3, 4, 6, 8}, {1, 3, 4, 7, 8}, {1, 3, 5, 6, 7}, {1, 3, 5, 6, 8}, {1, 3, 5, 7, 8}, {1, 3, 6, 7, 8}, {1, 4, 5, 6, 7}, {1, 4, 5, 6, 8}, {1, 4, 5, 7, 8}, {1, 4, 6, 7, 8}, {1, 5, 6, 7, 8}, {2, 3, 4, 5, 6}, {2, 3, 4, 5, 7}, {2, 3, 4, 5, 8}, {2, 3, 4, 6, 7}, {2, 3, 4, 6, 8}, {2, 3, 4, 7, 8}, {2, 3, 5, 6, 7}, {2, 3, 5, 6, 8}, {2, 3, 5, 7, 8}, {2, 3, 6, 7, 8}, {2, 4, 5, 6, 7}, {2, 4, 5, 6, 8}, {2, 4, 5, 7, 8}, {2, 4, 6, 7, 8}, {2, 5, 6, 7, 8}, {3, 4, 5, 6, 7}, {3, 4, 5, 6, 8}, {3, 4, 5, 7, 8}, {3, 4, 6, 7, 8}, {3, 5, 6, 7, 8}, {4, 5, 6, 7, 8}}}};

polygon_t::polygon_t(const bounding_rectangle_t &rect) :
        bounding_rect(rect.getRect()),
        origin_unit_tree(rect.getRect())
        {
    this->origin_unit_tree.add_value(polygon_module_t(rect));
    auto set = std::set<polygon_module_t*>({origin_unit_tree.get_values().front().get()});
    this->origin_unit_tree.get_values().front()->set_area(set);
    this->unit_lib[set] = this->origin_unit_tree.get_values().front().get();
}

const rect_t &polygon_t::get_bounding_rect() const {
    return bounding_rect;
}

const std::vector<std::shared_ptr<polygon_module_t>> &polygon_t::get_rects() const {
    return this->origin_unit_tree.get_values();
}

const std::vector<std::shared_ptr<polygon_module_t>> &polygon_t::get_overlap_unit() const {
    return this->overlap_unit;
}

bool polygon_t::is_bounding_collision(const rect_t &rect) const {
    return this->bounding_rect.is_collision(rect);
}

bool polygon_t::is_bounding_collision(const bounding_rectangle_t &rect) const {
    return this->is_bounding_collision(rect.getRect());
}

bool polygon_t::is_collision(const rect_t &rect) const {
    //todo quad tree

    return std::any_of(this->origin_unit_tree.get_values().begin(), this->origin_unit_tree.get_values().end(),
                       [&rect](const std::shared_ptr<polygon_module_t> &r) {
                           return r->get_module_bounding_rectangle().getRect().is_collision(rect);
                       });
}

bool polygon_t::is_collision(const bounding_rectangle_t &rect) const {
    return this->is_collision(rect.getRect());
}

const std::shared_ptr<polygon_module_t> &polygon_t::get_origin_unit() {
    return this->origin_unit_tree.get_values().front();
}

bool polygon_t::merge_polygon(polygon_t &polygon) {
    auto coli_share = polygon.origin_unit_tree.collision_value(this->get_origin_unit());
    auto coli = std::vector<polygon_module_t*>();
    for(auto share : coli_share){
        coli.push_back(share.get());
    }
    std::map<polygon_module_t*, polygon_module_t*> old_2_new;
    for (const auto& ori : polygon.origin_unit_tree.get_values()){
        this->origin_unit_tree.add_value(*ori.get());
        old_2_new[ori.get()] = this->origin_unit_tree.get_values().back().get();
    }
    for (const auto& over : polygon.overlap_unit){
        this->overlap_unit.push_back(std::make_shared<polygon_module_t>(*over.get()));
        old_2_new[over.get()] = this->overlap_unit.back().get();
    }
    for (const auto& lib : polygon.unit_lib){
        std::set<polygon_module_t*> new_set;
        for (auto set : lib.first){
            new_set.insert(old_2_new[set]);
        }
        this->unit_lib[new_set] = old_2_new[lib.second];
    }

    for (auto& orii : this->unit_lib){
        auto& ori = orii.second;
        auto ar = ori->area_from_where;
        ori->area_from_where.clear();
        for (auto area : ar){
            if (old_2_new.find(area.first) != old_2_new.end()){
                ori->area_from_where[old_2_new[area.first]] = area.second;
            }else{
                ori->area_from_where[area.first] = area.second;
            }
        }
        auto co = ori->connections;
        ori->connections.clear();
        for (auto conn : co){
            if (old_2_new.find(conn) != old_2_new.end()){
                ori->connections.push_back(old_2_new[conn]);
            }else{
                ori->connections.push_back(conn);
            }
        }
    }

    for (auto& co : coli){
        if (old_2_new.find(co) != old_2_new.end()){
            co = old_2_new[co];
        }
    }

    for (auto co : coli){
        if(co->get_bounding_rect().is_wrap(this->get_origin_unit()->get_bounding_rect()) ||
            this->get_origin_unit()->get_bounding_rect().is_wrap(co->get_bounding_rect())){
            return false;
        }
    }

    coli.push_back(this->get_origin_unit().get());

    for (int i = 1; i < 3; ++i) {
        auto c = polygon_t::combination_list[coli.size() - 1][i];
        for (auto sets: c) {
            std::set<polygon_module_t*> key_set;
            for (auto set: sets) {
                key_set.insert(coli[set]);
            }
            if (this->unit_lib.find(key_set) == this->unit_lib.end()) continue;

            auto find = this->unit_lib[key_set];

            auto new_rect = find->get_bounding_rect().intersect(
                    this->get_origin_unit()->get_bounding_rect());

            if(i == 2 && new_rect.first){
                return false;
            }

            if(!new_rect.first) continue;

            const polygon_module_t new_unit = polygon_module_t(bounding_rectangle_t(new_rect.second));

            this->overlap_unit.push_back(std::make_shared<polygon_module_t>(new_unit));
            key_set.insert(this->get_origin_unit().get());
            this->unit_lib[key_set] = this->overlap_unit.back().get();
            this->overlap_unit.back()->set_area_from_where(find->set_overlap_take(this->get_origin_unit().get(), new_unit.get_bounding_rect().get_area()));

            std::vector<polygon_module_t*> key_set_vec(key_set.begin(), key_set.end());
            int sign = 1;
            for (int j = key_set.size() - 1; j >= 1; --j) {
                sign *= -1;
                auto sub_combi = polygon_t::combination_list[key_set.size()][j];
                for (auto sub_sets : sub_combi){
                    std::set<polygon_module_t*> sub_key_set;
                    for (auto sub_set: sub_sets) {
                        sub_key_set.insert(key_set_vec[sub_set]);
                    }
                    this->unit_lib[sub_key_set]->fix_max_area(sign * new_unit.get_bounding_rect().get_area());
                }
            }

            auto conn_combi = polygon_t::combination_list[key_set.size()][key_set.size() - 1];
            for (auto sub_conn : conn_combi){
                std::set<polygon_module_t*> conn_key_set;
                for (auto conn_set: sub_conn) {
                    conn_key_set.insert(key_set_vec[conn_set]);
                }
                auto& conn_find = this->unit_lib[conn_key_set];
                conn_find->add_connection(this->unit_lib[key_set]);
                this->unit_lib[key_set]->add_connection(conn_find);
            }
        }
    }


//    for (int i = 1; i < 4; ++i) {
//        auto c = polygon_t::combination_list[coli.size() - 1][i];
//        for (auto sets: c) {
//            std::set<polygon_module_t*> key_set;
//            for (auto set: sets) {
//                key_set.insert(coli[set]);
//            }
//
//            if (this->unit_lib.find(key_set) == this->unit_lib.end()) continue;
//
//            auto find = this->unit_lib[key_set];
//
//            key_set.insert(this->get_origin_unit().get());
//            auto new_unit = this->unit_lib[key_set];
//            new_unit->set_area_from_where(find->set_overlap_take(this->get_origin_unit().get(), new_unit->max_area));
//        }
//    }

    this->get_origin_unit()->fix_area_reset();
    int left = this->get_origin_unit()->fix_area(this->get_origin_unit().get(), this->get_origin_unit()->get_module_bounding_rectangle().getLinkModule()->get_area());

    this->bounding_rect = this->bounding_rect.merge_bounding_rect(polygon.bounding_rect);

    return left == 0;
}

void polygon_t::print() {
    std::map<polygon_module_t*, std::string> unit_to_name;
    for (int i = 0; i < this->origin_unit_tree.get_values().size(); ++i) {
        unit_to_name[this->origin_unit_tree.get_values()[i].get()] = std::string(1, 'A' + i);
    }
    for (auto unit_pair : this->unit_lib){
        std::string name_set = "";
        for (auto units : unit_pair.first){
            name_set += unit_to_name[units] + " ";
        }
        std::cout << std::setw(8) << name_set;
        std::cout << " | ";

        std::cout << std::setw(2) << unit_pair.second->max_area << " | ";

        std::cout << unit_pair.second->get_bounding_rect();

        std::cout << " { ";
        for (auto area : unit_pair.second->get_area_from_where()) {
            std::cout << unit_to_name[area.first] << " : " << area.second << ", ";
        }
        std::cout << " }";

        std::cout << "\n";
    }
}