#pragma once

#include "AnimateObject.h"

//This class encompasses movable objects that are not categorized as characters
//e.g. elevators, projectiles, but not zombies, ghosts or player characters
class MovingObject : public AnimateObject
{
public: //methods
	//Default constructor TODO: proper initialization or make it private
	//do we want to be able to create gameobjects this way?
	MovingObject() {}

	//Constructor with values
	MovingObject(CL_GraphicContext* gc, CL_ResourceManager& resources, s32 posX, s32 posY, s8* objectType);

	//Default destructor
	~MovingObject() {}

	//Starts the object's movement process in the specified @dir
	void StartMoving(Direction dir);

	//Stops the object's movement process
	void StopMoving();
	
protected: //methods
	//Adjusts the angle, at which the sprite should be drawn, depending on the object's current direction
	//TODO: rename
	void SetSpriteDirection() override;

	//Checks for collisions with collidable objects
	//TODO: fucking complicated function, look into breaking it apart
	void HandleCollisions(s32 timeElapsedMs);

	//Calculates the object's movement offset in since last frame based on @speed
	s32 Move(float speed, s32 timeElapsedMs);

	//Loads object data from settings xmls
	virtual void Initialize(XmlParser& parser, CL_ResourceManager& resources) override;

	//Updates the current object's state according to changes to game state since the last frame
	virtual void Update(s32 timeElapsedMs) override;

	//Selects the appropriate sprite to draw, depending on the object's current state
	virtual void UpdateSprite() override;

	//Adjusts the object's current X speed, based on the object's current state
	virtual void AdjustXSpeed();

	//Adjusts the object's current Y speed, based on the object's current state
	virtual void AdjustYSpeed();

	//Resolves detected collisions
	virtual void ResolveCollisions(bool contacts[3], s32 nextMoveX, s32 nextMoveY);

protected: //members
	CL_Sprite m_moveSprite;
	CL_Point m_spawnPoint;
	CL_Point m_collisionPoints[8];
};