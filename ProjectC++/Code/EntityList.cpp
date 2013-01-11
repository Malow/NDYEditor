#include "EntityList.h"
#include <vector>
#include <string>
#include <fstream>


struct EntInfo
{
	std::string entName;
	std::string entModel;
};

static std::vector<EntInfo> entities;

void LoadEntList( const std::string& fileName ) throw(...)
{
	std::ifstream file(fileName);
	if ( !file.is_open() ) throw("File Not Found!");

	unsigned int curNumber = 0;
	while( !file.eof() )
	{
		std::string curLine;
		std::getline(file, curLine);
		
		// Force lowercase
		for( unsigned int x=0; x<curLine.size(); ++x )
		{
			curLine[x] = tolower(curLine[x]);
		}

		if ( curLine.substr(0,7) == "number:" )
		{
			curNumber = atoi(curLine.substr(7, curLine.length() - 7).c_str());
			if ( entities.size() < curNumber ) entities.resize(curNumber);
		}
		else if ( curLine.substr(0,5) == "name:" )
		{
			entities[curNumber-1].entName = curLine.substr(5, curLine.length() - 5);
		}
		else if ( curLine.substr(0,6) == "model:" )
		{
			entities[curNumber-1].entModel = curLine.substr(6, curLine.length() - 6);
		}
	}

	file.close();
}

const std::string& GetEntName( unsigned int entIndex ) throw(...)
{
	if ( entIndex-1 >= entities.size() ) throw("Index Out Of Bounds!");
	return entities[entIndex-1].entName;
}

const std::string& GetEntModel( unsigned int entIndex ) throw(...)
{
	if ( entIndex-1 >= entities.size() ) throw("Index Out Of Bounds!");
	return entities[entIndex-1].entModel;
}

unsigned int GetEntListSize()
{
	return entities.size();
}
