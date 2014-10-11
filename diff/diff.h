/*
 * =====================================================================================
 *
 *       Filename:  diff.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/11/2014 07:32:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yu Daoming (mn), yudaoming@360.cn
 *        Company:  Qihoo 360 LTD.
 *
 * =====================================================================================
 */
#ifndef DIFF_C_DIFF_H
#define DIFF_C_DIFF_H
#include <iostream>
#include <map>
#include <string>

struct CorresResult{
    CorresResult():first_position(-1),second_position(-1){}
    int first_position;
    int second_position;
};
struct DiffCorresResult{
    
    std::string key;
    std::list<CorresResult> correspond;
}

typedef std::map<std::string,std::list<std::string> > MapUrlList;
//return all the diff search result and return the change relations 
//between the two versions;
int diffSearch(const MapUrlList& first_key_urls,
               const MapUrlList& sec_key_urls,
               std::list<DiffCorresResult>& result);
//get the difference of the given list
int diffCorrespond(std::list<std::string> urls1,
                   std::list<std::string> urls2,
                   std::list<CorresResult>& result);
