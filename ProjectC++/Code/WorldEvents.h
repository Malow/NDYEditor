#pragma once

#include "Observer.h"

#pragma warning ( push )
#pragma warning ( disable : 4512 )

class WorldFile;
class WorldFileHeader;

class WorldHeaderLoadedEvent : public Event
{
public:
	WorldFile* file;
	const WorldFileHeader& header;

	WorldHeaderLoadedEvent( WorldFile* file, const WorldFileHeader& header ) :
		file(file),
		header(header)
	{
	}
};

class WorldHeaderCreateEvent : public Event
{
public:
	WorldFile* file;
	WorldFileHeader& header;

	WorldHeaderCreateEvent( WorldFile* file, WorldFileHeader& header ) : 
		file(file),
		header(header)
	{
	}
};

#pragma warning (pop)