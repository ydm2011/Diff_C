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
#include "getweb.h"
//namespace GETWEBNAMESPACE
//{

int cunn = 0;
template <typename TYPE, void (TYPE::*RecvData)(void*) > 
void* _call_thread(void* arg)
{
	TYPE* temp = &gb;
	temp->RecvData(arg);
	return NULL;
}

template <typename TYPE, void* (TYPE::*PthreadFun)(void*) > 
void* _call_thread1(void* arg)
{
	TYPE* temp = &gb;
	temp->PthreadFun(arg);
	return NULL;
}

template <typename TYPE,typename TYPE2 ,void (TYPE2::*master)(int) >
void* _threadmaster(void* arg)
{
	TYPE* temp = (TYPE*)arg;
	temp->pgetweb->master(temp->num);
	return NULL;
}

template <typename TYPE,typename TYPE2 ,void (TYPE2::*ctrl_master)(int) >
void* _threadctrlmaster(void* arg)
{
	TYPE* temp = (TYPE*)arg;
	temp->pgetweb->ctrl_master(temp->num);
	return NULL;
}

template <typename TYPE, void (TYPE::*ctrl_work)() > 
void* _threadEpollWait(void* arg)
{
	TYPE* temp = (TYPE*)arg;
	temp->ctrl_work();
	return NULL;
}

template <typename TYPE, void (TYPE::*sigalrm_handler)(int) > 
void _callsigal(int arg)
{
	TYPE* temp = (TYPE*)&gb;
	temp->sigalrm_handler(arg);
	return ;
}

GetWeb::GetWeb()
	:m_outdatemanager()
	,m_keyfilepath(""),m_urlfilepath("")
	,m_webinfonum(),m_finished(false),m_savewebinfo(false)
{
	//m_num = 4000;
	//init();
 
}

GetWeb::GetWeb(const GetWeb& other)
{

}

GetWeb::~GetWeb()
{
	if(!m_pmemcached)
	{
		delete m_pmemcached;
		m_pmemcached = NULL;
	}
// 	if(!m_pextract_so)
// 	{
// 		delete m_pextract_so;
// 		m_pextract_so = NULL;
// 	}
// 	if(!m_pextract)
// 	{
// 		delete m_pextract;
// 		m_pextract = NULL;
// 	}
// 	
	
}

GetWeb& GetWeb::operator=(const GetWeb& other)
{

}

bool GetWeb::operator==(const GetWeb& other)
{

}


void GetWeb::run(int num)
{
	
	pthread_t a_thread;
	pthread_attr_t thread_attr;
	int res = pthread_attr_init(&thread_attr);
	if(0 != res)
	{
		cout<<"attribute create failed\n";
	}
	res = pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED);
	if(0 != res)
	{
		cout<<"setting detached attribute failed\n";
		exit(-1);
	}
	MasterInfo masterinfo ={this,num};
	res = pthread_create(&a_thread,&thread_attr,_threadmaster<MasterInfo,GetWeb,&GetWeb::master>,&masterinfo);
	//res = pthread_create(&a_thread,NULL,_threadmaster<MasterInfo,GetWeb,&GetWeb::master>,&masterinfo);
	if(0 != res)
	{
		cout<<"create thread error\n";
	}
	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGALRM);
	pthread_sigmask(SIG_SETMASK,&sigset,NULL);
	(void)pthread_attr_destroy(&thread_attr);
	//while(true)_threadmaster
	//{}
}

void GetWeb::master(int num)
{
	
	typedef list<pthread_t*> ThreadList;
	ThreadList threads;
		
	for(int i = 0; i < num; ++i)
	{
		pthread_t* pth = new pthread_t();
		pthread_create(pth,NULL,_call_thread1<GetWeb,&GetWeb::PthreadFun>,this);
		threads.push_back(pth);
	}
	ThreadList::iterator beg = threads.begin();
	ThreadList::iterator end = threads.end();
	for(;beg != end; ++beg)
	{
		pthread_join(**beg,NULL);
	}
	
}

