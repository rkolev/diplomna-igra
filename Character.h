#pragma once

#include "MovingObject.h"

//This class serves as a base for all other character objects in the game (NPCs, Players and PlayerControllers)
//Implements common functionality that is later used by all the aforementioned objects
class Character : public MovingObject
{
public: //methods
	//Default constructor TODO: proper initialization or make it private
	//do we want to be able to create gameobjects this way?
	Character() {}

	//Constructor with values
	Character(CL_GraphicContext* gc, CL_ResourceManager& resources, s32 posX, s32 posY, s8* objectType);
	
	//Default destructor
	~Character() {}

	//Returns the current state of the object as a sequence of flags
	//Relevant bits are 4th (Attacking), 5th (Jumping), 6th (Moving), 7th (Dead)
	//and 8th (Direction - 1 if Right, 0 if Left)
	unsigned short GetCurrentState();

protected: //methods
	//Updates the current object's state according to changes to game state since the last frame
	virtual void Update(s32 timeElapsedMs) override;		//TODO: might not need to be a virtual

	//Selects the appropriate sprite to draw, depending on the object's current state
	virtual void UpdateSprite() override;					//TODO: might not need to be a virtual

	//Handles the object's jump and fall states
	//Ensures that a character's jump takes them up to a maximum vertical distance
	//then starts the falling process
	virtual void HandleJumpFallState(s32 timeElapsedMs);	//TODO: might not need to be a virtual

	//Loads object data from settings xmls
	virtual void Initialize(XmlParser& parser, CL_ResourceManager& resources) override;

	//Adjusts the object's current Y speed, based on the object's current state
	virtual void AdjustYSpeed() override;

	//Starts the character's jump process
	//Ensures that a new jump doesn't start before the previous one is finished
	//or while the object is in the air (e.g. just falling from an edge)
	virtual void StartJump();

	//Resolves detected collisions
	virtual void ResolveCollisions(bool contacts[3], s32 nextMoveX, s32 nextMoveY) override;

protected: //members
	CL_Sprite m_jumpSprite;
	CL_Sprite m_fallSprite; 
	//TODO: add hurt and die sprites
};

