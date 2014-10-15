/*
 * =====================================================================================
 *
 *       Filename:  diff.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/13/2014 02:12:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yu Daoming (mn), yudaoming@360.cn
 *        Company:  Qihoo 360 LTD.
 *
 * =====================================================================================
 */

#include "diff.h"
#include <vector>
#include <functional>

using namespace std;

int diffSearch(const MapUrlList &first_key_urls,  MapUrlList &sec_key_urls, std::list<DiffCorresResult> &result)
{
    map<string,list<string> >::const_iterator first_map_iter = first_key_urls.begin();


    map<string,list<string> >::const_iterator first_map_end = first_key_urls.end();

    DiffCorresResult temp;
    for( ;first_map_iter!=first_map_end; ++first_map_iter)
    {
        temp.key = first_map_iter->first;
        diffCorrespond(first_map_iter->second,sec_key_urls[first_map_iter->first],temp.correspond);
        if(temp.correspond.empty())
            continue;
        result.push_back(temp);
    }
    return 0;
}

//the core function
int diffCorrespond(const list<string> urls1,
                   const list<string> urls2,
                   std::list<CorrRelation> &result)
{
    if(urls1.empty()||urls2.empty())
    {
        cout<<"Empty urls in diffCorressond function!"<<endl;
        return -1;
    }
    list<string>::const_iterator iter1 = urls1.begin();
    list<string>::const_iterator urls1_end = urls1.end();

    list<string>::const_iterator iter2 = urls2.begin();
    list<string>::const_iterator urls2_end = urls2.end();
    vector<list<string>::const_iterator> diff_iter;


    CorrRelation temp_relation;
    result.clear();
    //get begining of the difference of the two versions;
    temp_relation.second_position = -1;
    for(int i=0;iter1 != urls1_end,iter2 != urls2_end;++i,++iter1,++iter2)
    {
        if(iter1->compare(*iter2)!=0)
        {
            temp_relation.first_position = i;
            result.push_back(temp_relation);
            diff_iter.push_back(iter1);
        }
    }
    if(diff_iter.size()==0)
    {
        result.clear();
        return 1;
    }

    vector<list<string>::const_iterator> ::iterator iter_diff = diff_iter.begin();
    list<CorrRelation>::iterator iter_res = result.begin();
    //get the position changing between the two versions
    for( ;iter_res != result.end(),iter_diff!=diff_iter.end();++iter_res ,++iter_diff)
    {
        iter2 = urls2.begin();
        iter1 = *iter_diff;
        for(int j=0 ;iter2 != urls2_end;++iter2,++j)
        {
            if(iter1->compare(*iter2)==0)
            {
                iter_res->second_position = j;
                continue;
            }
        }
    }
    return 0;
}


