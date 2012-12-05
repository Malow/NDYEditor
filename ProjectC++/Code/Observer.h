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
	virtual void onEvent( Event* e ) {};

	friend class Observed;
};


class Observed
{
	std::set<Observer*> _observers;
public:
	inline void addObserver( Observer* observer ) { if ( observer ) _observers.insert(observer); }
	inline void removeObserver( Observer* observer ) { _observers.erase(observer); }
protected:
	inline void notifyObservers( Event* e ) { for( std::set<Observer*>::iterator i=_observers.begin(); i != _observers.end(); ++i ) { (*i)->onEvent(e); } }
};

#endif // _OBSERVER_HPP_