void GetWeb::init(Engineparam engparam )
{
	m_epollfd = epoll_create(MAX_EPOLL);
	m_num = engparam.keynum;
	m_keyfilepath = engparam.keyfilepath;
	m_urlfilepath = engparam.urlfilepath;
	m_urlparam = engparam.urlparam;
	m_HZ = engparam.HZ;
	m_sendtomem = engparam.sendtomemcached;
	m_port = engparam.port;
	m_savewebinfo = engparam.savewebinfo;
	
	readkeyfile();
	readurlfile();
	inithostinfo();
	
	m_webinfonum = m_urls.size() * m_inputdates.size();
	
	sem_init(&m_sem,0,1);
	sem_init(&m_sem_recv,0,0);
	sem_init(&m_sem_send,0,m_HZ);
	
	int res=pthread_rwlock_init(&m_out_rwlock,NULL);//初始化读写锁
	if (res != 0)
	{
		perror("rwlock initialization failed");
	}
	
	res=pthread_rwlock_init(&m_webinfo_rwlock,NULL);//初始化读写锁
	if (res != 0)
	{
		perror("rwlock initialization failed");
	}
	m_hostsitercur = m_hosts.begin();
	m_hostbeg = m_hosts.begin();
	m_hostsend = m_hosts.end();
	
	if(m_sendtomem)
	{
		string hostAddress = engparam.memcachedhostaddr;//"--SERVER=test2.se.gzst.qihoo.net:11211";
		MemManipulateParam testPara;
		m_pmemcached = new EncapLibMemcached(hostAddress,testPara);
		
		//m_pextract = new ExtractBySunday;
		//m_pextract_so = new ExtractUrlFromSo(m_pextract);
	}
	
}

void GetWeb::AddEpoll(int epollfd,int fd,int events,WebInfo* arg)
{
	if(fd < 3)
	{return;}
	//MyLocalLock mylock(&m_mutex_epoll);
	struct epoll_event epv = {0, {0}};
	epv.data.ptr = arg; 
	arg->fd = arg->hostinfo.fd = fd;
	epv.events = events; 
	if(epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &epv) < 0)  
			printf("Event Add failed[fd=%d], evnets[%d]\n",  fd, events);  
	else  
	{
		//std::cout<<"add fd"<<fd<<"\n";
	}
}

void GetWeb::ModEpoll(int epollfd, int fd, int events, WebInfo* arg)
{
	if(fd < 3)
	{return;}
	//MyLocalLock mylock(&m_mutex_epoll);
	struct epoll_event epv = {0, {0}};
	epv.data.ptr = arg; 
	arg->fd = arg->hostinfo.fd = fd;
	epv.events = events; 
	if(epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &epv) < 0)  
			printf("Event MOD failed[fd=%d], evnets[%d]\n",  fd, events);  
	else  
	{
		//std::cout<<"MOD fd"<<fd<<"\n";
	}
}

void GetWeb::DelEpoll(int epollfd,int fd)
{
	//MyLocalLock mylock(&m_mutex_epoll);
	struct epoll_event epv = {0, {0}};  
	epv.data.ptr = NULL;  
	if(epoll_ctl(epollfd, EPOLL_CTL_DEL,fd, &epv) <0 )
	{
		cout<<"delepoll error\n";
	}
	
}

int GetWeb::WaitEpoll(int epollfd, epoll_event* es, int size, int para)
{
	//sem_wait(&m_sem);
	return epoll_wait(epollfd,es,size,para);
	//sem_post(&m_sem);
}

void GetWeb::first(int num)
{
	while(num)
	{
		WebInfo *pwebinfo = NULL;
		pwebinfo = GetWebinfo();
		if(!pwebinfo)
		{
			return;
			//break;
		}
		pwebinfo->buf = NULL;
		CreatSocket(pwebinfo);
		Connect(pwebinfo);
		--num;
	}
	
}


