/*
 * =====================================================================================
 *
 *       Filename:  extractUrl.h
 *
 *    Description:  this file define the interface of the extract algorthims
 *
 *        Version:  1.0
 *        Created:  2014年09月17日 16时07分44秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yu daoming (), ydm2011@163.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef EXTRACT_URL_H
#define EXTRACT_URL_H
#include <string>
#include <map>
#include <list>
#define _DEBUG_
//this is the interface of the 
class ExtractContentInterface{
public:
    enum MatchMode{ONE,ALL};

    //extract the position of tag from the src html file; 
    virtual int  extractTag(const char* src,size_t src_len,
                            const char* tag, size_t tag_len,
                            size_t& match_position)=0;
    //this function extract the pattern tag content that pattern 
    //from the given str;
    virtual void extract(const char* src,
                         size_t src_len,
                         const char* attribute,
                         size_t att_len,
                         std::string& content)=0;
    
    //this function will return  the match strings' position;
    virtual int getMatch(const char* src, size_t src_len,
                         const char* pattern, size_t pat_len,
                         std::list<size_t>& match_position,
                         int mode=ONE)=0;
    //release the data
    virtual ~ExtractContentInterface(){}
    //get the required data;
    virtual int getAttributeOfTag(const char* src,
                                  size_t src_len,
                                  const std::string& attribute_name,
                                  size_t& tag_location)=0;
};

//this use sunday algorithm to match the given pattern
class ExtractBySunday:public ExtractContentInterface{
public:
       
    //this will extract the tag content of the html file;
    int extractTag(const char* src, size_t src_len,
                   const char* tag, size_t tag_len,
                   size_t& match_position);


    //use sunday algorithm  to match the given pattern
    virtual int getMatch(const char* src, size_t src_len, 
                         const char* pattern, size_t pat_len,
                         std::list<size_t>& match_position,
                         int mode=ONE);
    //use sunday algorithm to match the given pattern and extract
    //the given attribute content;
    void extract(const char* src, size_t src_len,
                 const char* attribute,size_t attr_len,
                 std::string& content);

    //get the location of the given attribute of the tag
    int getAttributeOfTag(const char* src, 
                          size_t src_len,
                          const std::string& attribute_name,
                          size_t& tag_location);
private:
    //locate the patten character's loaction in the pattern
    int* preProcessPattern(const char*pattern,size_t pat_len);
    
    //just use the temp store continer
    std::string tag_container;
    int pattern_position[256];
};

//extract url interface
class ExtractUrlInterface{
public:
    struct TagNode{
        std::string tag;
        std::string attribute_name;
        std::string attribute_value;
    };
    //set the parameters to determain which to extract
    virtual void init(const std::string& layer_tags,int layers)=0;
    //get the laocation of the search result container
    virtual int getSearchResultContainer(const char* src,size_t src_len,
                                         size_t& result_loca,
                                         const TagNode& node)=0;
    //get all the urls of the search result;
    virtual int getUrls(const char* src,
                size_t src_len,
                const std::string& url_tag,
                std::list<std::string>& urls)=0;
    virtual bool parseLayerTag(const std::string& lay_tags,
                               int total_layer,
                               std::list<TagNode>&tag_key_value);
    virtual ~ExtractUrlInterface(){}
};

//extract the url from the so.com
class ExtractUrlFromSo:public ExtractUrlInterface{
public:
    ExtractUrlFromSo(ExtractContentInterface* extract_algorithm);
    //set the parameters to determain which to extract
    void init(const std::string& layer_tags,int layers);
    //get the laocation of the search result container
    int getSearchResultContainer(const char* src,
                                 size_t src_len,
                                 size_t& result_loca,
                                 const TagNode& node);
    //get all the urls of the search result;
    int getUrls(const char* src,
                size_t src_len,
                const std::string& url_tag,
                std::list<std::string>& urls);
protected:
    //extract the url from the given tag
    int extractUrl(const char* src,
                   size_t src_len,
                   const TagNode& node,
                   bool& is_last,
                   std::string& url,
                   size_t& curr_iter);
private:
    ExtractContentInterface* extract_obj;
    std::list<TagNode> layer_nodes;
};

//extract the url from the baidu.com
class ExtractUrlFromBaidu:public ExtractUrlInterface{
public:
    //set the match algorithm
    ExtractUrlFromBaidu(ExtractContentInterface* extractAlgorithm);

    //set the location of the search result container
    void init(const std::string& layers_tag,int layers);

    //get the laocation of the search result container
    int getSearchResultContainer(const char* src,
                                 size_t src_len,
                                 size_t& result_loca,
                                 const TagNode& node);
    //get all the urls of the search result;
    int getUrls(const char* src,
                size_t src_len,
                const std::string& url_tag,
                std::list<std::string>& urls);
protected:
    //this is  the real funtional method of get urls method
    int extractUrl(const char* src,
                   size_t src_len,
                   const TagNode& node,
                   std::string& url,
                   size_t& curr_iter);
private:
    ExtractContentInterface* extract_obj;//the match algorithm
    std::list<TagNode>  layer_nodes;//parse the html tag layer's node
    std::list<std::string> list_id_baidu;//baidu result "id"
};


#endif


