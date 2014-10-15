/*
 * Copyright 2014 <copyright holder> <email>
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */

#ifndef GETWEB_H
#define GETWEB_H

#include <sys/socket.h>  
#include <sys/epoll.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <netdb.h>
#include <fcntl.h>  
#include <unistd.h>  
#include <stdio.h>  
#include <errno.h> 
#include <stdlib.h> 
#include <iostream>  
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>

#include <string>
#include <list>
#include <map>
#include <fstream>

#include "memcached/encaplibmemcache.h"
#include "extractUrl/extractUrl.h"
#include "my_atomic.h"
//#include "threadpool.h"
//namespace GETWEBNAMESPACE
//{  
using namespace std;
#define BUFSIZE 256
//#define RECVSIZE 800*1024
/*
 *	include key url webinfo
 *	callback
 */
typedef struct hostinf
{
	hostinf()
	:fd(),url("")
	{}
	int fd;
	string url;
	struct sockaddr_in caadd;
	char GET[BUFSIZE];
	char host[BUFSIZE];
	string com;
	string param;
}HostInfo;



typedef struct 
{
	int fd;
	char GET[BUFSIZE];
	char host[BUFSIZE];
}ConnectInfo;



typedef struct
{
	string key;
}InputDate;

typedef struct urlinfo
{
	urlinfo():port(80){};
	string url;
	int port ;
	string com;
	string param;
}UrlInfo;

class WebInfo
{
public:
	
	WebInfo()
	:fd(0),key(""),url("")
	,pwebinfo(NULL),buf(NULL)
	,webinfo_len(0)/*,call_back(NULL)*/
	{}
	~WebInfo()
	{
		if(pwebinfo)
		{
			delete[] pwebinfo;
		}
	}
	int fd;
	string key;
	string url;
	char* pwebinfo;
	char* buf;
	int webinfo_len;
	HostInfo hostinfo;
	//void (*call_back)(int fd, int events, void *arg); 
};

class OutputDate
{
public:
	OutputDate():pwebinfo(NULL),webinfo_len(0){}
	~OutputDate()
	{}
	string key;
	string url;
	char* pwebinfo;
	int webinfo_len;
	inline void init(WebInfo* webinfo)
	{
		key = webinfo->key;
		url = webinfo->url;
		webinfo_len = webinfo->webinfo_len;
		if(webinfo->pwebinfo)
		{
			pwebinfo = new char[webinfo_len];
			if(pwebinfo)
			{
				memcpy(pwebinfo,webinfo->pwebinfo,webinfo_len);
			}
			else
			{
				//Application memory failure
				exit(-9);
			}
		}
		if(webinfo->buf)
		{
			pwebinfo = new char[webinfo_len];
			if(pwebinfo)
			{
				memcpy(pwebinfo,webinfo->buf,webinfo_len);
			}
			else
			{
				//Application memory failure
				exit(-9);
			}
		}
	}
	
	void relase()
	{
		if(pwebinfo)
		{
			delete[] pwebinfo;
			pwebinfo = NULL;
		}
	}
};
typedef list<OutputDate> OutputdateList;

class MyLocalLock
{
private:
	pthread_mutex_t* m_r;
public:
	MyLocalLock(pthread_mutex_t* r)
		:m_r(r)
	{
		pthread_mutex_lock(m_r);//获取读取锁;
	}
	~MyLocalLock()
	{
		pthread_mutex_unlock(m_r); //释放锁;
	}
private:
	MyLocalLock(const MyLocalLock& ml);
	MyLocalLock& operator= (const MyLocalLock& ml);
};

class MyLocalRLock
{
private:
	pthread_rwlock_t* m_r;
public:
	MyLocalRLock(pthread_rwlock_t* r)
		:m_r(r)
	{
		pthread_rwlock_rdlock(m_r);//获取读取锁;
	}
	~MyLocalRLock()
	{
		pthread_rwlock_unlock(m_r); //释放锁;
	}
private:
	MyLocalRLock(const MyLocalRLock& ml);
	MyLocalRLock& operator= (const MyLocalRLock& ml);
};

class MyLocalWLock
{
private:
	pthread_rwlock_t* m_w;
public:
	MyLocalWLock(pthread_rwlock_t* w)
		:m_w(w)
	{
		pthread_rwlock_wrlock(m_w);//获取写锁;
	}
	~MyLocalWLock()
	{
		pthread_rwlock_unlock(m_w); //释放锁;
	}
private:
	MyLocalWLock(const MyLocalWLock& ml);
	MyLocalWLock& operator=(const MyLocalWLock& ml);
};

