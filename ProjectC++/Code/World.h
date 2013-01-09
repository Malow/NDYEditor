#pragma once

#include <string>
#include <map>
#include "Sector.h"
#include "Observer.h"
#include "WorldFile.h"
#include "Entity.h"

class World;

class WorldAnchor
{
private:
	WorldAnchor() : position(0.0f,0.0f), radius(100)
	{
	}

	~WorldAnchor()
	{
	}
public:
	Vector2 position;
	float radius;

	friend class World;
};


class World : public Observed, public Observer
{
private:
	WorldFile* zFile;

	Sector*** zSectors;

	std::vector <Entity*> zEntities;

	unsigned int zNrOfSectorsWidth;
	unsigned int zNrOfSectorsHeight;

	// Anchors
	std::set< WorldAnchor* > zAnchors;

public:
	World( Observer* observer, const std::string& fileName="" ) throw(const char*);
	World( Observer* observer, unsigned int nrOfSectorWidth, unsigned int nrOfSectorHeight);
	virtual ~World();

	// Save World To File
	void SaveFile();
	void SaveFileAs( const std::string& fileName );

	// Load All Sectors
	void LoadAllSectors();

	// Modify Height Functions
	void ModifyHeightAt( unsigned int x, unsigned int y, float value );
	float GetHeightAt( unsigned int x, unsigned int y );
	void SetHeightAt( unsigned int x, unsigned int y, float value );

	// Modify Blend Functions
	void ModifyBlendingAt( float x, float y, const Vector4& val );
	Vector4 GetBlendingAt( float x, float y );
	void SetBlendingAt( float x, float y, const Vector4& val );

	// Entity Functions
	bool CreateEntity(Vector3 pos, ENTITYTYPE entityType, std::string filePath);
	void RemoveEntity(Entity* entity);

	// Inline Functions
	unsigned int GetNumSectorsWidth() const;
	unsigned int GetNumSectorsHeight() const;

	// Sector Functions
	Sector* GetSector( unsigned int x, unsigned int y ) throw(const char*);
	Sector* GetSectorAtWorldPos( const Vector2& worldPos );
	Vector2 WorldPosToSector( const Vector2& worldPos );
	bool IsSectorLoaded( unsigned int x, unsigned int y ) const;
	void SetSectorTexture( unsigned int x, unsigned int y, const std::string& texture, unsigned int index );
	const char* const GetSectorTexture( unsigned int x, unsigned int y, unsigned int index );

	// Anchors
	WorldAnchor* CreateAnchor();
	void DeleteAnchor( WorldAnchor*& anchor );

	// Logic
	void Update();

	// Data Access
	unsigned int GetEntitiesInCircle( const Vector2& center, float radius, std::vector<Entity*>& out) const;
	unsigned int GetSectorsInCicle( const Vector2& center, float radius, std::set<Vector2>& out ) const;
	unsigned int GetHeightNodesInCircle( const Vector2& center, float radius, std::vector<Vector2>& out ) const;
	unsigned int GetTextureNodesInCircle( const Vector2& center, float radius, std::set<Vector2>& out ) const;
	unsigned int GetLoadedSectors( std::set<Vector2>& out ) const;

protected:
	// Engine Events
	virtual void onEvent( Event* e );
	
};