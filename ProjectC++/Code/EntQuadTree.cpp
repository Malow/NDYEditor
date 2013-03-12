#include "EntQuadTree.h"
#include "Entity.h"
#include <fstream>
#include <string>

// The minimum node size, nodes will not divide if theire size is equal
const static float FMINIMUMNODESIZE = 16;

// Amount of entities required before the node divides
const static size_t DIVISIONCOUNT = 16;


EntQuadTree::Node::Node(const Rect& rect) : 
	zRect(rect)
{
	zChildNodes[0] = 0;
	zChildNodes[1] = 0;
	zChildNodes[2] = 0;
	zChildNodes[3] = 0;
}

EntQuadTree::Node::~Node()
{
	if ( zChildNodes[0] ) delete zChildNodes[0];
	if ( zChildNodes[1] ) delete zChildNodes[1];
	if ( zChildNodes[2] ) delete zChildNodes[2];
	if ( zChildNodes[3] ) delete zChildNodes[3];
}

void EntQuadTree::Node::Insert(Entity* elem, const Vector2& pos)
{
	// Quad Center Position
	Vector2 nodeCenter(zRect.topLeft + zRect.size * 0.5f);

	// Find Child Node
	unsigned int childNodeIndex = ( pos.x >= nodeCenter.x ) + ( pos.y >= nodeCenter.y ) * 2;
	
	// Insert To Child Node
	if ( zChildNodes[childNodeIndex] )
	{
		zChildNodes[childNodeIndex]->Insert(elem, pos);
	}
	else
	{
		// Subdivision
		if ( zElements.size() >= DIVISIONCOUNT-1 && zRect.size.x > FMINIMUMNODESIZE )
		{
			for( auto i = zElements.cbegin(); i != zElements.cend(); ++i )
			{
				// New Node
				unsigned int newNodeIndex = ( (*i)->GetPosition().x >= nodeCenter.x ) + ( (*i)->GetPosition().z >= nodeCenter.y ) * 2;
				
				// Create Child Node
				if ( !zChildNodes[newNodeIndex] )
				{
					// Child Node Rect
					Rect childRect;
					childRect.topLeft.x = zRect.topLeft.x + (float)(newNodeIndex%2) * zRect.size.x * 0.5f;
					childRect.topLeft.y = zRect.topLeft.y + (float)(newNodeIndex>1) * zRect.size.y * 0.5f;
					childRect.size.x = zRect.size.x * 0.5f;
					childRect.size.y = zRect.size.y * 0.5f;

					zChildNodes[newNodeIndex] = new Node(childRect);
				}

				// Insert To Child
				zChildNodes[newNodeIndex]->Insert(*i, (*i)->GetPosition().GetXZ());
			}
			zElements.clear();

			// Create Child Node
			if ( !zChildNodes[childNodeIndex] )
			{
				// Child Node Rect
				Rect childRect;
				childRect.topLeft.x = zRect.topLeft.x + (float)(childNodeIndex%2) * zRect.size.x * 0.5f;
				childRect.topLeft.y = zRect.topLeft.y + (float)(childNodeIndex>1) * zRect.size.y * 0.5f;
				childRect.size.x = zRect.size.x * 0.5f;
				childRect.size.y = zRect.size.y * 0.5f;

				zChildNodes[childNodeIndex] = new Node(childRect);
			}

			// Insert To Children
			zChildNodes[childNodeIndex]->Insert(elem, pos);
		}
		else
		{
			zElements.insert(elem);
		}
	}
}

bool EntQuadTree::Node::Erase(Entity* elem, const Vector2& pos)
{
	// Quad Center Position
	Vector2 nodeCenter(zRect.topLeft + zRect.size * 0.5f);

	// Find Child Node Index
	unsigned int childNodeIndex = ( pos.x >= nodeCenter.x ) + ( pos.y >= nodeCenter.y ) * 2;

	// Find Child
	if ( zChildNodes[childNodeIndex] )
	{
		if ( zChildNodes[childNodeIndex]->Erase(elem, pos) )
		{
			if ( zChildNodes[childNodeIndex]->CalcNumEntities() == 0 )
			{
				delete zChildNodes[childNodeIndex];
				zChildNodes[childNodeIndex] = 0;
			}

			if ( CalcNumEntities() < DIVISIONCOUNT )
			{
				for( unsigned int x=0; x<4; ++x )
				{
					if ( zChildNodes[x] ) 
					{
						zChildNodes[x]->Transfer(this);
						delete zChildNodes[x];
						zChildNodes[x] = 0;
					}
				}
			}

			return true;
		}
	}

	return zElements.erase(elem) > 0;
}

void EntQuadTree::Node::SetNode(unsigned int index, Node* node)
{
	if ( zChildNodes[index] ) 
	{ 
		delete zChildNodes[index];
	}

	zChildNodes[index] = node;
}

