#pragma once

#include <map>
#include <vector>
#include <string>
#include <set>


class ShuffleList
{
	struct ListData
	{
		std::set< unsigned int > zFindIDs;
		std::map< unsigned int, float > zNewIDs;
		std::string zName;
	};

	std::map< unsigned int, ListData > zGroups;

public:
	ShuffleList( const std::string& fileName );

	bool IsPartOfGroup( unsigned int groupNum, unsigned int entType );
	unsigned int RandomizeNewType( unsigned int groupNum );
};
