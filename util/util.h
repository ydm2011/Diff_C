#ifndef UTIL_H
#define UTIL_H
#include <ctime>
class TimeStatcis
{
public:
    TimeStatcis(const char* test_term);
    ~TimeStatcis();
private:
    time_t time_eclipse;
    const char* test_term;
    time_t time_start;
};

inline bool is_character(char c)
{
    if((c>='a'&&c<='z')||(c>='A'&&c<'Z'))
        return true;
    return false;
}

#endif // UTIL_H
