#include "osm_base.hpp"
map <string, int> every_member::kodyKey;
vector <string> every_member::kodyKey2;
map<string, string> every_member::getTags() const
{
	map <string, string> wynik;
	for(int i=0; i<tagiSize; i++)
	{
		wynik[kodyKey2[tagi1[i]]]=kodyKey2[tagi2[i]];
	}
	return wynik;
}
void every_member::setTags (map<string, string> tags)
{
	if(tagi1!=NULL)
	{
		delete tagi1;
		tagi1=NULL;
		delete tagi2;
		tagi2=NULL;
	}
	tagiSize=tags.size();
	if(tags.size()>0)
	{
		tagi1 = new int[tags.size()];
		tagi2 = new int[tags.size()];
	}
	auto it1=tags.begin();
	int licznik=0;
	while(it1!=tags.end())
	{
		if(kodyKey.find(it1->first)==kodyKey.end())
		{
			int s1=kodyKey2.size();
			kodyKey.insert(make_pair(it1->first, s1));
			kodyKey2.push_back(it1->first);
		}
		if(kodyKey.find(it1->second)==kodyKey.end())
		{
			int s1=kodyKey2.size();
			kodyKey.insert(make_pair(it1->second, s1));
			kodyKey2.push_back(it1->second);
		}
		tagi1[licznik]=kodyKey[it1->first];
		tagi2[licznik]=kodyKey[it1->second];
		licznik++;
		it1++;
	}
}
every_member::every_member()
{
	tagi1=NULL;
	tagi2=NULL;
	tagiSize=0;
	modify=0;
	todelete=0;
}
every_member::every_member(every_member& foo)
{
	tagi1=NULL;
	tagi2=NULL;
	tagiSize=0;
	version=foo.version;
	id=foo.id;
	modify=foo.modify;
	todelete=foo.todelete;
	setTags(foo.getTags());
}
every_member::every_member(const every_member& foo)
{
	tagi1=NULL;
	tagi2=NULL;
	tagiSize=0;
	version=foo.version;
	id=foo.id;
	modify=foo.modify;
	todelete=foo.todelete;
	map<string,string> tags=foo.getTags();
	setTags(tags);
}
every_member& every_member::operator=(every_member const& foo)
{
	tagi1=NULL;
	tagi2=NULL;
	tagiSize=0;
	version=foo.version;
	id=foo.id;
	modify=foo.modify;
	todelete=foo.todelete;
	map<string,string> tags=foo.getTags();
	setTags(tags);

}
every_member::~every_member()
{
	if(tagi1!=NULL)
	{
		delete[] tagi1;
		delete[] tagi2;
	}
}
