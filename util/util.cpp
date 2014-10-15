#include "util/util.h"
#include <cstdio>
#include <iostream>
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
int corrToJson(std::list<DiffCorresResult>& correspond,
               std::ofstream& out)
{
    using namespace std;
    list<DiffCorresResult>::iterator iter_diff = correspond.begin();
    list<CorrRelation>::iterator iter_corr;
    list<CorrRelation>::iterator iter_corr_end;
    //the correspond

    //first
    out<<"[";
    out<<"{"<<"\"query\""<<":"
       <<"\""<<iter_diff->key<<"\""<<",";
    out<<"\""<<"correspond"<<"\""<<":";
    out<<"[{";
    iter_corr = iter_diff->correspond.begin();
    out<<"\""<<"current"<<"\""<<":"<<iter_corr->first_position
       <<",\""<<"previous"<<"\""<<":"<<iter_corr->second_position;
    out<<"}";
    iter_corr_end = iter_diff->correspond.end();
    ++iter_corr;
    for( ;iter_corr != iter_corr_end;++iter_corr)
    {
        out<<",\""<<"{"
           <<"\""<<"current"<<"\""<<":"<<iter_corr->first_position
           <<",\""<<"previous"<<"\""<<":"<<iter_corr->second_position
           <<"}";

    }
    out<<"]}";
    ++iter_diff;
    for( ;iter_diff != correspond.end();++iter_diff)
    {
        out<<",\n{";
        out<<"\"query\""<<":"
           <<"\""<<iter_diff->key<<"\""<<",";
        out<<"\""<<"correspond"<<"\""<<":";
        out<<"[{";
        iter_corr = iter_diff->correspond.begin();
        out<<"\""<<"current"<<"\""<<":"<<iter_corr->first_position
           <<",\""<<"previous"<<"\""<<":"<<iter_corr->second_position;
        out<<"}";
        iter_corr_end = iter_diff->correspond.end();
        ++iter_corr;
        for( ;iter_corr != iter_corr_end;++iter_corr)
        {
            out<<",\""<<"{"
               <<"\""<<"current"<<"\""<<":"<<iter_corr->first_position
               <<",\""<<"previous"<<"\""<<":"<<iter_corr->second_position
               <<"}";

        }
        out<<"]}";
    }

    out<<"]"<<endl;
    return 0;
}

int mapToJson(std::map<std::string, std::list<std::string> >& key_values, std::ofstream &out)
{
    using namespace std;
    if(key_values.empty())
    {
        cout<<"Empty paratemers in mapToJson:function!"<<endl;
        return -1;
    }
    map<string,list<string> >::iterator iter = key_values.begin();
    map<string,list<string> >::iterator iter_end = key_values.end();

    out<<"[";
    list<string>::iterator iter_value;

    string json_start= "{\"query\":";

    //the first
    out<<json_start<<"\""<<iter->first<<"\""<<",";

    iter_value = iter->second.begin();
    out<<"\""<<"result"<<"\""<<":"<<"["
       <<"\""<<*iter_value<<"\"";
    ++iter_value;
    for( ;iter_value!=iter->second.end();++iter_value)
    {
        out<<","<<"\""<<*iter_value<<"\"";
        cout<<*iter_value<<endl;
    }
    out<<"]}";

    //the leaving items
    ++iter;
    for( ; iter != iter_end;++iter)
    {
        out<<",\n"<<json_start<<"\""<<iter->first<<"\""<<",";

        iter_value = iter->second.begin();
        out<<"\""<<"result"<<"\""<<":"<<"["
           <<"\""<<*iter_value<<"\"";
        ++iter_value;
        for( ;iter_value!=iter->second.end();++iter_value)
        {
            out<<","<<"\""<<*iter_value<<"\"";
        }
        out<<"]}";
    }
    out<<"]"<<endl;
    return 0;
}