void* GetWeb::PthreadFun(void*)
{
	struct epoll_event events[20];
	first(60);
	int fds = 0;
	char* buf = new char[RECVSIZE];
	WebInfo* webinfo = NULL;
	while(true&&m_webinfonum.m_coun > 0)
	{
        //first(fds);true
		fds = 0;
		while(!fds)
		{
			fds = WaitEpoll(m_epollfd,events,sizeof(events)/sizeof(events[0]),100);
			//fds = epoll_wait(m_epollfd,events,20,100);
		}
		for(int i = 0; i < fds; i++)
		{
			webinfo = static_cast<WebInfo*>(events[i].data.ptr);
			if(events[i].events&EPOLLIN)
			{
				webinfo->buf = buf;
				RecvData(webinfo);
				//AddPutDate(webinfo);
				if(m_savewebinfo)
				{
					m_outdatemanager.AddOutdate(webinfo);
				}
				if(m_sendtomem)
				{
					AddWebinfoToMemcached(webinfo);
				}
				first(1);
			 //std::cout<<pwebinfo->pwebinfo<<std::endl;
			}
			
			if(events[i].events&EPOLLOUT)
			{
				SendDate(events[i].data.ptr);
				ModEpoll(m_epollfd,webinfo->hostinfo.fd,EPOLLIN,(WebInfo*)events[i].data.ptr);
			}
		}
		
	}
	delete[] buf;
	m_finished = true;
	return NULL;
}

void GetWeb::Gethost(string url, HostInfo& hostinfo)
{
	int cfd = -1;
	struct sockaddr_in& cadd = hostinfo.caadd;
	struct hostent *pURL = NULL;
    //int BUFSIZE = (int)malloc(2048);
    char myurl[BUFSIZE];
    char *pHost = 0;
    //char *pGET = 0;
    char host[BUFSIZE],GET[BUFSIZE];
    char request[BUFSIZE];
    static char text[BUFSIZE];
    int i,j;

    //分离主机中的主机地址和相对路径
    memset(myurl,'\0',BUFSIZE);
    memset(host,'\0',BUFSIZE);
    strcpy(myurl,url.c_str());
    for(pHost = myurl;*pHost != '/' && *pHost != '\0';++pHost);

    //获取相对路径保存到GET中
    memset(GET,0,BUFSIZE);
    if((int)(pHost-myurl) == strlen(myurl))
    {
         strcpy(GET,"/");//即url中没有给出相对路径，需要自己手动的在url尾部加上/
    }
    else
    {
         strcpy(GET,pHost);//地址段pHost到strlen(myurl)保存的是相对路径
    }

    //将主机信息保存到host中
    //此处将它置零，即它所指向的内容里面已经分离出了相对路径，剩下的为host信息(从myurl到pHost地址段存放的是HOST)
    *pHost = '\0';
     strcpy(host,myurl);
    pURL = gethostbyname(host);//将上面获得的主机信息通过域名解析函数获得域>名信息
    //设置IP地址结构
    bzero(&cadd,sizeof(struct sockaddr_in));
    cadd.sin_family = AF_INET;
    cadd.sin_addr.s_addr = *((unsigned long*)pURL->h_addr_list[0]);
    cadd.sin_port = htons(m_port);
	strcpy(hostinfo.GET,GET);
	strcpy(hostinfo.host,host);
	
}

WebInfo* GetWeb::GetWebinfo()
{
	MyLocalWLock mywlock(&m_webinfo_rwlock);
	if(0 == m_inputdates.size())
	{
		return NULL;
	}
	else
	{
		WebInfo webinfo;
		if(m_hostsend == m_hostsitercur)
		{
			m_hostsitercur = m_hostbeg;
		}
		webinfo.hostinfo = *m_hostsitercur;
		webinfo.key = m_inputdates.front().key;
		webinfo.url = m_hostsitercur->url;
		m_webinfos.push_back(webinfo);
		m_inputdates.pop_front();
		++ m_hostsitercur;
		++cunn;
		return &m_webinfos.back();
	}
}

