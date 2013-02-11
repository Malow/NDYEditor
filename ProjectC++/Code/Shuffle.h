#pragma once

#include <map>
#include <vector>
#include <string>


class ShuffleList
{
	struct ListData
	{
		std::vector< unsigned int > zFindIDs;
		std::vector< std::pair< unsigned int, float > > zNewIDs;
	};

	std::map< unsigned int, ListData > zLists;

public:
	ShuffleList( const std::string& fileName );
};
