#include <iostream>
#include <string>
#include <sstream>
#include <ctype.h>
#include <ctime>
#include <fstream>
#include <unistd.h>
#include <climits>

using namespace std;

bool fexists(const char *filename)
{
	ifstream ifile(filename);
	return (bool)ifile;
}

int to_int(string str)
{
	int i;
	std::istringstream s (str);
	s >> i;
	//cout<<"String: "<<str<<", int: "<<i<<endl;
	return i;
}

int main(int argc, char *argv[])
{
	int count, size, mod, rndhold, countLength, iLength;
	int runctr, seed;
	int total=0;
	string countstr="";
	string sizestr="";
	string modstr="";
	string runctr_str="";
	string format;
	string user=getlogin();
	string roll=argv[1];
	ofstream ws;
	ifstream rs;

	if(!fexists(("/home/"+user+"/.rollctr").c_str()))
	{
		ws.open("/home/"+user+"/.rollctr");
		ws<<"1";
		ws.close();
	}
	else
	{
		rs.open("/home/"+user+"/.rollctr");
		getline(rs,runctr_str);
		runctr=to_int(runctr_str);

		if((time(NULL)+(runctr*runctr))>=INT_MAX)
		{
			runctr=1;
		}
		rs.close();
	}

	//Handle bad argument count
	if(argc<2)
	{
		cout<<"ERROR: Dice roll is null."<<endl;
	}
	else if(argc>2)
	{
		cout<<"ERROR: Expected only one argument."<<endl;
	}

	//Parse through dice roll
	else
	{
		if(roll.substr(0,1)=="d")
		{
			count=1;
		}
		else
		{
			//Get number of dice
			for(int i=0; i<roll.length()&&roll.substr(i,1)!="d"; i++)
			{
				countstr+=roll.substr(i,1);
			}

			count=to_int(countstr);
		}

		//Get size of dice
		for(int i=roll.find("d")+1; i<roll.length()&&roll.substr(i,1)!="+"; i++)
                {
                        sizestr+=roll.substr(i,1);
		}

		size=to_int(sizestr);

		//Get modifer
		if(roll.find("+")!=std::string::npos)
		{
                       	for(int i=roll.find("+"); i<roll.length(); i++)
                      	{
                       		modstr+=roll.substr(i,1);
               		}

                       	mod=to_int(modstr);
		}
		else if(roll.find("-")!=std::string::npos)
                {
                        for(int i=roll.find("-"); i<roll.length(); i++)
                        {
                                modstr+=roll.substr(i,1);
                        }

                        mod=to_int(modstr);
                }
		else
		{
			mod=0;
		}
	}

	//Roll dice
	seed=(time(NULL)+(runctr*runctr));
	//cout<<"Seed: "+(to_string(seed))<<endl;
	srand(seed);

	//cout<<endl;

	if(count>1)
	{
		for(int i=0; i<count; i++)
		{
			rndhold=(rand()%size)+1;
			countLength=countstr.length();
			iLength=to_string(i+1).length();
			printf("Roll ");
		
			for(int s=countstr.length(); s>to_string(i+1).length(); s--)
			{
				printf(" ");
			}

			format=("%d%s");
			printf(format.c_str(),i+1,": ");

			for(int s=sizestr.length(); s>to_string(rndhold).length(); s--)
			{
				printf(" ");
			}

			printf("%d\n",rndhold);

			total+=rndhold;
		}
	}
	else
	{
		rndhold=(rand()%size)+1;
		printf("Natural roll: %d\n",rndhold);
		printf("Modifier    : %d\n",mod);
		printf("Total roll  : %d\n",rndhold+mod);
	}

	if(count>1)
	{
		cout<<endl;
		printf("Natural roll: %d\n",total);
                printf("Modifier    : %d\n",mod);
                printf("Total roll  : %d\n",total+mod);
	}

	ws.open("/home/"+user+"/.rollctr");
        ws<<to_string(runctr+1);
        ws.close();

	return 0;
}
