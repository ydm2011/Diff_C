#include "extractUrl/extractUrl.h"
#include "memcached/encaplibmemcache.h"
#include "util/util.h"
#include "getweb/getweb.h"
#include <iostream>
#include <fstream>
#include <ctime>
using namespace std;
GetWeb gb;
int upLoadMemcache(ExtractUrlInterface* extract,
                   const string& query_path,
                   const string& result_path,
                   const string& source_flag,
                   EncapLibMemcached& mem)
{
    //test so.com
    ifstream in;
    ifstream in_result;

    in.open(query_path.c_str(),ios::in);
    if(!in)
    {
        std::cout<<query_path<<" Not Found!"<<endl;
        return -1;
    }


    string contents;
    string complete_path;

    string tmp_key;
    string store_key;
    string format_urls;
    list<string> urls;
    while(!in.eof())
    {

        contents.clear();
        urls.clear();
        format_urls.clear();

        in>>tmp_key;
        //querys.push_back(string(buffer,256));
        cout<<tmp_key<<endl;
        complete_path = result_path + tmp_key;
        //complete_path.pop_back();
        cout<<complete_path<<endl;
        in_result.open(complete_path.c_str(),ios::in|ios::binary);
        if(!in_result)
        {
            std::cout<<complete_path.c_str()<<endl;
            return -1;
        }
        in_result.seekg(0,ios::end);
        cout<<in_result.tellg()<<endl;
        contents.resize(in_result.tellg());
        in_result.seekg(0,ios::beg);
        in_result.read(&contents[0],contents.size());
        in_result.close();

        extract->getUrls(contents.c_str(),contents.size(),"<h3",urls);
        format_urls = tmp_key;
        format_urls += ":";
        while(!urls.empty())
        {
            format_urls += urls.front()+",";
            urls.pop_front();
        }
        format_urls.pop_back();
        store_key = tmp_key + source_flag;

        if(mem.set(store_key,format_urls)!=MEM_SUCCESS)
        {
            cout<<"memcached failed!"<<endl;
            return -1;
        }
        shared_ptr<char> result;
        mem.get(store_key,result);
        cout<<result<<endl;
    }
    in.close();
    return 0;
}

int main()
{
    string query_path = "/home/daoming/Desktop/200keys/200keys.txt";
    string result_path = "/home/daoming/Desktop/www.so.com.new/";
    string source_flag = "_360";

    Engineparam engineparam;
    engineparam.keyfilepath = "key.txt";
    engineparam.urlfilepath = "url.txt";
    engineparam.keynum = 200;
    engineparam.memcachedhostaddr = "--SERVER=test2.se.gzst.qihoo.net:11211";
    engineparam.sendtomemcached = false;
    engineparam.HZ = 10;
    engineparam.urlparam = "";
    engineparam.port = 80;

    gb.init(engineparam);
    //gb.ctrl_run(10);
    gb.run(1);
    int i = 0;

    while(!gb.finish())
    {

        sleep(10);

    }

    ExtractContentInterface * algorithm = new ExtractBySunday;
    EncapLibMemcached mem;
    ExtractUrlInterface *extract = new ExtractUrlFromSo(algorithm);

    upLoadMemcache(extract,query_path,result_path,source_flag,mem);

    return 0;
}


/*
int main()
{
    ExtractBySunday *extract = new ExtractBySunday;
    ExtractUrlFromSo extract_so(extract);

    //test so.com
    ifstream in;


    in.open("/home/daoming/Desktop/index.html",ios::in|ios::binary);
    if(!in)
    {
        std::cout<<"index.html"<<" Not Found!"<<endl;
        return -1;
    }
    string contents;
    in.seekg(0,ios::end);
    contents.resize(in.tellg());
    in.seekg(0,ios::beg);
    in.read(&contents[0],contents.size());
    in.close();

    list<string> urls;
    extract_so.getUrls(contents.c_str(),contents.size(),"<h3",urls);



    //test extract url from baidu.com

    contents.clear();
    urls.clear();
    in.open("/home/daoming/Desktop/baidu_index.html",ios::in|ios::binary);
    if(!in)
    {
        std::cout<<"index.html"<<" Not Found!"<<endl;
        return -1;
    }
    in.seekg(0,ios::end);
    contents.resize(in.tellg());
    in.seekg(0,ios::beg);
    in.read(&contents[0],contents.size());
    in.close();

    ExtractUrlFromBaidu extract_baidu(extract);
   // extract_baidu.init("<ul: id=1;",1);
    extract_baidu.getUrls(contents.c_str(),contents.size(),"<h3",urls);

    return 0;
}
*/
