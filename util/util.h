#ifndef UTIL_H
#define UTIL_H
#include <ctime>
#include <fstream>
#include <map>
#include <list>
#include <diff/diff.h>
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


//key value to json file
int mapToJson(std::map<std::string, std::list<std::string> > &key_values, std::ofstream& out);
int corrToJson(std::list<DiffCorresResult>& correspond,
               std::ofstream& out);
//std::string UriDecode(const std::string & sSrc);

int topRateToJson(std::vector<double>& rate,std::string& json);
#endif // UTIL_H
