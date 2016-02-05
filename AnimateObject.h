#pragma once

#include "StaticObject.h"

//This class encompasses objects, that are not supposed to move, but still have animations
//e.g. idle animation or attacking animation, like traps or trees
class AnimateObject : public StaticObject
{
public: //methods
	//Default constructor TODO: proper initialization or make it private
	//do we want to be able to create gameobjects this way?
	AnimateObject() {}

	//Constructor with values
	AnimateObject(CL_GraphicContext* gc, CL_ResourceManager& resources, s32 posX, s32 posY, s8* objectType);

	//Default destructor
	~AnimateObject() {}

	//Assigns a list of objects which the current object will consider enemies (for attacking purposes)
	void SetEnemyList(GameObjects* enemies) { m_enemies = enemies; }

	//Assigns a list of objects which the current object will consider environment (for collision purposes)
	void SetEnvList(GameObjects* environment) { m_environment = environment; }

	//Assigns a list of objects which the current object will consider destructibles
	//These are different from the enemy list (as enemies are character objects)
	//Used for inflicting environment damage (breaking tiles, destroying traps, etc)
	void SetDestructiblesList(GameObjects* destructibles) { m_destructibles = destructibles; }

	//Connects @drawSignal and @updateSignal to the object's drawSlot and updateSlot
	void ConnectSlots(CL_Signal_v0& drawSignal, CL_Signal_v1<s32>& updateSignal);

protected: //methods
	//Calls StaticObject's CalculateBounds() and calculates object's attack bounds
	void CalculateBounds() override;

	//Starts the object's attack process /melee attacks/
	void StartAttack();

	//Starts the object's attack process /ranged attacks - shoot at @target point/
	void StartAttack(CL_Point target);

	//Controls the objects Attacking state.
	//Ensures that a new attack cannot startwhile one is already in progress (the attack animation is not finised)
	//Ensures that a new attack does not start before the object's attack cooldown has expired.
	//Ensures that a hit check is performed only once per attack animation.
	//TODO: break this method apart to reduce complexity?
	void HandleAttackState(s32 timeElapsedMs);

	//Iterates through the enemies and destructibles list, to check if a hit is successfully scored during attack.
	//If a hit has occurred, inflicts the current object's attack power as damage to the hit object.
	void PerformHitCheck();
	
	//Loads object data from settings xmls
	virtual void Initialize(XmlParser& parser, CL_ResourceManager& resources) override;

	//Updates the current object's state according to changes to game state since the last frame
	virtual void Update(s32 timeElapsedMs);

	//Selects the appropriate sprite to draw, depending on the object's current state
	virtual void UpdateSprite();

	//Adjusts the angle, at which the sprite should be drawn, depending on the object's current direction
	//TODO: rename
	virtual void SetSpriteDirection();

protected: //members
	CL_Slot m_updateSlot;
	CL_Sprite m_attackSprite;
	Direction m_spawnDir;
	GameObjects* m_enemies;
	GameObjects* m_environment;
	GameObjects* m_destructibles;
	AttackType m_attackType;
	s32 m_attackBoundsOffsets[4];
	CL_Rect m_attackBounds; //used only for melee attacks
	CL_Signal_v0* m_drawSignal;
	CL_Signal_v1<s32>* m_updateSignal;
};