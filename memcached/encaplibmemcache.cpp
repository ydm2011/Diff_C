/*
 * =====================================================================================
 *
 *       Filename:  encaplibmemcache.cpp
 *
 *    Description: this is the defination of the  EncapLibMemcached cla                   class; 
 *
 *        Version:  1.0
 *        Created:  2014年09月04日 10时11分14秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yu daoming (), ydm2011@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "encaplibmemcache.h"
using namespace std;
/* this struct store the  result fetched  from the server by key; each instance
 * contain the key and the value;
 */
ResultFromMemcache::ResultFromMemcache():key((char*)0),value((char*)0),\
                            keyLength(0),valueLength(0),flags(0){}
ResultFromMemcache::ResultFromMemcache(char* _key,
                                       size_t _keyLength,
                                       char* _value,
                                       size_t _valueLength,
                                       uint32_t _flags):key(_key,_keyLength)
{

    shared_ptr<char> tmp(_value,free);

    value = tmp;
    valueLength = _valueLength;
    keyLength = _keyLength;
    flags = _flags;
}

/*
 *
 * this is util class free the stack memory that not allocated by new operator
 */
template<class FreeType>
void FreeNotByNew<FreeType>::operator ()(FreeType* instance)
{
    if(mode == MALLOC )
    {
        free(instance);
        return;
    }
    if(mode == ARRAY)
    {
        delete[] instance;
    }
}


/*****************************************************************************/
/*
 * this constructor function is construct by the --SERVER=<servername>:<optional port>/<option weight>
 */
EncapLibMemcached::EncapLibMemcached(const string& configParameters,
                                     const MemManipulateParam& mmparam):manipuParam(mmparam){
    //initial the memcached_st smart pointer
    memcached_st* memc = memcached(configParameters.c_str(),configParameters.size());
    if(!memc)
    {
        cout<<"memcached connected failed!"<<endl;
    }
    shared_ptr<memcached_st> tempMemc(memc,memcached_free);
    smartMemcPointer = tempMemc;
}
/*
 * this constructor using the outside memcached struct consturt the object;
 */
EncapLibMemcached::EncapLibMemcached(shared_ptr<memcached_st> initializedServer,\
                         const MemManipulateParam& mmParam):manipuParam(mmParam)
{
    smartMemcPointer = initializedServer;

}
//this fucntion store the value to the memcached
int EncapLibMemcached::set(const string& key,const string& value){
    return_info = memcached_set(smartMemcPointer.get(),
                                key.c_str(),key.size(),
                                value.c_str(), value.size(),
                                manipuParam.expirationTime,
                                manipuParam.flags);

    const char* return_status;
    return_status =  memcached_strerror(smartMemcPointer.get(),return_info);
    return parse_status(return_status,strlen(return_status));
}
//this function add the nonexist value to the memcache server
int EncapLibMemcached::add(const string& key,const string& value){
    return_info = memcached_add(smartMemcPointer.get(),
                                key.c_str(),key.size(),
                                value.c_str(),value.size(),
                                manipuParam.expirationTime,
                                manipuParam.flags);
    const char* return_status;
    return_status =  memcached_strerror(smartMemcPointer.get(),return_info);
    return parse_status(return_status,strlen(return_status));
}
/*
 * this function replace the value of the given key;
 * if the key is not found ,it will return error string;
 */
int EncapLibMemcached::replace(const string& key,const string& value)
{
    return_info = memcached_replace(smartMemcPointer.get(),
                                    key.c_str(),key.size(),
                                    value.c_str(),value.size(),
                                    manipuParam.expirationTime,
                                    manipuParam.flags);
    const char* return_status;
    return_status =  memcached_strerror(smartMemcPointer.get(),return_info);
    return parse_status(return_status,strlen(return_status));
}
/*
 * this function places a segment of data at the end of piece of the data stored;
 */
