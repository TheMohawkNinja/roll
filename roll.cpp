#include <iostream>
#include <string>
#include <sstream>
#include <ctype.h>
#include <ctime>
#include <fstream>
#include <unistd.h>
#include <climits>

bool fexists(const char *filename)
{
	std::ifstream ifile(filename);
	return (bool)ifile;
}

int main(int argc, char *argv[])
{
	bool onlyRolls=false;
	bool onlyTotal=false;
	int count, size, mod, rndhold, countLength, iLength;
	int runctr, seed;
	int total=0;
	std::string roll="";
	std::string countstr="";
	std::string sizestr="";
	std::string modstr="";
	std::string runctr_str="";
	std::string format;
	std::string user=getlogin();
	std::ofstream ws;
	std::ifstream rs;

	//Handle args
	if(argc<2)
	{
		fprintf(stderr,"Dice roll is null\n");
		return -1;
	}

	for(int i=0; i<argc; i++)
	{
		//Flags
		if(i<argc&&std::string(argv[i]).substr(0,1)=="-")
		{
			if(std::string(argv[i])=="-r")//Flag for only showing the rolls
			{
				if(!onlyTotal)
				{
					onlyRolls=true;
				}
				else
				{
					fprintf(stderr,"Found args for both -r and -t. These are mutually exclusive!\n");
					return -3;
				}
			}
			else if(std::string(argv[i])=="-t")//Flag for only showing total
			{
				if(!onlyRolls)
				{
					onlyTotal=true;
				}
				else
				{
					fprintf(stderr,"Found args for both -r and -t. These are mutually exclusive!\n");
					return -3;
				}
			}
			else if(std::string(argv[i])=="-?"||std::string(argv[i])=="--help")//Help text
			{
				fprintf(stdout,"roll: The simple terminal dice roller\n\n");
				fprintf(stdout,"SYNTAX \"roll [-rt[?-help]] [count]d[size][+-][modifier]\"\n\n");
				fprintf(stdout,"-r\n");
				fprintf(stdout,"Only show rolls\n\n");
				fprintf(stdout,"-t\n");
				fprintf(stdout,"Only show total\n\n");
				fprintf(stdout,"-? or --help\n");
				fprintf(stdout,"Print this help text\n\n");
				fprintf(stdout,"count\n");
				fprintf(stdout,"The number of dice to roll\n\n");
				fprintf(stdout,"size\n");
				fprintf(stdout,"The number of sides on the die(ce)\n\n");
				fprintf(stdout,"modifier\n");
				fprintf(stdout,"A modifier that is added to, or subtracted from, the total\n\n");
				fprintf(stdout,"EXAMPLES\n\n");
				fprintf(stdout,"roll d20\n");
				fprintf(stdout,"roll 2d6\n");
				fprintf(stdout,"roll 4d8+2\n");
				fprintf(stdout,"roll -r 3d6\n");
				fprintf(stdout,"roll -t 2d10+6\n");
				return 0;
			}
			else
			{
				fprintf(stderr,"Unknown arg \"%s\"\n",std::string(argv[i]).c_str());
				return -2;
			}
		}

		//Try parsing a roll format for XdY
		if(std::string(argv[i]).find("d")!=std::string::npos)
		{
			try
			{
				std::stoi(std::string(argv[i]).substr(std::string(argv[i]).find("d")+1,1));
				roll=argv[i];
			}
			catch(...)
			{
				fprintf(stderr,"Invalid roll format (Unable to convert dice size to integer)\n");
				return -4;
			}
		}
		else if(i==argc)
		{
			fprintf(stderr,"Invalid roll format (no 'd' found)\n");
			return -4;
		}

		//If a roll is never established
		if(i==argc&&roll=="")
		{
			fprintf(stderr,"No arguments appear to contain a dice roll\n");
			return -5;
		}
	}

	//Open roll counter file
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
		runctr=stoi(runctr_str);

		if((time(NULL)+(runctr*runctr))>=INT_MAX)
		{
			runctr=1;
		}
		rs.close();
	}

	//Parse through dice roll
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

		try
		{
			count=stoi(countstr);
		}
		catch(...)
		{
			fprintf(stderr,"Can't parse dice count");
		}
	}

	//Get size of dice
	for(int i=roll.find("d")+1; i<roll.length()&&roll.substr(i,1)!="+"&&roll.substr(i,1)!="-"; i++)
	{
		sizestr+=roll.substr(i,1);

		if(i==roll.length()&&sizestr=="")
		{
			fprintf(stderr,"Invalid roll format ('d' found, but no modifier value located)\n");
			return -4;
		}
	}

	try
	{
		size=stoi(sizestr);
	}
	catch(...)
	{
		fprintf(stderr,"Invalid roll format (Unable to convert dice size into integer)\n");
		return -4;
	}

	//Get modifer
	if(roll.find("+")!=std::string::npos)
	{
		for(int i=roll.find("+"); i<roll.length(); i++)
		{
			modstr+=roll.substr(i,1);
		}

		try
		{
			mod=stoi(modstr);
		}
		catch(...)
		{
			fprintf(stderr,"Invalid roll format (Unable to convert positive roll modifier to integer)\n");
			return -4;
		}
	}
	else if(roll.find("-")!=std::string::npos)
	{
		for(int i=roll.find("-"); i<roll.length(); i++)
		{
			modstr+=roll.substr(i,1);
		}

		try
		{
			mod=stoi(modstr);
			fprintf(stdout,"Mod=%d",mod);
		}
		catch(...)
		{
			fprintf(stderr,"Invalid roll format (Unable to convert negative roll modifier to integer)\n");
			return -4;
		}
	}
	else
	{
		mod=0;
	}

	//Roll dice
	if(!onlyRolls&&!onlyTotal)
	{
		fprintf(stdout,"Rolling \033[1m%s\033[0m\n\n",roll.c_str());
	}

	seed=(time(NULL)+(runctr*runctr));
	srand(seed);

	//Output rolls
	if(count>1)
	{
		for(int i=0; i<count; i++)
		{
			rndhold=(rand()%size)+1;
			total+=rndhold;

			if(!onlyRolls&&!onlyTotal)
			{
				countLength=countstr.length();
				iLength=std::to_string(i+1).length();
				fprintf(stdout,"Roll ");
			
				for(int s=countstr.length(); s>std::to_string(i+1).length(); s--)
				{
					fprintf(stdout," ");
				}

				format=("%d%s");
				fprintf(stdout,format.c_str(),i+1,": ");

				for(int s=sizestr.length(); s>std::to_string(rndhold).length(); s--)
				{
					fprintf(stdout," ");
				}

				fprintf(stdout,"\033[1m%d\033[0m\n",rndhold);
			}
			else if(onlyRolls)
			{
				fprintf(stdout,"%d\n",rndhold);
			}
		}
	}
	else
	{
		rndhold=(rand()%size)+1;

		if(!onlyTotal&&!onlyRolls)
		{
			fprintf(stdout,"Natural roll: \033[1m%d\033[0m\n",rndhold);
			fprintf(stdout,"Modifier    : \033[1m%d\033[0m\n",mod);
			fprintf(stdout,"Total roll  : \033[1m%d\033[0m\n",rndhold+mod);
		}
		else if(onlyRolls)
		{
			fprintf(stdout,"%d\n",rndhold+mod);
		}
	}

	//Output total
	if(count>1)
	{
		if(!onlyTotal&&!onlyRolls)
		{
			fprintf(stdout,"\n");
			fprintf(stdout,"Natural roll: \033[1m%d\033[0m\n",total);
			fprintf(stdout,"Modifier    : \033[1m%d\033[0m\n",mod);
			fprintf(stdout,"Total roll  : \033[1m%d\033[0m\n",total+mod);
		}
		else if(onlyTotal)
		{
			fprintf(stdout,"%d\n",total+mod);
		}
	}

	ws.open("/home/"+user+"/.rollctr");
        ws<<std::to_string(runctr+1);
        ws.close();

	return 0;
}
