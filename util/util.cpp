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
    if(correspond.empty())
    {
        cout<<"There are not difference between the two versions!"<<endl;
        return 0;
    }
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
    iter_value = iter->second.begin();
    if(iter_value == iter->second.end())
        return -1;

    string json_start= "{\"query\":";

    //the first
    out<<json_start<<"\""<<iter->first<<"\""<<",";


    size_t bad_position=-1;
    bad_position = iter_value->find("\r\n");
    if(bad_position != -1)
        iter_value->erase(bad_position,8);
    out<<"\""<<"result"<<"\""<<":"<<"["
       <<"\""<<*iter_value<<"\"";
    ++iter_value;
    for( ;iter_value!=iter->second.end();++iter_value)
    {
        //iter_value->find_first_of('\\',)
        bad_position = iter_value->find("\r\n");
        if(bad_position != -1)
            iter_value->erase(bad_position,8);
        out<<","<<"\""<<*iter_value<<"\"";
        //cout<<*iter_value<<endl;
    }
    out<<"]}";

    //the leaving items
    ++iter;
    if(iter==iter_end)
    {
        out<<"]"<<endl;
        return -1;
    }
    for( ; iter != iter_end;++iter)
    {
        iter_value = iter->second.begin();
        if(iter_value ==iter->second.end())
            continue;
        bad_position = iter_value->find("\r\n");
        if(bad_position != -1)
            iter_value->erase(bad_position,8);
        out<<",\n"<<json_start<<"\""<<iter->first<<"\""<<",";


        out<<"\""<<"result"<<"\""<<":"<<"["
           <<"\""<<*iter_value<<"\"";
        ++iter_value;
        for( ;iter_value!=iter->second.end();++iter_value)
        {
            bad_position = iter_value->find("\r\n");
            if(bad_position != -1)
                iter_value->erase(bad_position,8);
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
int getChangeRate(std::list<DiffCorresResult>& diff_result, int key_num,std:: vector<double>& rate)
{
    using namespace std;
    rate.resize(20);
    if(key_num ==0||diff_result.empty())
    {
        cout<<"Wrong parameter in the getChangeRate: function! ";
        return -1;
    }
    //double change_num = 0;
    list<DiffCorresResult>::iterator iter = diff_result.begin();
    list<DiffCorresResult>::iterator iter_end = diff_result.end();

    list<CorrRelation>::iterator corr_iter;
    int max;
    for( ; iter != iter_end; ++iter)
    {
        //cout<<topN[i].size();
        corr_iter = iter->correspond.begin();
        rate[corr_iter->first_position] +=1;
        if(max<corr_iter->first_position)
        {
            max = corr_iter->first_position;
        }
    }
    vector<double>::iterator rate_iter = rate.begin();
    rate.erase(rate_iter+max+1,rate_iter+rate.size());
    rate_iter = rate.begin();
    ++rate_iter;
    for( ;rate_iter != rate.end(); ++rate_iter)
    {
        *rate_iter = (*rate_iter)/key_num +*(rate_iter-1);
    }
    rate_iter = rate.begin()+1;
    for( ; rate_iter != rate.end(); ++rate_iter)
    {
         *rate_iter = 1-*rate_iter;
    }
    return 0;
}
//topRateToJson
int topRateToJson(std::vector<double>& rate,std::string& json)
{
    using namespace std;
    if(rate.empty())
    {
        std::cout<<"not element in rate!"<<endl;
        return -1;
    }
    using namespace std;
    string tag = "Top";
    json += "[";
    for(int i=1;i<rate.size();++i)
    {
        json += "{\"";
        json += tag;
        json +=to_string(i);
        json += "\":";
        json += to_string(rate[i]);
        json += "},";
    }
    json.pop_back();
    json +="]";
}

