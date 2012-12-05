#include "World.h"


World::World( Observer* observer, const std::string& fileName) throw(const char*) : 
	zSectors(NULL), 
	zNrOfSectorsWidth(0), 
	zNrOfSectorsHeight(0)
{
	addObserver(observer);
	notifyObservers( &WorldLoadedEvent(this) );
	zFile = new WorldFile(this, fileName, false);
	zNrOfSectorsWidth = zFile->getWorldWidth();
	zNrOfSectorsHeight = zFile->getWorldHeight();
}


World::World( Observer* observer, unsigned int nrOfSectorWidth, unsigned int nrOfSectorHeight ) :
	zNrOfSectorsWidth(nrOfSectorWidth),
	zNrOfSectorsHeight(nrOfSectorHeight),
	zFile(0)
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

	notifyObservers( &WorldLoadedEvent(this) );
}


World::~World()
{
	// Close File
	if ( zFile ) delete zFile, zFile=0;

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

		// Delete the zSector pointer.
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


void World::SaveFile()
{
	if ( zFile )
	{
		for(unsigned int x=0; x<zNrOfSectorsWidth; ++x)
		{
			for(unsigned int y=0; y<zNrOfSectorsHeight; ++y)
			{
				if ( this->zSectors[x][y] && this->zSectors[x][y]->isEdited() )
				{
					zFile->writeHeightMap(this->zSectors[x][y]->GetHeightMap(),x,y);
					this->zSectors[x][y]->setEdited(false);
				}
			}
		}
	}
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
		s = new Sector();

		this->zSectors[(int)(pos.x)][(int)(pos.y)] = s;

		if ( zFile )
		{
			// Load From File
			zFile->readHeightMap(s->GetHeightMap(),pos.x,pos.y);
		}
		else
		{
			s->Reset();
			s->setEdited(true);
		}

		notifyObservers( &SectorLoadedEvent(this,pos) );
	}

	return s;
}


void World::onEvent( Event* e )
{
	if ( WorldHeaderLoadedEvent* WHL = dynamic_cast<WorldHeaderLoadedEvent*>(e) )
	{
		zNrOfSectorsWidth = WHL->file->getWorldWidth();
		zNrOfSectorsHeight = WHL->file->getWorldHeight();

		// World Has Been Loaded
		notifyObservers( &WorldLoadedEvent(this) );
	}
}


void World::LoadAllSectors()
{
	for( unsigned int x=0; x<zNrOfSectorsWidth; ++x )
	{
		for( unsigned int y=0; y<zNrOfSectorsHeight; ++y )
		{
			GetSector( Vector2(x,y) );
		}
	}
}