size_t EntQuadTree::Node::CircleScan(std::set<Entity*>& ents, const Circle& circle, unsigned int entType) const
{
	// Counter
	size_t entFoundCounter = 0;

	// My entities
	for( auto i = zElements.cbegin(); i != zElements.cend(); ++i )
	{
		if ( !entType || (*i)->GetType() == entType )
		{
			if ( circle.IsInside((*i)->GetPosition().GetXZ()) )
			{
				ents.insert(*i);
				entFoundCounter++;
			}
		}
	}

	// Children node entities
	for( unsigned int x=0; x<4; ++x )
	{
		if ( zChildNodes[x] ) 
		{
			if ( DoesIntersect( zChildNodes[x]->GetRect(), circle ) )
			{
				entFoundCounter += zChildNodes[x]->CircleScan(ents, circle, entType);
			}
		}
	}

	// Return number of additions
	return entFoundCounter;
}

size_t EntQuadTree::Node::RectangleScan(std::set<Entity*>& ents, const Rect& rect, unsigned int entType) const
{
	// Counter
	size_t entFoundCounter = 0;

	// My entities
	for( auto i = zElements.cbegin(); i != zElements.cend(); ++i )
	{
		if ( !entType || (*i)->GetType() == entType )
		{
			if ( rect.IsInside((*i)->GetPosition().GetXZ()) )
			{
				ents.insert(*i);
				entFoundCounter++;
			}
		}
	}

	// Children node entities
	for( unsigned int x=0; x<4; ++x )
	{
		if ( zChildNodes[x] ) 
		{
			if ( DoesIntersect( zChildNodes[x]->GetRect(), rect ) )
			{
				entFoundCounter += zChildNodes[x]->RectangleScan(ents, rect, entType);
			}
		}
	}

	// Return number of additions
	return entFoundCounter;
}

size_t EntQuadTree::Node::FlatScan(std::set<Entity*>& ents, unsigned int entType) const
{
	// Counter
	size_t entFoundCounter = 0;

	// Add Mine
	if ( entType )
	{
		for( auto i = zElements.cbegin(); i != zElements.cend(); ++i )
		{
			if ( !entType || (*i)->GetType() == entType )
			{
				ents.insert(*i);
				entFoundCounter++;
			}
		}
	}
	else
	{
		ents.insert(zElements.cbegin(), zElements.cend());
		entFoundCounter = ents.size();
	}

	// Add Childrens
	if ( zChildNodes[0] ) entFoundCounter += zChildNodes[0]->FlatScan(ents, entType);
	if ( zChildNodes[1] ) entFoundCounter += zChildNodes[1]->FlatScan(ents, entType);
	if ( zChildNodes[2] ) entFoundCounter += zChildNodes[2]->FlatScan(ents, entType);
	if ( zChildNodes[3] ) entFoundCounter += zChildNodes[3]->FlatScan(ents, entType);

	return entFoundCounter;
}

size_t EntQuadTree::Node::Transfer(Node* node)
{
	size_t amount = zElements.size();
	node->zElements.insert(zElements.cbegin(), zElements.cend());
	zElements.clear();

	// Child Nodes
	for( unsigned int x=0; x<4; ++x )
	{
		if ( zChildNodes[x] ) amount += zChildNodes[x]->Transfer(node);
	}

	return amount;
}

size_t EntQuadTree::Node::CalcNumEntities(unsigned int entType) const
{
	size_t entFoundCounter = 0;

	if ( entType )
	{
		// Count My Elements
		for( auto i = zElements.cbegin(); i != zElements.cend(); ++i )
		{
			if ( (*i)->GetType() == entType )
			{
				entFoundCounter++;
			}
		}
	}
	else
	{
		entFoundCounter = zElements.size();
	}

	// Add Childrens
	for( unsigned int x=0; x<4; ++x )
	{
		if ( zChildNodes[x] )
		{
			entFoundCounter += zChildNodes[x]->CalcNumEntities(entType);
		}
	}

	return entFoundCounter;
}

EntQuadTree::EntQuadTree() : 
	zRoot(0)
{

}

EntQuadTree::~EntQuadTree()
{
	if ( zRoot ) 
	{
		// Iterate and remove observers
		std::set<Entity*> ents;
		if ( zRoot->FlatScan(ents, 0) )
		{
			for( auto i = ents.cbegin(); i != ents.cend(); ++i )
			{
				(*i)->RemoveObserver(this);
			}
		}

		delete zRoot;
	}
}

size_t EntQuadTree::FlatScan(std::set<Entity*>& ents, unsigned int entType) const
{
	if ( zRoot )
	{
		return zRoot->FlatScan(ents, entType);
	}

	return 0;
}

size_t EntQuadTree::CircleScan(std::set<Entity*>& ents, const Circle& circle, unsigned int entType) const
{
	if ( zRoot )
	{
		return zRoot->CircleScan(ents, circle, entType);
	}

	return 0;
}

size_t EntQuadTree::RectangleScan(std::set<Entity*>& ents, const Rect& rect, unsigned int entType) const
{
	if ( zRoot )
	{
		return zRoot->RectangleScan(ents, rect, entType);
	}

	return 0;
}