class OutDateManager
{
	typedef map<string,OutputdateList >OutDateMap;
public:
	OutDateManager();
	~OutDateManager();
	OutputdateList GetWebinfoBykey(string key);
	bool AddOutdate(WebInfo* webinfo);
	bool DelOutdate(string key);
	inline OutDateMap::size_type size()
	{return m_outdates.size();}
private:
	OutDateManager(const OutDateManager& m);
	OutDateManager& operator=(const OutDateManager& m);
private:
	pthread_rwlock_t m_rwlock;
	OutDateMap m_outdates;
	
};

typedef struct engineparam
{
	engineparam()
	:keyfilepath(""),urlfilepath(""),urlparam("")
	,memcachedhostaddr(""),keynum(2000)
	,HZ(10),sendtomemcached(false),savewebinfo(false)
	,port(80)
	{}
	string keyfilepath;
	string urlfilepath;
	int port;
	string urlparam;
	string memcachedhostaddr;
	int keynum;
	int  HZ;
	bool sendtomemcached;
	bool savewebinfo;
}Engineparam;

typedef list<string> UrlList;
class GetWeb
{
	typedef list<WebInfo> WebInfoList;
	typedef list<HostInfo> HostInfoList;
	typedef list<InputDate> InputDateList;
	typedef list<char*> CharPointList;
	//typedef list<OutputDate> OutputdateList;
	typedef map<string,OutputdateList >OutDateMap;
	typedef list<UrlInfo> UrlInfoList;
	typedef struct
	{
		GetWeb* pgetweb;
		int num;
	}MasterInfo;
public:
	GetWeb();
	GetWeb(const GetWeb& other);
	~GetWeb();
	void init(Engineparam);
	void run(int num);
	void* PthreadFun(void *);
	void master(int num);
	
	void ctrl_run(int num = -9);
	void ctrl_master(int HZ );
	void ctrl_work();
	void sigalrm_handler(int sig);
	inline bool finish() {return m_finished;}
	inline OutDateMap& GetOutDate()
	{return m_outdates;}
	inline OutDateManager& GetOutDateManager()
	{return m_outdatemanager;}
	inline OutDateManager* GetOutDateManagerPoint()
	{return &m_outdatemanager;}
	
private:
	GetWeb& operator=(const GetWeb& other);
	bool operator==(const GetWeb& other);
	void AddEpoll(int epollfd,int fd,int events,WebInfo* arg);
	void ModEpoll(int epollfd,int fd,int events,WebInfo* arg);
	void DelEpoll(int epollfd,int fd);
	int WaitEpoll(int epollfd,epoll_event* es,int size,int para);
	void first(int num);
	void Connect(WebInfo* webinfo);
	void setnotblock(int fd);
	void SendDate(void* arg);
	void RecvData(void *arg);
	ssize_t socket_send(int fd, const char* buffer, size_t buflen,int param);
	void CreatSocket(WebInfo* webinfo);
	void Gethost(UrlInfo ,HostInfo& hostinfo);
	WebInfo* GetWebinfo();
	void Delwebinfo(WebInfo* webinfo);
	void AddPutDate(WebInfo* webinfo);
	bool AddWebinfoToMemcached(WebInfo* webinfo);
	void ExtractWebinfo(WebInfo* webinfo,UrlList& urls);
	//void ctrl_RecvDate(void* arg);
	void ctrl_first();
	
	void Settimer(int HZ);
	
private:
	void readkeyfile();
	void readurlfile();
	void inithostinfo();
private:
	int m_epollfd; //epollheadle
	CharPointList m_charpoints;
	InputDateList m_inputdates;
	UrlInfoList m_urls;
	WebInfoList m_webinfos;
	HostInfoList m_hosts;
	HostInfoList::iterator m_hostsitercur;
	HostInfoList::iterator m_hostbeg;
	HostInfoList::iterator m_hostsend;
	OutDateMap m_outdates;
	
	OutDateManager m_outdatemanager;
	EncapLibMemcached* m_pmemcached;
	//ExtractBySunday* m_pextract;
	//ExtractUrlFromSo* m_pextract_so; 
	
	pthread_mutex_t m_mutex_getwebinfo; 
	pthread_rwlock_t m_out_rwlock;
	pthread_rwlock_t m_webinfo_rwlock;
	sem_t m_sem;
	sem_t m_sem_recv;
	sem_t m_sem_send;
	
	string m_keyfilepath;
	string m_urlfilepath;
	string m_urlparam;
	int m_port;
	int m_HZ;
	int m_num; //key worlds num;
	my_atmic m_webinfonum;
	
	bool m_sendtomem;
	bool m_savewebinfo;
	bool m_finished;
	//WebInfo* pwebinfo;
	enum 
	{
		RECVSIZE = 800*1024,
		MAX_EPOLL = 1024
	};
};

extern GetWeb gb;
//}
#endif // GETWEB_H
