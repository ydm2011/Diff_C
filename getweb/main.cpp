#include <iostream>
#include "getweb.h"

GetWeb gb;
int main(int argc, char **argv) 
{
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
	/*
	OutputdateList dates = gb.GetOutDateManager().GetWebinfoBykey("美女qq号码");
	for(OutputdateList::iterator beg = dates.begin();
		beg != dates.end(); ++beg)
	{
		
	}
	gb.GetOutDateManager().DelOutdate("美女qq号码");
	
	OutDateManager* pout = gb.GetOutDateManagerPoint();
	dates = pout.GetWebinfoBykey("丘比龙");
	for(OutputdateList::iterator beg = dates.begin();
		beg != dates.end(); ++beg)
	{
		
	}
	pout->DelOutdate("丘比龙");
	delete pout;
	
	*/
	while(!gb.finish())
	{
		
		sleep(10);
	
	}
    return 0;
}
