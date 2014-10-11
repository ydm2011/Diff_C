#include <iostream>
#include "getweb.h"

GetWeb gb;
int main(int argc, char **argv) 
{
	Engineparam engineparam;
	engineparam.keyfilepath = "key.txt";
	engineparam.urlfilepath = "url.txt";
	engineparam.keynum = 20;
	engineparam.memcachedhostaddr = "--SERVER=test2.se.gzst.qihoo.net:11211";
	engineparam.sendtomemcached = false;
	engineparam.savewebinfo = true;
	engineparam.HZ = 10;
	engineparam.urlparam = "";
	engineparam.port = 80;
	
	gb.init(engineparam);
    gb.ctrl_run(10);
	//gb.run(1);
	
	while(!gb.finish())
	{
		
		sleep(1);
	
	}
	
	OutputdateList dates = gb.GetOutDateManager().GetWebinfoBykey("美女qq号码");
	for(OutputdateList::iterator beg = dates.begin();
		beg != dates.end(); ++beg)
	{
		//char* pc = new char[beg->webinfo_len+1];
		//pc
		cout<<beg->pwebinfo<<endl;
		cout<<beg->webinfo_len<<endl;
	}
	gb.GetOutDateManager().DelOutdate("美女qq号码");
	
	dates = gb.GetOutDateManager().GetWebinfoBykey("美女qq号码");
	for(OutputdateList::iterator beg = dates.begin();
		beg != dates.end(); ++beg)
	{
		//char* pc = new char[beg->webinfo_len+1];
		//pc
		cout<<beg->pwebinfo<<endl;
		cout<<beg->webinfo_len<<endl;
	}
	
	OutDateManager* pout = gb.GetOutDateManagerPoint();
	dates = pout->GetWebinfoBykey("丘比龙");
	for(OutputdateList::iterator beg = dates.begin();
		beg != dates.end(); ++beg)
	{
		cout<<beg->pwebinfo<<endl;
		cout<<beg->webinfo_len<<endl;
	}
	pout->DelOutdate("丘比龙");
	//delete pout;
	
	
	
    return 0;
}