void GetWeb::Delwebinfo(WebInfo* webinfo)
{
	MyLocalWLock mywlock(&m_webinfo_rwlock);
	WebInfoList::iterator beg = m_webinfos.begin();
	WebInfoList::iterator end = m_webinfos.end();
	while(beg != end)
	{
		if(&(*beg) == webinfo)
		{
			m_webinfos.erase(beg);
			break;
		}
		++beg;
	}
}

void GetWeb::AddPutDate(WebInfo* webinfo)
{
	OutputDate temp;
	temp.init(webinfo);
	MyLocalWLock mywlock(&m_out_rwlock);
	OutputdateList outlist = m_outdates[webinfo->key];
	outlist.push_back(temp);
}

bool GetWeb::AddWebinfoToMemcached(WebInfo* webinfo)
{
	if(!webinfo)
	{
		return false;
	}
	int res = 0;
	if(webinfo->buf)
	{
		string strweb(webinfo->buf,webinfo->webinfo_len);
		res = m_pmemcached->set(webinfo->key,strweb);
	}
	else if(webinfo->pwebinfo)
	{
		string strweb(webinfo->pwebinfo,webinfo->webinfo_len);
		res = m_pmemcached->set(webinfo->key,strweb);
	}
	else
	{
		return false;
	}
	if(MEM_SUCCESS == res)
	{
		return true; 
	}
	else
	{
		cout<<"addmemcached error!"<<endl;
		return false;
	}
}

void GetWeb::ExtractWebinfo(WebInfo* webinfo, UrlList& urls)
{
	if(!webinfo)
	{
		return;
	}
	char* pwebinfo = NULL;
	if(webinfo->buf)
	{
		pwebinfo = webinfo->buf;
	}
	if(webinfo->pwebinfo)
	{
		pwebinfo = webinfo->pwebinfo;
	}
	//m_pextract_so->getUrls(pwebinfo,webinfo->webinfo_len,"<h3",urls);
}


void GetWeb::ctrl_first()
{
	WebInfo *pwebinfo = NULL;
	pwebinfo = GetWebinfo();
	if(!pwebinfo)
	{
		return;
		//break;
	}
	pwebinfo->buf = NULL;
	CreatSocket(pwebinfo);
	Connect(pwebinfo);
}

void GetWeb::ctrl_run(int num)
{
	if(-9 == num && m_HZ >1)
	{
		sem_init(&m_sem_recv,0,0);
		sem_init(&m_sem_send,0,m_HZ);
	}
	else if(num > 1)
	{
		sem_init(&m_sem_recv,0,0);
		sem_init(&m_sem_send,0,num);
	}
	else
	{
		cout<<"param error\n";
	}
	pthread_t a_thread;
	pthread_attr_t thread_attr;
	int res = pthread_attr_init(&thread_attr);
	if(0 != res)
	{
		cout<<"attribute create failed\n";
	}
	res = pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED);
	if(0 != res)
	{
		cout<<"setting detached attribute failed\n";
		exit(-1);
	}
	MasterInfo masterinfo ={this,num};
	res = pthread_create(&a_thread,&thread_attr,_threadctrlmaster<MasterInfo,GetWeb,&GetWeb::ctrl_master>,&masterinfo);
	if(0 != res)
	{
		cout<<"create thread error\n";
	}
	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGALRM);
	pthread_sigmask(SIG_SETMASK,&sigset,NULL);
	(void)pthread_attr_destroy(&thread_attr);
}
/*
void GetWeb::ctrl_RecvDate(void* arg)
{

}
*/
void GetWeb::ctrl_master(int HZ )
{
	
	pthread_t a_thread;
	int res = pthread_create(&a_thread,NULL,_threadEpollWait<GetWeb,&GetWeb::ctrl_work>,(void*)this);
	if(0 != res)
	{
		cout<<"create thread error\n";
	}
	ctrl_first();
	signal(SIGALRM, _callsigal<GetWeb,&GetWeb::sigalrm_handler>);
	Settimer(HZ);
// 	while(true)
// 	{
// 		pause();
// 	}
	pthread_join(a_thread,NULL);
}

