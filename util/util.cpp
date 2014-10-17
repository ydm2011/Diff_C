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

//these function are ugly just accomplish the certain goals
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
    out<<"[";
    iter_corr = iter_diff->correspond.begin();
    out<<"["<<iter_corr->first_position
       <<","<<iter_corr->second_position<<"]";
    //out<<"]";
    iter_corr_end = iter_diff->correspond.end();
    ++iter_corr;
    for( ;iter_corr != iter_corr_end;++iter_corr)
    {
        out<<","
           <<"["<<iter_corr->first_position
           <<","<<iter_corr->second_position<<"]";

    }
    out<<"]}";
    ++iter_diff;
    for( ;iter_diff != correspond.end();++iter_diff)
    {
        out<<",\n{";
        out<<"\"query\""<<":"
           <<"\""<<iter_diff->key<<"\""<<",";
        out<<"\""<<"correspond"<<"\""<<":";
        out<<"[";
        iter_corr = iter_diff->correspond.begin();
        out<<"["<<iter_corr->first_position
           <<","<<iter_corr->second_position<<"]";
        //out<<"}";
        iter_corr_end = iter_diff->correspond.end();
        ++iter_corr;
        for( ;iter_corr != iter_corr_end;++iter_corr)
        {
            out<<","<<"["
               <<iter_corr->first_position
               <<","<<iter_corr->second_position
               <<"]";

        }
        out<<"]}";
    }

    out<<"]"<<endl;
    return 0;
}
//write this to json
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


int topToJsonNode(const Top& top,
                  std::string& result)
{
    using namespace std;
    result.clear();
    result +="{\"query\":\"";
    result +=top.key+"\",";
    result +="\"url\":[";
    list<string>::const_iterator iter = top.urls.begin();
    list<string>::const_iterator iter_end = top.urls.end();
    for( ;iter != iter_end; ++iter)
    {
        result += "\""+*iter+"\",";
    }
    result.pop_back();
    result +="},";
    return 0;
}

//
int mapTopToJson(const std::map<int,std::list<Top> > &top_n,std::ofstream& out)
{
    using namespace std;
    if(top_n.empty())
    {
        cout<<"Empty paratemers in mapTopToJson:function!"<<endl;
        return -1;
    }
    int top_num = top_n.size();

    out<<"[[";
    list<Top>::const_iterator top_iter;
    map<int,list<Top> >::const_iterator map_iter;
    string temp;
    for(int i=0;i<top_num; i++)
    {
        map_iter = top_n.find(i);
        top_iter = map_iter->second.begin();
        topToJsonNode(*top_iter,temp);
    }
    return 0;
}

//get the changing rate of each result item;
int getChangeRate(std::map<int,std::list<Top> >&topN,int key_num,std::list<double>& rate)
{
    using namespace std;
    if(key_num ==0)
    {
        cout<<"Wrong parameter in the getChangeRate: function! ";
        return -1;
    }
    for(int i=1; i< topN.size()+1;i++)
    {
        rate.push_back(topN[i].size()/key_num);
    }
    return 0;
}
