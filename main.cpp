#include "extractUrl/extractUrl.h"
#include "memcached/encaplibmemcache.h"
#include "util/util.h"
#include "getweb/getweb.h"
#include "diff/diff.h"
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



int main(int argc, char** argv)
{

    string query_path = "/home/daoming/QtProject/Diff_C/getweb/queries";
    string url_path = "/home/daoming/QtProject/Diff_C/getweb/url.txt";//set by user
    string source_flag1 = "_360";
    string source_flag2 = "_baidu";//set by user

    // getweb configure
    Engineparam engineparam;
    engineparam.keyfilepath = query_path;
    engineparam.urlfilepath = url_path;
    engineparam.keynum = 2000;//set by user
    engineparam.memcachedhostaddr = "--SERVER=test2.se.gzst.qihoo.net:11211";
    engineparam.sendtomemcached = false;
    engineparam.savewebinfo = true;
    engineparam.HZ = 10;
    engineparam.urlparam = "";

    engineparam.block = false;
    engineparam.readurlfile = true;
    engineparam.testurlport = "www.so.com:80";
    engineparam.testcom = "q=";
    engineparam.onlineurlport = "www.so.com:80";
    engineparam.onlinecom = "q=";
    engineparam.readkeyfile = true;

    gb.init(engineparam);
    gb.ctrl_run(60);
    //gb.run(1);
    while(!gb.finish())
    {
        ;//sleep(1);
    }

    //map<string,int> host_names;
    //ifstream url_file;
    //url_file.open(url_path.c_str());
    //string temp_engine;
    //while(!url_file.eof())
    //{
     //   url_file>>temp_engine;
      //  host_names[temp_engine]=1;
    //}
    //url_file.close();
    //extract the urls init
    ExtractContentInterface * algorithm = new ExtractBySunday;
    EncapLibMemcached mem(engineparam.memcachedhostaddr);
    ExtractUrlInterface *extract = new ExtractUrlFromSo(algorithm);

    //work
    ifstream query_file;
    string tmp_key;
    query_file.open(query_path,ios::in);

    //store the extract urls
    map<string,list<string> > key_urls1;
    map<string,list<string> > key_urls2;

    //get the html contents init
    OutputdateList datas;
    for(int i=0;i<engineparam.keynum; ++i)
    {
        query_file>>tmp_key;
        //key_urls1
        datas = gb.GetOutDateManager().GetWebinfoBykey(tmp_key);
        for(OutputdateList::iterator beg = datas.begin();
            beg != datas.end(); ++beg)
        {
            //if()

            extract->getUrls(beg->pwebinfo,beg->webinfo_len,"<h3 ",key_urls1[tmp_key]);
            beg++;
            extract->getUrls(beg->pwebinfo,beg->webinfo_len,"<h3 ",key_urls2[tmp_key]);
            //upLoadMemcache(tmp_key,urls,source_flag,mem);
            //key_urls1[tmp_key+source_flag1] = urls1;
            //key_urls2[tmp_key+source_flag2] = urls2;
        }
    }
    list<string>test;

    //get the diff
    std::list<DiffCorresResult> diff_result;
    diffSearch(key_urls1,key_urls2,diff_result);
    //write json files;
    ofstream out;
    std::list<DiffCorresResult>:: iterator iter_diff = diff_result.begin();

    map<string,list<string> > diff_urls1;
    map<string,list<string> > diff_urls2;
    for( ;iter_diff!=diff_result.end();++iter_diff)
    {
        diff_urls1[iter_diff->key] = key_urls1[iter_diff->key];
        diff_urls2[iter_diff->key] = key_urls2[iter_diff->key];
    }
    //all diff result
    out.open("current_360",ios::out|ios::binary);
    mapToJson(diff_urls1,out);
    out.close();

    out.open("previous_360",ios::out|ios::binary);
    mapToJson(diff_urls2,out);
    out.close();
    //all diff correspond
    out.open("correspond",ios::out|ios::binary);
    corrToJson(diff_result,out);
    out.close();


    //all diff TopN
    //get all the topN urls
    map<int,list<Top> > topN1;
    map<int,list<Top> > topN2;

    //diffTop(key_urls1,key_urls2,diff_result,topN1,topN2);
    //get engine to json;

    string json;
    list<string> engine;
    list<string> engine_tag;
    engine.push_back("http://www.so.com/s?src=srp&fr=360sou_home&cache=255&_api=srv2.safe.zzbc.qihoo.net:9501&q=");
    engine.push_back("http://www.so.com/s?src=srp&fr=360sou_home&cache=255&_api=srv3.safe.zzbc.qihoo.net:9501&q=");

    engine_tag.push_back("testUrl");
    engine_tag.push_back("defaultUrl");
    json +="[";
    keyValueJson(engine_tag,engine,json);
    json += ",";
    //get rate to json
    vector<double> top_n_rate;
    getChangeRate(diff_result,engineparam.keynum,top_n_rate);

    topRateToJson(top_n_rate,json);
    json += "]";
    out.open("para",ios::out|ios::binary);
    out<<json;
    out.close();
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
