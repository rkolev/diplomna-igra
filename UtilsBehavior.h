#pragma once
#include "Utility.h"

namespace UtilsBehavior
{	
	//GameObjList DetectTerrain(); //TODO: implement?

	//Detect enemies in a given radius
	//TODO: what's the radius?
	GameObjects DetectEnemies(GameObject& npc, GameObjects& enemies, GameObjects& environment);

	//Get the closest character
	GameObject* GetClosestCharacter(GameObject& npc, GameObjects& characters);

	//Turn the character to face towards or away from the target
	void FaceTarget(GameObject& npc, GameObject& target, bool faceAway);

	//Determine the speed required to reach destination
	//TODO: is this description correct?
	void DetermineSpeeds(GameObject& npc, CL_Point& destination);

	//Check if a given object is withing a given radius
	//TODO: is this description correct?
	//If yes, how is the radius determined?
	bool IsNearby(GameObject& npc, GameObject& object);

	//Check if @npc has vision of @charPos point
	bool HasLOS(GameObject& npc, CL_Point charPos, GameObjects& obstacles);

	//Check if @npc can hit @target
	bool CanHit(GameObject& npc, GameObject& target);

	//Check if @npc can move
	//TODO: better description
	bool CanMove(GameObject& npc, GameObjects& environment);

	//Check if @npc is at a higher position than @target
	bool IsHigher(GameObject& npc, GameObject& target);

	//Check if @npc is facing @target
	bool IsFacing(GameObject& npc, GameObject& target);
}; //namespace UtilsBehavior