void GetWeb::ctrl_work()
{
	
	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGALRM);
	pthread_sigmask(SIG_SETMASK,&sigset,NULL);
	
	
	struct epoll_event events[20];
	int fds = 0;
	char* buf = new char[RECVSIZE];
	WebInfo* webinfo = NULL;
	while( m_webinfonum.m_coun > 0)
	{
		fds = 0;
		while(!fds)
		{
			fds = WaitEpoll(m_epollfd,events,sizeof(events)/sizeof(events[0]),100);
		}
		for(int i = 0; i < fds; i++)
		{
			webinfo = static_cast<WebInfo*>(events[i].data.ptr);
			if(events[i].events&EPOLLIN)
			{
				webinfo->buf = buf;
				RecvData(webinfo);
				sem_post(&m_sem_send);
				if(m_savewebinfo)
				{
					m_outdatemanager.AddOutdate(webinfo);
				}
				if(m_sendtomem)
				{
					AddWebinfoToMemcached(webinfo);
				}
			}
			
			if(events[i].events&EPOLLOUT)
			{
				SendDate(events[i].data.ptr);
				ModEpoll(m_epollfd,webinfo->hostinfo.fd,EPOLLIN,(WebInfo*)events[i].data.ptr);
			}
		}
		
	}
	cout<<"work_thread over\n";
	delete[] buf;
	m_finished = true;
	return ;
}


void GetWeb::Settimer(int HZ)
{
	int sec = 0;
	long usec = 0;
	if(1 == HZ)
	{ 
		sec = HZ;
		usec = 0;
	}
	else
	{
		usec = (1000/HZ)*1000;
	}
	struct itimerval timerval;
	timerval.it_interval.tv_sec = sec;
	timerval.it_interval.tv_usec = usec;
	timerval.it_value.tv_sec = 0;
	timerval.it_value.tv_usec = usec;
	setitimer(ITIMER_REAL,&timerval,NULL);
	
}

void GetWeb::sigalrm_handler(int sig)
{
	sem_wait(&m_sem_send);
	ctrl_first();
}

void GetWeb::CreatSocket(WebInfo* webinfo)
{
	//MyLocalLock mlocal(&m_mutex_createsocket);
	int cfd = -1;
    if(-1 == (cfd = socket(AF_INET,SOCK_STREAM,0)))
    {
         printf("create socket failed of client!\n");
         exit(-1);
    }
    else
	{
		webinfo->fd = webinfo->hostinfo.fd = cfd;
	}
}

void GetWeb::Connect(WebInfo* webinfo)
{
	int cfd = webinfo->fd;
	setnotblock(cfd);
	int cc;
    if(-1 == (cc = connect(cfd,(struct sockaddr*)&(webinfo->hostinfo.caadd),(socklen_t)sizeof(webinfo->hostinfo.caadd))))
    {
		if(errno == EINPROGRESS)
		{
			//printf("connect ...\n");
			AddEpoll(m_epollfd,cfd,EPOLLOUT,webinfo);
		}
	}
    else
	{
		SendDate(webinfo);
		//setnotblock(cfd);
		ModEpoll(m_epollfd,cfd,EPOLLIN,webinfo);
		//AddEpoll(m_epollfd,cfd,EPOLLIN,webinfo);
		return;
	}
  
}

