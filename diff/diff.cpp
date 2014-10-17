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

int diffSearch(const MapUrlList &first_key_urls, MapUrlList &sec_key_urls, std::list<DiffCorresResult> &result)
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
int diffCorrespond(const list<string>& urls1,
                   const list<string>& urls2,
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
    for(int i=1;iter1 != urls1_end&&iter2 != urls2_end;++i,++iter1,++iter2)
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
    for( ;iter_res != result.end()&&iter_diff!=diff_iter.end();++iter_res ,++iter_diff)
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

//get the every top n urls
int diffTop(const MapUrlList &key_urls1,
            const MapUrlList &key_urls2,
            const list<DiffCorresResult> &diffCorres,
            map<int,list<Top> >& topN1,
            map<int,list<Top> >& topN2)
{
    //the relation list of the two different versions
    list<DiffCorresResult>::const_iterator iter = diffCorres.begin();
    list<DiffCorresResult>::const_iterator diff_end = diffCorres.end();
    //all the difference urls iter
    std::map<std::string,std::list<std::string> >::const_iterator map_iter1;
    std::map<std::string,std::list<std::string> >::const_iterator map_iter2;
    //the given key's urls
    list<CorrRelation>::const_iterator iter_corr;

    //the real urls iter;
    std::list<string>::const_iterator urls_iter1;
    std::list<string>::const_iterator urls_iter2;

    int position;
    Top temp_top1,temp_top2;
    list<string> *temp_urls1 = &temp_top1.urls;
    list<string> *temp_urls2 = &temp_top2.urls;

    map<int,list<Top> >::iterator result_iter1;
    map<int,list<Top> >::iterator result_iter2;

    for( ; iter!= diff_end; ++iter)
    {


        iter_corr = iter->correspond.begin();//the diff relation list;

        //find the urls list  of the given top
        map_iter1 = key_urls1.find(iter->key);
        map_iter2 = key_urls2.find(iter->key);
        //the urls from the key_urls;
        urls_iter1 = map_iter1->second.begin();
        urls_iter2 = map_iter2->second.begin();

        temp_top1.key = iter->key;
        temp_top2.key = iter->key;

        temp_urls1->clear();
        temp_urls2->clear();
        for( ; iter_corr != iter->correspond.end();++iter_corr)
        {
            position = iter_corr->first_position;
            for(int i=0;i<position+1;i++)
            {
                temp_urls1->push_back(*urls_iter1);
                temp_urls2->push_back(*urls_iter2);
            }
            topN1[position];
            topN2[position];
            result_iter1 = topN1.find(position);
            result_iter2 = topN2.find(position);

            result_iter1->second.push_back(temp_top1);
            result_iter2->second.push_back(temp_top2);
            temp_urls1->clear();
            temp_urls2->clear();
        }

    }
    return 0;
}



























