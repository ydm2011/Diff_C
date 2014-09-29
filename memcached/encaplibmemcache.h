/*
 * =====================================================================================
 *
 *       Filename:  encaplibmemcache.h
 *
 *    Description:  this file just encap the libmemcached by c++
 *
 *        Version:  1.0
 *        Created:  2014年09月03日 14时49分41秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yu daoming (), ydm2011@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef ENCAPLIBMEMCACHE_H
#define ENCAPLIBMEMCACHE_H
#include <iostream>
#include "memcached.h"
#include <vector>
#include  <memory>
#include <map>

enum NetProtocol{TCP,UDP,UNIX};
enum {MALLOC,ARRAY};
enum ReturnStatus{MEM_SUCCESS,MEM_FAILED,MEM_ERROR};
/*
 * this struct is the MemManipulateParam 
 */
struct MemManipulateParam{
    MemManipulateParam(time_t extime=0,uint32_t _flags=0):\
        expirationTime(extime),flags(_flags){
    }
    time_t expirationTime;
    uint32_t flags;
};
/*
 * this template class could free the stack memory that
 * not allocted by new;
 */ 
template<class FreeType>
class FreeNotByNew{
public:
    FreeNotByNew(int _mode=MALLOC):mode(_mode)
    {
        ;
    }
    void operator()(FreeType*);
private:
    int mode;
};
/*
 * this was the result fetched from the memcached server
 */
struct ResultFromMemcache{
public:
    ResultFromMemcache();
    ResultFromMemcache(char* _key,size_t _keyLength,char* _value,\
                       size_t _valueLength,uint32_t flags=0);
    std::string key;
    std::shared_ptr<char> value;
    size_t keyLength;
    size_t valueLength;
    uint32_t flags;
};
class EncapLibMemcached{
public:
    //constucted by the given configparameters
    EncapLibMemcached(const std::string& configParameters="--SERVER=localhost:11211",
                      const MemManipulateParam& mmparams=MemManipulateParam());
    //constructed by the internel memcached function ,such as the
    //memcached,memcacehd_create,memcached_clone and so on
    EncapLibMemcached(std::shared_ptr<memcached_st> initializedServer,
                      const MemManipulateParam& mmParam);
    //modify the key and value of the memcached;
    int set(const std::string& key,const std::string& value);
    //add the noexist key and value pair to the memcached
    int add(const std::string& key, const std::string& value);
    //replace the existed key's value
    int replace(const std::string& key,const std::string& value);
    //append new value after the existing key's  value
    int append(const std::string& key,const std::string& value);
    //insert before the the existing key's  value .Example:the existing
    // key and value is : test_key  test_value ; now we want to prepend the
    //test_value2 before the test_value,after call this method the key-value pair is
    //test_key test_value2 test_value2
    int prepend(const std::string& key,const std::string& value);
    //increment the numberic value of the given key
    int increment(const std::string& key,std::uint32_t offset,std::uint64_t& result);
    //decrement the numberic value of the given key
    int decrement(const std::string& key,std::uint32_t offset,std::uint64_t& result);
    //delete the given key and its value
    int delete_data(const std::string& key);
    //check the unique of the key and set
    int cas(const std::string& key,const std::string& value,std::uint64_t casUnique);
    //to judge whether the given key is existing or not
    int key_exist(const std::string& key);

    //get the memcached instance;
    std::shared_ptr<memcached_st> getMemcached();
    //get infomation from the server
    const MemManipulateParam& get_configurtaion();
    //retrieving data from the server
    int get(const std::string& key,std::shared_ptr<char>& returnResult);
    //send some keys to the memcached server to process
    int muti_get(const std::vector<std::string>& keys);
    //get the result of the memcached processing result,so this method must call after the
    //muti_get method
    int fetch_result(std::map<std::string,std::shared_ptr<ResultFromMemcache> >&\
                             results);
    //add the sever to the current object
    int add_server(const std::string& host,in_port_t port,\
                           NetProtocol ptotocol=TCP);
    //deconstructor
    ~EncapLibMemcached();
protected:
    //forbiden copy
    EncapLibMemcached(const EncapLibMemcached& memc);
    EncapLibMemcached& operator=(EncapLibMemcached& memc);
    //flush the command in the client buffer
    int flush_client();
    //parse the return status information
    int parse_status(const char* status,size_t length);
    //disconnect the memcahced server
    void mem_quit();
private:
    //the return info strucor
    memcached_return_t return_info;
    //memcached_st structor
    std::shared_ptr<memcached_st> smartMemcPointer;
    //the default parameter:expirtion time and flags
    MemManipulateParam manipuParam;
};

#endif
