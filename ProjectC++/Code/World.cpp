#include "World.h"


World::World( Observer* observer, const std::string& fileName) throw(const char*) : 
	zSectors(NULL), 
	zNrOfSectorsWidth(0), 
	zNrOfSectorsHeight(0), 
	zFileName(fileName)
{
	addObserver(observer);
	notifyObservers( &WorldLoadedEvent(this) );

	if ( !fileName.empty() )
	{
		zFileName = fileName;
		zFile.open( zFileName.c_str() );

		if ( !zFile.good() )
			throw("Failed Opening File!");
	}
}


World::World( Observer* observer, unsigned int nrOfSectorWidth, unsigned int nrOfSectorHeight ) :
	zNrOfSectorsWidth(nrOfSectorWidth),
	zNrOfSectorsHeight(nrOfSectorHeight)
{
	addObserver(observer);

	this->zSectors = new Sector**[this->zNrOfSectorsWidth];
	for(unsigned int i = 0; i < this->zNrOfSectorsWidth; i++)
	{
		this->zSectors[i] = new Sector*[this->zNrOfSectorsHeight];
		for( unsigned int o=0; o < this->zNrOfSectorsHeight; ++o )
		{
			this->zSectors[i][o] = NULL;
		}
	}
}


World::~World()
{
	// Close File
	zFile.close();

	// Delete the zSectors pointers.
	if ( this->zSectors )
	{
		for(unsigned int i = 0; i < this->zNrOfSectorsWidth; i++)
		{
			for(unsigned int o=0; o < this->zNrOfSectorsHeight; o++)
			{
				if ( this->zSectors[i][o] )
				{
					delete this->zSectors[i][o];
					this->zSectors[i] = NULL;
				}
			}
			delete [] this->zSectors[i];
			this->zSectors[i] = 0;
		}

		//Delete the zSector pointer.
		delete [] this->zSectors;
		this->zSectors = NULL;
	}
}


bool World::ModifyPoint( Vector2 pos, float value )
{
	Sector* sector = GetSectorAtWorldPos( pos );
	Vector2 localPos = Vector2(fmod(pos.x, (float)SECTOR_LENGTH), fmod(pos.y, (float)SECTOR_LENGTH));
	return sector->ModifyPoint(localPos.x, localPos.y, value);
}


bool World::SaveFile( const std::string& fileName ) throw(const char*)
{
	if ( !zFile.good() )
	{
		zFile.open(fileName);

		if ( !zFile.good() )
			throw("Failed Accessing File!");

		// Check Map Size

	}

	return true;
}


Sector* World::GetSectorAtWorldPos( const Vector2& pos ) throw(const char*)
{
	return GetSector( Vector2(pos.x / (float)SECTOR_LENGTH, pos.y / (float)SECTOR_LENGTH) );
}


Sector* World::GetSector( const Vector2& pos ) throw(const char*)
{
	if( (this->zNrOfSectorsWidth) <= pos.x || (this->zNrOfSectorsHeight) <= pos.y )
		throw("Index Out Of Bounds");

	Sector* s = this->zSectors[(int)(pos.x)][(int)(pos.y)];

	if ( !s )
	{
		if ( !zFile.good() )
		{
			s = new Sector();
			this->zSectors[(int)(pos.x)][(int)(pos.y)] = s;
		}
		else
		{
			// Load From File
		}
	}

	return s;
}
