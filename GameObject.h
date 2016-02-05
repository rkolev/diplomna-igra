#pragma once
#include "Precomp.h"

//Abstract class, parent to all classes for game objects
//exists to give access to basic object information
//Used in interactions between objects.
class GameObject
{
public: //methods	
	//Default constructor TODO: proper initialization or make it private
	//do we want to be able to create gameobjects this way?
	GameObject();

	//Destructor
	virtual	~GameObject();

	//Get object's assigned id
	s32 GetId()	const;

	//Get object's current position
	virtual CL_Point const&	GetPosition() const	= 0;

	//Get object's collision bounds
	virtual CL_Rect  const&	GetBounds() const = 0;

	//Get the direction the object is currently facing as Direction enum/left or right/
	virtual Direction GetDirection() const = 0;

	//Get the value of the flag specified by @flagName
	virtual bool GetFlag(Flag flagName) const = 0;

	//Get the value of the stat specified by @statName
	virtual s32 GetStat(Stat statName) const = 0;

	//Get the value of the speed specified by @speedName
	virtual float GetSpeed(Speed speedName)	const = 0;

	//Set the position of the object
	virtual void SetPosition(CL_Point const& position) = 0;

	//Set the direction of the object
	virtual void SetDirection(Direction dir) = 0;

	//Set the value of the flag specified by @flagName
	virtual void SetFlag(Flag flagName, bool value) = 0;

	//Set the value of the stat specified by @statName
	virtual void SetStat(Stat statName, s32 value) = 0;

	//Set the value of the speed specified by @speedName
	virtual void SetSpeed(Speed speedName, float value)	= 0;

	//Lowers the life points of the object (if destructible) by the amount specified
	virtual void Hurt(s32 damage) = 0;
	
	//Equals operator override: compares object IDs of the two objects to see if they match
	bool operator==(GameObject const& other) const;

protected: //members	
	static s32 ms_idCounter;
	s32 m_objectId;
}; // class GameObject

//TODO: more appropriate container?
typedef std::list<GameObject *> GameObjects;