void GetWeb::setnotblock(int fd)
{
	if(fd > 2)
	{
		int flags;
		flags = fcntl(fd, F_GETFL, 0);
		if (flags < 0)
		{
			perror("get socket flags fail.\n");
			return;
		}
		
		if (fcntl(fd, F_SETFL, flags|O_NONBLOCK) < 0) 
		{
			perror("set socket O_NONBLOCK fail.\n");
			return ;
		}
	}
}

void GetWeb::SendDate(void* arg)
{
	WebInfo* pwebinfo = static_cast<WebInfo*>(arg);
	int fd = pwebinfo->hostinfo.fd;
	if(fd < 3)
	{return;}
	int len;  
	// send data
	char request[BUFSIZE];
	//char host[BUFSIZE],GET[BUFSIZE];
	memset(request,0,BUFSIZE);
	strcat(request,"GET /s?q=");//only www.so.com
	//strcat(request,conif.GET);
	strcat(request,+pwebinfo->key.c_str());
	strcat(request," HTTP/1.1\r\n");//至此为http请求行的信息
	strcat(request,"HOST: ");
	strcat(request,pwebinfo->hostinfo.host);
	strcat(request,"\r\n");
	strcat(request,"Connection: close\r\n");
	strcat(request,"Cache-Control: no-cache\r\n\r\n");
	int cs;
	//if(-1 == (cs = send(fd,request,strlen(request),0)))
	if(-1 == (cs = socket_send(fd,request,strlen(request),0)))
	{
		printf("向服务器发送请求的request失败!\n");
		exit(1);
	}	
}

ssize_t GetWeb::socket_send(int fd, const char* buffer, size_t buflen,int param)
{
	ssize_t tmp;
	size_t total = buflen;
	const char *p = buffer;
	while(true)
	{
		tmp = send(fd, p, total, 0);
		if(tmp < 0)
		{
			// 当send收到信号时,可以继续写,但这里返回-1.
			if(errno == EINTR)
			{return -1;}

			// 当socket是非阻塞时,如返回此错误,表示写缓冲队列已满,
			// 在这里做延时后再重试.
			if(errno == EAGAIN)
			{
				usleep(1000);
				continue;
			}

			return -1;
		}

		if((size_t)tmp == total)
		return buflen;
		total -= tmp;
		p += tmp;
	}

	return tmp;
}


void GetWeb::RecvData(void *arg)
{
	int len = RECVSIZE-1;
	int strl = -1;
	int sumstrl = 0;
	int rc = -1;
	WebInfo* webinfo = (WebInfo*)arg;
	char* pc = webinfo->buf;
	memset(pc,'\0',RECVSIZE);
	int fd = webinfo->hostinfo.fd;
	
	while(true)
	{
		int rc = recv(fd,pc,len,0);
		if(rc < 0)
		{
			if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
			{
				//cout<<"errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN\n";
				continue;
			}
			else
			{
				//cout<<"error1\n";
				pc = NULL;
				sumstrl = 0;
				break;
			}
			//cout<<"error2\n";
			break;
		}
		if(0 == rc)
		{
			//cout<<"0 == rc\n";
			 // 这里表示对端的socket已正常关闭.
			break;
		}
		if(len == rc)
		{
			cout<<"len == rc\n";
			int buflen = 0;
			char* ptemp = NULL;
			bool needfree = false;
			if(webinfo->buf)
			{
				cout<<"webinfo->buf\n";
				buflen = strlen(webinfo->buf);
				ptemp = webinfo->buf;
				webinfo->buf = NULL;
				needfree = false;
			}
			else if(webinfo->pwebinfo )
			{
				cout<<"webinfo->pwebinfo	len= "<<strlen(webinfo->pwebinfo)<<"	rc = "<<rc;
				buflen = strlen(webinfo->pwebinfo);
				cout<<"	buflen "<<buflen<<endl;
				ptemp = webinfo->pwebinfo;
				needfree = true;
			}
			webinfo->pwebinfo = new char[buflen+RECVSIZE];
			if(webinfo->pwebinfo)
			{
				memcpy(webinfo->pwebinfo,ptemp,buflen);
				pc = webinfo->pwebinfo + buflen;
				memset(pc,'\0',RECVSIZE);
				sumstrl += len;
				rc = 0;
				len = RECVSIZE - 1;
				cout<<"len ="<<len<<endl;
				if(needfree)
				{
					delete[] ptemp;
				}
				continue;
			}
			else
			{
				cout<<"new fail\n";
				exit(-11);
			}
		}
		//cout<<"len != rc\n";
		strl = strlen(pc);
		sumstrl += strl;
		pc = pc + strl;
		len -= strl;  
		
	}
	DelEpoll(m_epollfd,fd);
	close(fd);
	webinfo->webinfo_len = sumstrl;
	--m_webinfonum;
	if(0 == sumstrl)
	{
		std::cout<<webinfo->key<<"	fuck!\n";
	}
	std::cout<<"getwebinfo len "<<sumstrl/1024<<"KB	"<<m_webinfonum.m_coun<<"\n";
	
}

