#include "extractUrl/extractUrl.h"
#include "memcached/encaplibmemcache.h"
#include "util/util.h"
#include "getweb/getweb.h"
#include <iostream>
#include <fstream>
#include <ctime>
using namespace std;
GetWeb gb;
int upLoadMemcache(const string& query_key,
                   const list<string>& urls_list,
                   const string& store_flag,
                   EncapLibMemcached& mem)
{
    string format_urls = query_key;
    format_urls += ":";
    list<string>::const_iterator iter = urls_list.begin();
    while(iter!=urls_list.end())
    {
        format_urls += urls_list.front()+",";
        iter++;
    }
    format_urls.pop_back();
    string store_key = query_key + store_flag;
    if(mem.set(store_key,format_urls)!=MEM_SUCCESS)
    {
        cout<<"memcached failed!"<<endl;
        return -1;
    }
    return 0;
}

int main()
{
    string query_path = "/home/daoming/Qtproject/Diff_C/getweb/key.txt";
    string url_path = "/home/daoming/Qtproject/Diff_C/getweb/url.txt";
    string source_flag = "_360";

    // getweb configure
    Engineparam engineparam;
    engineparam.keyfilepath = query_path;
    engineparam.urlfilepath = url_path;
    engineparam.keynum = 20;
    engineparam.memcachedhostaddr = "--SERVER=localhost:11211";
    engineparam.sendtomemcached = true;
    engineparam.savewebinfo = true;
    engineparam.HZ = 10;
    engineparam.urlparam = "";
    engineparam.port = 80;

    gb.init(engineparam);
    //gb.ctrl_run(10);
    gb.run(1);

    while(!gb.finish())
    {
        sleep(10);
    }
    /*
    OutputdateList dates = gb.GetOutDateManager().GetWebinfoBykey("美女qq号码");
        for(OutputdateList::iterator beg = dates.begin();
            beg != dates.end(); ++beg)
        {
            cout<<beg->pwebinfo<<endl;
        }
   */
    //extract the urls
    ExtractContentInterface * algorithm = new ExtractBySunday;
    EncapLibMemcached mem;
    ExtractUrlInterface *extract = new ExtractUrlFromSo(algorithm);
    //string contents;

    ifstream query_file;
    string tmp_key;
    query_file.open(query_path,ios::in|ios::binary);
    shared_ptr<char> contents;
    list<string> urls;
    for(int i=0;i<engineparam.keynum; ++i)
    {
        query_file>>tmp_key;
        mem.get(tmp_key,contents);
        extract->getUrls(contents.get(),strlen(contents.get()),"<h3 ",urls);
        upLoadMemcache(tmp_key,urls,source_flag,mem);
    }

    //diff


    //judge



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
