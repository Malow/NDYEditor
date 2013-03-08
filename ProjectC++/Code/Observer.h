#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

#include <set>

class Event;
class Observer;
class Observed;


class Event
{
public:
	virtual ~Event() {}
};

class Observer
{
protected:
	virtual void OnEvent( Event* ) {};
	virtual ~Observer() {};

	friend class Observed;
};

class Observed
{
	std::set<Observer*> zObservers;
	bool zInUse;

	std::set<Observer*> zObserversToInsert;
	std::set<Observer*> zObserversToErase;

public:
	inline void AddObserver( Observer* observer ) 
	{ 
		if ( observer ) 
		{
			if ( zInUse )
			{
				zObserversToInsert.insert(observer);
			}
			else
			{
				zObservers.insert(observer); 
			}
		}
	}

	inline void RemoveObserver( Observer* observer ) 
	{
		if ( observer )
		{
			if ( zInUse )
			{
				zObserversToErase.insert(observer); 
			}
			else
			{
				zObservers.erase(observer);
			}
		}
	}

protected:
	Observed( Observer* default=0 ) : 
		zInUse(false)
	{
		AddObserver(default); 
	}

	virtual ~Observed() {};

	inline void NotifyObservers( Event* e ) 
	{ 
		if ( zObservers.size() )
		{
			zInUse = true;

			// Notify Observers
			for( std::set<Observer*>::iterator i = zObservers.begin(); i != zObservers.end(); ++i ) 
			{ 
				(*i)->OnEvent(e);
			}

			// Insert New Observers
			for( std::set<Observer*>::iterator i = zObserversToInsert.begin(); i != zObserversToInsert.end(); ++i ) 
			{ 
				zObservers.insert(*i);
			}
			zObserversToInsert.clear();

			// Remove Observers
			for( std::set<Observer*>::iterator i = zObserversToErase.begin(); i != zObserversToErase.end(); ++i ) 
			{ 
				zObservers.erase(*i);
			}
			zObserversToErase.clear();

			zInUse = false;
		}
	}
};

#endif // _OBSERVER_HPP_