void GetWeb::readkeyfile()
{
	ifstream fin(m_keyfilepath,ios::in);
	string key;
	if(fin)
	{
		InputDate temp;
		//int num = 4000;
		m_inputdates.push_back(temp);
		while(fin >> m_inputdates.back().key && m_num)
		{
			if(fin.eof())
			{
				m_inputdates.pop_back();
				break;
			}
			m_inputdates.push_back(temp);
			--m_num;
		}
	}
}

void GetWeb::readurlfile()
{
	ifstream fin(m_urlfilepath,ios::in);
	string url;
	if(fin)
	{
		UrlInfo temp;
		string url;
		while(fin>>url)
		{
			if("" != url)
			{
				temp.url = url;
				m_urls.push_back(temp);
			}
		}
	}
	
}

void GetWeb::inithostinfo()
{
	HostInfo temphost;
	UrlInfoList::iterator beg = m_urls.begin();
	UrlInfoList::iterator end = m_urls.end();
	cout<<"urls size is "<<m_urls.size()<<endl;
	for(;beg != end; ++beg)
	{
		cout<<beg->url<<endl;
		Gethost(beg->url,temphost);
		m_hosts.push_back(temphost);
	}
}
OutDateManager::OutDateManager()
{
	int res=pthread_rwlock_init(&m_rwlock,NULL);//初始化读写锁
	if (res != 0)
	{
		perror("rwlock initialization failed");
	}
}

OutputdateList OutDateManager::GetWebinfoBykey(string key)
{
	MyLocalRLock mylock(&m_rwlock);
	return m_outdates[key];
}

bool OutDateManager::AddOutdate(WebInfo* webinfo)
{
	OutputDate temp;
	//temp.init(webinfo);
	MyLocalWLock mylock(&m_rwlock);
	m_outdates[webinfo->key].push_back(temp);
	m_outdates[webinfo->key].back().init(webinfo);
}

bool OutDateManager::DelOutdate(string key)
{
	MyLocalWLock mylock(&m_rwlock); 
	OutputdateList::iterator beg = m_outdates[key].begin();
	OutputdateList::iterator end = m_outdates[key].end();
	for(;beg != end; ++beg)
	{
		beg->relase();
	}
	return m_outdates.erase(key) > 0 ? true : false;
}

OutDateManager::~OutDateManager()
{
	OutDateMap::iterator begmap = m_outdates.begin();
	OutDateMap::iterator endmap = m_outdates.end();
	OutputdateList::iterator beg;
	OutputdateList::iterator end;
	for(;begmap != endmap; ++begmap)
	{
		beg = begmap->second.begin();
		end = begmap->second.end();
		for(;beg != end; ++beg)
		{
			beg->relase();
		}
	}
	m_outdates.clear();
}

//}

