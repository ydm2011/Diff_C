#include "util/util.h"
#include <cstdio>
TimeStatcis::TimeStatcis(const char* test_term)
    :time_eclipse(0)
{
    this->test_term = test_term;
    time_start = clock();
}
TimeStatcis::~TimeStatcis()
{
    time_eclipse = clock();
    time_eclipse -=time_start;
    printf("%s runtime: %f s",
           test_term,
           double(time_eclipse/CLOCKS_PER_SEC));
}