void EntQuadTree::Insert(Entity* ent, const Vector2& pos)
{
	if ( ent )
	{
		if ( !zRoot )
		{
			// Find Minimum
			Rect rootRect;
			rootRect.topLeft.x = floorf(pos.x / FMINIMUMNODESIZE) * FMINIMUMNODESIZE;
			rootRect.topLeft.y = floorf(pos.y / FMINIMUMNODESIZE) * FMINIMUMNODESIZE;
			rootRect.size.x = FMINIMUMNODESIZE;
			rootRect.size.y = FMINIMUMNODESIZE;

			// Create Root
			zRoot = new Node(rootRect);
		}
		else if ( !zRoot->GetRect().IsInside(pos) )
		{
			// Calc Root Level
			float level = zRoot->GetRect().size.x / FMINIMUMNODESIZE;
			float newSize = FMINIMUMNODESIZE * level * 2.0f;

			// Expand QuadTree
			Rect rootRect;
			rootRect.topLeft.x = floorf(zRoot->GetRect().topLeft.x / newSize) * newSize;
			rootRect.topLeft.y = floorf(zRoot->GetRect().topLeft.y / newSize) * newSize;
			rootRect.size.x = newSize;
			rootRect.size.y = newSize;

			// New Root
			Node* newRoot = new Node(rootRect);

			// Transfer Nodes
			if ( zRoot->CalcNumEntities() < DIVISIONCOUNT )
			{
				zRoot->Transfer(newRoot);
				delete zRoot;
			}
			else
			{
				// Old Root Index
				unsigned int oldRootIndex = ( zRoot->GetRect().topLeft.x != rootRect.topLeft.x ) + ( zRoot->GetRect().topLeft.y != rootRect.topLeft.y ) * 2;
				newRoot->SetNode(oldRootIndex, zRoot);
			}

			zRoot = newRoot;
		}

		zRoot->Insert(ent, pos);
	}
}

bool EntQuadTree::Erase(Entity* ent, const Vector2& pos)
{
	if ( ent )
	{
		if ( zRoot )
		{
			if ( zRoot->GetRect().IsInside(pos) )
			{
				if ( zRoot->Erase(ent, pos) )
				{
					while( zRoot->GetNumNodes() == 1 && zRoot->GetNumElements() == 0 )
					{
						Node* newRoot = 0;

						for( unsigned int x=0; x<4 && !newRoot; ++x )
						{
							if ( zRoot->GetNode(x) ) newRoot = zRoot->GetNode(x);
						}

						delete zRoot;
						zRoot = newRoot;
					}

					if ( zRoot->GetNumElements() == 0 && zRoot->GetNumNodes() == 0 )
					{
						delete zRoot;
						zRoot = 0;
					}

					return true;
				}
			}
		}
	}

	return false;
}

void EntQuadTree::Insert(Entity* ent)
{
	if ( ent ) 
	{
		Insert(ent, ent->GetPosition().GetXZ());
		ent->AddObserver(this);
	}
}

bool EntQuadTree::Erase(Entity* ent)
{
	if ( ent )
	{
		if ( Erase(ent, ent->GetPosition().GetXZ()) )
		{
			ent->RemoveObserver(this);
			return true;
		}
	}

	return false;
}

void EntQuadTree::OnEvent(Event* e)
{
	if ( zRoot )
	{
		if ( EntityMovingEvent* EME = dynamic_cast<EntityMovingEvent*>(e) )
		{
			Erase(EME->zEntity, EME->zEntity->GetPosition().GetXZ());
			Insert(EME->zEntity, EME->zNewPos.GetXZ());
		}
		else if ( EntityDeletedEvent* EDE = dynamic_cast<EntityDeletedEvent*>(e) )
		{
			Erase(EDE->entity);
		}
	}
}

void EntQuadTree::BranchPrint(EntQuadTree::Node* node, std::ofstream& file, unsigned int level)
{
	std::string pad;
	pad.resize(level);
	for( unsigned int x=0; x<pad.size(); ++x )
	{
		pad[x] = 32;
	}

	if ( node )
	{
		file << pad << "Pos: (" << node->GetRect().topLeft.x << ", " << node->GetRect().topLeft.y << ")" << std::endl;
		file << pad << "Size: (" << node->GetRect().size.x << ", " << node->GetRect().size.y << ")" << std::endl;

		// Print Num Elements
		file << pad << "NumElements: " << node->GetNumElements() << std::endl;

		for( unsigned int x=0; x<4; ++x )
		{
			BranchPrint(node->GetNode(x), file, level+1);
		}
	}
	else
	{
		file << pad << "Empty" << std::endl;
	}
}

void EntQuadTree::PrintTree()
{
	std::ofstream file("Tree.txt", std::ios::app);

	if ( file.good() )
	{
		BranchPrint(zRoot, file, 0);
	}

	file.close();
}