#pragma once

#include <vector>

class Action;
class ActionGroup;
class GameEngine;


class Action
{
public:
	Action() {};
	virtual ~Action() {};

protected:
	virtual void Execute() = 0;
	virtual void Undo() = 0;

	friend GameEngine;
	friend ActionGroup;
};

class ActionGroup : public Action
{
public:
	std::vector< Action* > zActions;

	ActionGroup()
	{
	}

	virtual ~ActionGroup()
	{
		for( auto i = zActions.begin(); i != zActions.end(); ++i )
		{
			delete (*i);
		}
		zActions.clear();
	}

	virtual void Execute()
	{
		for( auto i = zActions.begin(); i != zActions.end(); ++i )
		{
			(*i)->Execute();
		}
	}

	virtual void Undo()
	{
		for( auto i = zActions.rbegin(); i != zActions.rend(); ++i )
		{
			(*i)->Undo();
		}
	}
};