#include "ScheduleReader/ScheduleReader.hpp"
using namespace std;

class MyHand : public ScheduleHandler
{
  public:
  void nowa_linia(string nazwa, vector <vector <string> > trasy);
};

void MyHand::nowa_linia(string nazwa, vector <vector <string> > trasy)
{
  for(int i=0; i<trasy.size(); i++)
  {
    for(int j=0; j<trasy[i].size(); j++)
    {
      cout<<nazwa<<" "<<i+1<<" "<<j+1<<" "<<trasy[i][j]<<endl;
    }
  }
}

int main(int argc, char** argv)
{
  MyHand hnd;
  string tmp1 = argv[1];
  ScheduleReaderWarszawa reader(tmp1, &hnd);
  reader.run();
}
