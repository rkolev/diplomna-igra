#pragma once
#include "Character.h"
#include "Behavior.h"

class NPC :	public Character

{
public: //methods
	//Default constructor TODO: proper initialization or make it private
	//do we want to be able to create gameobjects this way?
	NPC() {}

	//Constructor with values
	NPC(CL_GraphicContext *gc, CL_ResourceManager &resources, s32 posX, s32 posY, s8* objectType);

	//Default destructor
	~NPC() {}

protected: //methods
	//Loads object data from settings xmls
	void Initialize(XmlParser& parser, CL_ResourceManager& resources) override;

	//Updates the current object's state according to changes to game state since the last frame
	void Update(s32 timeElapsedMs) override;

	//Resolves detected collisions
	void ResolveCollisions(bool contacts[3], s32 nextMoveX, s32 nextMoveY) override;

	//Starts the character's jump process
	//Ensures that a new jump doesn't start before the previous one is finished
	//or while the object is in the air (e.g. just falling from an edge)
	void StartJump() override;

private: //methods
	//Checks for player objects in the NPC's vicinity
	void EvaluateSurroundings();

	//Determines the behaviour of the NPC, depending on its surroundings
	NpcBehavior ChooseBehavior();

	//Determines the action the NPC will take, depending on its current behaviour
	BasicAction DetermineAction();

	//TODO: do we need separate shoot methods?
	//Shoot a forward travelling missile (unaffected by gravity)
	void ShootMissile();

	//Shoot an arrow that travels in an arc (affected by gravity)
	void ShootArrow();

	//Initiates the object's shoot process
	void Shoot(AttackType projectile);

	//Executes the pre-selected action
	void TakeAction(BasicAction action, s32 timeElapsedMs);

private: //members
	std::map<NpcBehavior, s32> m_behaviorValues;
	//TODO: don't these lists need to be pointers?
	GameObjects m_nearbyObjects;
	GameObjects m_nearbyPlayers;
	GameObject* m_target;
	//TODO: why are these here?
	float m_speedX;
	float m_speedY;
	//TODO: don't parent classes contain this value?
	s32 m_timeSinceLastAttack;
};
