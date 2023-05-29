#include <iostream>
#include <string>
#include <sstream>
#include <ctype.h>
#include <ctime>
#include <fstream>
#include <unistd.h>
#include <climits>

int rollctr, seed;
std::string user=getlogin();
std::string rollctr_path="/home/"+user+"/.rollctr";
std::string rollctr_str="";
std::ifstream rs;
std::ofstream ws;

bool fexists(const char *filename)
{
	std::ifstream ifile(filename);
	return (bool)ifile;
}
void set_rollctr()
{
	//Open roll counter file
	if(!fexists(rollctr_path.c_str()))
	{
		ws.open(rollctr_path);
		ws<<"1";
		ws.close();
	}
	else
	{
		rs.open(rollctr_path);
		getline(rs,rollctr_str);
		rollctr=stoi(rollctr_str);

		if((rollctr*rollctr)>=INT_MAX)
		{
			rollctr=1;
		}
		rs.close();
	}
	seed=(time(NULL)+(rollctr*rollctr));
	srand(seed);
}
void append_rollctr()
{
	ws.open("/home/"+user+"/.rollctr");
        ws<<std::to_string(rollctr+1);
        ws.close();
}

int main(int argc, char *argv[])
{
	bool onlyRolls=false;
	bool onlyTotal=false;
	bool usingList=false;
	int count, size, mod, rndhold, countLength, iLength;
	int faces_size=0;
	int total=0;
	std::string italics="\u001b[3m";
	std::string normal="\u001b[0m";
	std::string roll="";
	std::string countstr="";
	std::string sizestr="";
	std::string modstr="";
	std::string format;
	std::string list="";
	std::string temp;
	std::string* faces;

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
					return -1;
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
					return -1;
				}
			}
			else if(std::string(argv[i])=="-l")//Flag for custom list
			{
				if(!onlyTotal)
				{
					usingList=true;
				}
				else
				{
					fprintf(stderr,"Found args for both -l and -t. These are mutually exclusive!\n");
					return -1;
				}

				if(!argv[i++])
				{
					fprintf(stderr,"No file specified!\n");
					return -1;
				}
				else
				{
					list=argv[i++];
					if(!fexists((list).c_str()))
					{
						fprintf(stderr,("File \""+list+"\" does not exist!\n").c_str());
						return -1;
					}
					else
					{
						rs.open(list);
						while(rs >> temp)
						{
							getline(rs,temp);
							faces_size++;
						}
						faces=new std::string[faces_size];
						rs.close();

						rs.open(list);
						for(int i=0; i<faces_size; i++)
						{
							getline(rs,faces[i]);
						}
						rs.close();
					}

					set_rollctr();
					fprintf(stdout,(faces[rand()%faces_size]+"\n").c_str());
					append_rollctr();
					return 0;
				}
			}
			else if(std::string(argv[i])=="-?"||std::string(argv[i])=="--help")//Help text
			{
				fprintf(stdout,"roll: The simple terminal dice roller\n\n");
				fprintf(stdout,("SYNTAX \"roll [-r|-t|-l "+italics+"file"+normal+"[-?|--help]] ["+italics+"count"+normal+"]d["+italics+"size"+normal+"]["+italics+"+"+normal+"|"+italics+"-"+normal+"]["+italics+"modifier"+normal+"]\"\n\n").c_str());
				fprintf(stdout,"-r\n");
				fprintf(stdout,"Only show rolls\n\n");
				fprintf(stdout,"-t\n");
				fprintf(stdout,"Only show total\n\n");
				fprintf(stdout,("-l "+italics+"file"+normal+"\n").c_str());
				fprintf(stdout,"Use custom newline-delimited list for die faces\n\n");
				fprintf(stdout,"-? or --help\n");
				fprintf(stdout,"Print this help text\n\n");
				fprintf(stdout,(italics+"count"+normal+"\n").c_str());
				fprintf(stdout,"The number of dice to roll\n\n");
				fprintf(stdout,(italics+"size"+normal+"\n").c_str());
				fprintf(stdout,"The number of sides on the die(ce)\n\n");
				fprintf(stdout,(italics+"modifier"+normal+"\n").c_str());
				fprintf(stdout,"A modifier that is added to, or subtracted from, the total\n\n");
				fprintf(stdout,"EXAMPLES\n\n");
				fprintf(stdout,"roll d20\n");
				fprintf(stdout,"roll 2d6\n");
				fprintf(stdout,"roll 4d8+2\n");
				fprintf(stdout,"roll -r 3d6\n");
				fprintf(stdout,"roll -t 2d10+6\n");
				fprintf(stdout,"roll -l test\n");
				return 0;
			}
			else
			{
				fprintf(stderr,"Unknown arg \"%s\"\n",std::string(argv[i]).c_str());
				return -1;
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
				return -1;
			}
		}
		else if(i==argc)
		{
			fprintf(stderr,"Invalid roll format (no 'd' found)\n");
			return -1;
		}

		//If a roll is never established
		if(i==argc&&roll=="")
		{
			fprintf(stderr,"No arguments appear to contain a dice roll\n");
			return -1;
		}
	}

	set_rollctr();

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
			return -1;
		}
	}

	//Get size of dice
	for(int i=roll.find("d")+1; i<roll.length()&&roll.substr(i,1)!="+"&&roll.substr(i,1)!="-"; i++)
	{
		sizestr+=roll.substr(i,1);

		if(i==roll.length()&&sizestr=="")
		{
			fprintf(stderr,"Invalid roll format ('d' found, but no modifier value located)\n");
			return -1;
		}
	}

	try
	{
		size=stoi(sizestr);
	}
	catch(...)
	{
		fprintf(stderr,"Invalid roll format (Unable to convert dice size into integer)\n");
		return -1;
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
			return -1;
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
		}
		catch(...)
		{
			fprintf(stderr,"Invalid roll format (Unable to convert negative roll modifier to integer)\n");
			return -1;
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

	append_rollctr();
	return 0;
}
