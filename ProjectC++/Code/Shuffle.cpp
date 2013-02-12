#include "Shuffle.h"
#include <fstream>


ShuffleList::ShuffleList( const std::string& fileName )
{
	std::ifstream file(fileName);

	unsigned int curGroupNumber = 0;

	while ( file.good() )
	{
		std::string curLine;
		std::getline(file, curLine);
		
		// Force lowercase
		for( unsigned int x=0; x<curLine.size(); ++x )
		{
			curLine[x] = (char)tolower(curLine[x]);
		}

		if ( curLine.find("number:") == 0 )
		{
			curGroupNumber = atoi(curLine.substr(7, curLine.length() - 7).c_str());
		}
		else if ( curLine.find("name:") == 0 )
		{
			zGroups[curGroupNumber-1].zName = curLine.substr(5, curLine.length() - 5);
		}
		else if ( curLine.find("find:") == 0 )
		{
			// Entity ID
			unsigned int eID;

			// Scan
			sscanf_s(curLine.c_str(), "find:%d", &eID);

			// Insert
			zGroups[curGroupNumber].zFindIDs.insert(eID);
		}
		else if ( curLine.find("replace:") == 0 )
		{
			// Entity ID
			unsigned int eID;
			float chance = 0.0f;

			// Scan
			sscanf_s(curLine.c_str(), "replace:%d,%f", &eID, &chance);

			// Insert
			zGroups[curGroupNumber].zNewIDs[eID] = chance;
		}
	}

	file.close();
}

bool ShuffleList::IsPartOfGroup( unsigned int groupNum, unsigned int entType )
{
	if ( groupNum == 0 ) return false;

	// Find Group
	auto i = zGroups.find(groupNum);
	if ( i == zGroups.end() )
		return false;

	// Find Part
	auto i2 = i->second.zFindIDs.find(entType);
	if ( i2 == i->second.zFindIDs.end() )
		return false;

	return true;
}

unsigned int ShuffleList::RandomizeNewType( unsigned int groupNum )
{
	if ( groupNum == 0 ) throw("Invalid Group Number!");

	// Find Group
	auto i = zGroups.find(groupNum);
	if ( i == zGroups.end() )
		throw("Nonexistant Group Number!");

	// Generate List
	float totalValue = 0.0f;
	std::vector< std::pair<float, unsigned int > > values;
	for( auto it = i->second.zNewIDs.begin(); it != i->second.zNewIDs.end(); ++it )
	{
		totalValue += it->second;
		values.push_back( std::pair<float, unsigned int>(totalValue, it->first) );
	}

	// Create A Random List
	float value = ((float)rand() / (float)RAND_MAX) * totalValue;

	for( unsigned int x=0; x<values.size(); ++x )
	{
		if ( values[x].first >= value )
			return values[x].second;
	}

	return 0;
}