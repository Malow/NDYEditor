#pragma once

#include "Sector.h"
#include "Observer.h"
#include "WorldFile.h"
#include "Entity.h"
#include <string>


class World : public Observed, public Observer
{
private:
	WorldFile* zFile;

	Sector*** zSectors;

	std::vector <Entity*> zEntities;

	unsigned int zNrOfSectorsWidth;
	unsigned int zNrOfSectorsHeight;
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
	Sector* GetSectorAtWorldPos( const Vector2& pos ) throw(const char*);
	bool IsSectorLoaded( unsigned int x, unsigned int y ) const;

	// Data Access
	unsigned int GetEntitiesInCircle( const Vector2& center, float radius, std::vector<Entity*>& out) const;
	unsigned int GetSectorsInCicle( const Vector2& center, float radius, std::vector<Vector2>& out ) const;
	unsigned int GetHeightNodesInCircle( const Vector2& center, float radius, std::vector<Vector2>& out ) const;
	unsigned int GetTextureNodesInCircle( const Vector2& center, float radius, std::vector<Vector2>& out ) const;

protected:
	// Engine Events
	virtual void onEvent( Event* e );
	
};