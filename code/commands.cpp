#include "commands.h"
#include <sstream>

void Execute(const std::string & input, const CommandTable & commands ) 
{
	//split string by white space. 
	std::vector<char* > args;
	std::string word;
	std::istringstream iss;
	iss.str(input);
	while (std::getline(iss, word, ' '))
	{
		char* copy = new char[word.size()+1];
		for(int i = 0; i < word.size(); i++)
			copy[i] = word[i];
		copy[word.size()] = '\0';
		args.push_back(copy);
	}
	Execute(args.size(), &args[0], commands);
	for (int i = 0; i < args.size(); i++)
		delete args[i];
}

void Execute(int argc, char** argv, const CommandTable& commands)
{
	Args args; 
	//ignore first arg
	int i = 0;
	while( i < argc)
	{
		CommandTable::const_iterator it = commands.find(argv[i]);
		if(it == commands.end())
		{
			printf("Execute: Invalid Command (%s)\n", argv[i]);
			return;
		}
		//parse until non cmd is found
		const Command & command  = it->second; 
		i++;
		while(i < argc)
		{
			const char *arg=  argv[i];
			it = commands.find(arg);
			if (it == commands.end()) //if does not exist
				args.push_back(arg);
			else
				break;
			i++;
		}
		command(args);
		args.clear();
	}
}