int EncapLibMemcached::append(const string& key,const string& value)
{
    return_info = memcached_append(smartMemcPointer.get(),
                                   key.c_str(),key.size(),
                                   value.c_str(),value.size(),
                                   manipuParam.expirationTime,
                                   manipuParam.flags);
    const char* return_status;
    return_status =  memcached_strerror(smartMemcPointer.get(),return_info);
    return parse_status(return_status,strlen(return_status));
}
/*
 * this function places a segment of data before the last piece of the data stored;
 */
int EncapLibMemcached::prepend(const string& key, const string& value)
{
    return_info = memcached_prepend(smartMemcPointer.get(),
                                    key.c_str(),key.size(),
                                    value.c_str(),value.size(),
                                    manipuParam.expirationTime,
                                    manipuParam.flags);
    const char* return_status;
    return_status =  memcached_strerror(smartMemcPointer.get(),return_info);
    return parse_status(return_status,strlen(return_status));
}
/* this function incrememnt the numberic value of the key,if the value is not t
 * numeberic value ,this function will return the error;
 */

int EncapLibMemcached::increment(const string& key,uint32_t offset,uint64_t& result)
{
    return_info = memcached_increment(smartMemcPointer.get(),
                                      key.c_str(), key.size(),
                                      offset, &result);
    const char* return_status;
    return_status =  memcached_strerror(smartMemcPointer.get(),return_info);
    return parse_status(return_status,strlen(return_status));
} 
/*
 * this function is decrement ,similiar as the increment value
 */

int EncapLibMemcached::decrement(const string& key,uint32_t offset, uint64_t& result)
{
    return_info = memcached_decrement(smartMemcPointer.get(),
                                      key.c_str(), key.size(),
                                      offset, &result);
    const char* return_status;
    return_status =  memcached_strerror(smartMemcPointer.get(),return_info);
    return parse_status(return_status,strlen(return_status));
}
/*
 * this function delete the data of the given key
 */
int EncapLibMemcached::delete_data(const string& key)
{
    return_info = memcached_delete(smartMemcPointer.get(),
                                   key.c_str(),key.size(),
                                   manipuParam.expirationTime);
    const char* return_status;
    return_status =  memcached_strerror(smartMemcPointer.get(),return_info);
    return parse_status(return_status,strlen(return_status));
}
/*
 * this function is use the unique flag to keep the data consistent
 */
int EncapLibMemcached::cas(const string& key,const string& value,uint64_t casUnique)
{
    return_info = memcached_cas(smartMemcPointer.get(),
                                key.c_str(), key.size(),
                                value.c_str(), value.size(),
                                manipuParam.expirationTime,
                                manipuParam.flags, casUnique);
    const char* return_status;
    return_status =  memcached_strerror(smartMemcPointer.get(),return_info);
    return parse_status(return_status,strlen(return_status));
}
/*
 * to check whether the key exist or not 
 */
int EncapLibMemcached::key_exist(const string& key)
{
    return_info = memcached_exist(smartMemcPointer.get(),
                                  key.c_str(),
                                  key.size());
    const char* return_status;
    return_status =  memcached_strerror(smartMemcPointer.get(),return_info);
    return parse_status(return_status,strlen(return_status));
}

/* 
 * this function wii set will get the memcahced_st smart pointer 
 */
shared_ptr<memcached_st> EncapLibMemcached::getMemcached()
{
    return smartMemcPointer;
}
/* 
 * this function get the get_configurtaion of the memcached_st 
 *  *
 */
const MemManipulateParam& EncapLibMemcached::get_configurtaion()
{
   return manipuParam; 
} 
/*
 * this function will fecch one result from the server
 */
int EncapLibMemcached:: get(const string& key,
                            shared_ptr<char>& returnResult)
{
    size_t value_length;
    uint32_t flags;
    char *result;
    result = memcached_get(smartMemcPointer.get(),key.c_str(),key.size(),\
           &value_length,&flags,&return_info);
    FreeNotByNew<char> freeChar;
    shared_ptr<char>tmp_shared(result,freeChar);
    returnResult = tmp_shared;

    const char* return_status;
    return_status =  memcached_strerror(smartMemcPointer.get(),return_info);
    return parse_status(return_status,strlen(return_status));
}
/* 
 *call fetch_result method ,you must first call the muti_get method 
 */
int EncapLibMemcached::muti_get(const vector<string>& keys)
{
    const char** tmp_keys = new const char*[keys.size()];
    size_t keysNo = keys.size();
    size_t *eachKeyLength = new size_t[keys.size()];
    vector<string>::const_iterator iter = keys.begin();
    for(size_t i=0;i<keysNo;i++,iter++)
    {
        eachKeyLength[i] = iter->size();
    }
    return_info = memcached_mget(smartMemcPointer.get(),tmp_keys,
                                 eachKeyLength ,keysNo);
    const char* return_status;
    return_status =  memcached_strerror(smartMemcPointer.get(),return_info);
    return parse_status(return_status,strlen(return_status));
}
/* 
 *this function fetch the result of the key sended by the muti_get funcition
 */
int EncapLibMemcached::fetch_result(map<string,shared_ptr<ResultFromMemcache> >& results)
{
    flush_client();
    char  return_key[MEMCACHED_MAX_KEY];
    size_t return_key_length;
    char* return_value;
    size_t return_value_length; 
    uint32_t tmp_flag;
    results.clear();
    ResultFromMemcache* tmp_result;
    while((return_value = memcached_fetch(smartMemcPointer.get(),
                                          return_key,
                                          &return_key_length,
                                          &return_value_length,
                                          &tmp_flag,
                                          &return_info)))
    {
        tmp_result =new ResultFromMemcache(return_key,return_key_length,\
                return_value,return_value_length,tmp_flag);
        results.insert(pair<string,shared_ptr<ResultFromMemcache>>(\
                    string(return_key,return_key_length),shared_ptr<ResultFromMemcache>(tmp_result)));
    }
    const char* return_status;
    return_status =  memcached_strerror(smartMemcPointer.get(),return_info);
    return parse_status(return_status,strlen(return_status));
}
/*
 * add the new memcached server to the memcached_st structor;
 */
int EncapLibMemcached::add_server(const string& host,in_port_t port,NetProtocol protocol)
{
    if(host.empty())
    {
        return MEM_ERROR;
    }
    switch(protocol)
    {
        case TCP:
            return_info = memcached_server_add(smartMemcPointer.get(),\
                    host.c_str(),port);
            break;
        case UDP:
            return_info = memcached_server_add_udp(smartMemcPointer.get(),\
                    host.c_str(),port);
        case UNIX:
            return_info = memcached_server_add_unix_socket(smartMemcPointer.get(),\
                    host.c_str());
        default:
            return_info = memcached_server_add(smartMemcPointer.get(),\
                    host.c_str(),port);
    }

    const char* return_status;
    return_status =  memcached_strerror(smartMemcPointer.get(),return_info);
    return parse_status(return_status,strlen(return_status));
}
void EncapLibMemcached::mem_quit()
{
    memcached_quit(smartMemcPointer.get());
}
int EncapLibMemcached::flush_client()
{
    return_info = memcached_flush_buffers(smartMemcPointer.get());
    const char* return_status;
    return_status = memcached_strerror(smartMemcPointer.get(),return_info);
    return (parse_status(return_status,strlen(return_status)));
}
int EncapLibMemcached::parse_status(const char* status,size_t length)
{
    if(!strncmp(status,"SUCCESS",length))
    {
        return MEM_SUCCESS;
    }
    if(!strncmp(status,"FAILURE",length))
    {
        std::cout<<"FAILURE"<<std::endl;
        return MEM_FAILED;
    }
    std::cout<<"ERROR"<<std::endl;
    return MEM_ERROR;
}

EncapLibMemcached::~EncapLibMemcached()
{
    ;
}
