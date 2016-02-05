#include "UtilsBehavior.h"

namespace UtilsBehavior {

	GameObjects DetectEnemies(GameObject& npc, GameObjects& enemies, GameObjects& environment)
	{
		GameObjects nearbyPlayers;

		GameObjects::iterator it = enemies.begin();
		for ( ; it != enemies.end(); ++it) {
			if ((*it)->GetFlag(eDead)) {
				continue;
			}
			
			//check if the object is in sight range; ignore LoS if it is
			if (IsNearby(npc, **it)	&& (HasLOS(npc, (*it)->GetPosition(), environment) || npc.GetFlag(eSpectral))) {				
				nearbyPlayers.push_back(*it);
			}
		}
		return nearbyPlayers;
	}

	GameObject* GetClosestCharacter(GameObject& npc, GameObjects& characters)
	{
		GameObject* closestCharacter = nullptr;
		if (characters.size() > 1) {
			for (GameObjects::iterator charObj = characters.begin(); charObj != characters.end(); ++charObj) {
				if (closestCharacter == nullptr) {
					//1st iteration only
					closestCharacter = *charObj;
					continue;
				} else {
					//TODO: can cut down on method invocations
					if (npc.GetPosition().x - closestCharacter->GetPosition().x > npc.GetPosition().x - (*charObj)->GetPosition().x) {
						closestCharacter = *charObj;
					}
				}
			}
		} else {
			//if there is only 1 element, it's the closest
			closestCharacter = *characters.begin();
		}

		return closestCharacter;
	}

	bool IsNearby(GameObject& npc, GameObject& object)
	{
		s32 distance = Utility::GetDistance(npc.GetPosition(), object.GetPosition());
		return distance <= npc.GetStat(eSightRadius);
	}

	bool HasLOS(GameObject& npc, CL_Point charPos, GameObjects& obstacles)
	{
		Utility::Line sight;
		sight.p1 = charPos;
		sight.p2 = npc.GetPosition();

		for (GameObjects::iterator iter = obstacles.begin(); iter != obstacles.end(); ++iter)
		{
			if (npc.GetId() == (*iter)->GetId()){
				//Skip comparing npc to itself
				continue;
			}

			if (Utility::LineIntesectsRectangle(sight, (*iter)->GetBounds())) {
				return false;
			}
		}
		return true;
	}

	bool CanHit(GameObject& npc, GameObject& target)
	{
		//TODO: save attack range stat as local var to reduce method invocations
		s32 npcX = npc.GetDirection() == eRight
			? npc.GetPosition().x + npc.GetStat(eAttackRange)
			: npc.GetPosition().x - npc.GetStat(eAttackRange);

		return Utility::ContainsPoint(target.GetBounds(), npcX, npc.GetPosition().y);
	}

	bool IsHigher(GameObject& npc, GameObject& target)
	{
		return npc.GetPosition().y > target.GetPosition().y;
	}

	bool CanMove(GameObject& npc, GameObjects& environment)
	{
		s32 moveCheckX, moveCheckY;

		//TODO: save npc bounds as local var to reduce method invocations
		if (npc.GetDirection() == eRight) {
			moveCheckX = npc.GetBounds().right + 3;
		} else {
			moveCheckX = npc.GetBounds().left - 3;
		}
		moveCheckY = npc.GetBounds().bottom + 3;

		// we need to iterate through all the tiles on the screen, because their bounds might not start in the
		// sight radius, therefore the objects you can move on might not be in the nearByObjects list
		for (GameObjects::iterator tileObject = environment.begin(); tileObject != environment.end(); ++tileObject)
		{
			if (Utility::ContainsPoint((*tileObject)->GetBounds(), moveCheckX, moveCheckY)) {
				return true;
			}
		}
		return false;
	}

	bool IsFacing(GameObject& npc, GameObject& target)
	{
		// Check if object is facing a specific point
		if ((npc.GetPosition().x < target.GetPosition().x) && (npc.GetDirection() == eRight)) return true;
		if ((npc.GetPosition().x > target.GetPosition().x) && (npc.GetDirection() == eRight)) return true;
		return false;
	}

	//TODO: think about a fix to IsFacing + FaceTarget methods interaction
	void FaceTarget(GameObject& npc, GameObject& target, bool faceAway)
	{
		//TODO: switch case on a boolean, really?
		switch (faceAway) {
		case true:
		{
			if (UtilsBehavior::IsFacing(npc, target)) {
				//TODO: ok what?
				npc.SetDirection(npc.GetDirection() == eLeft ? eRight : eLeft);
			}
		}
		break;
		case false:
		{
			if (!UtilsBehavior::IsFacing(npc, target)) {
				//TODO: see above
				npc.SetDirection(npc.GetDirection() == eLeft ? eRight : eLeft);
			}
		}
		break;
		}
	}

	void DetermineSpeeds(GameObject& npc, CL_Point& destination)
	{
		// Used by flying objects to determine the x and y speeds needed to reach a point (destination)
		float xDistance, yDistance, distance;
		xDistance = static_cast<float>(destination.x - npc.GetPosition().x);
		yDistance = static_cast<float>(destination.y - npc.GetPosition().y);
		distance = sqrt(pow(xDistance, 2) + pow(yDistance, 2));
		//TODO: unnecessary double method invocation
		npc.SetSpeed(eXSpeed, npc.GetSpeed(eCompositeSpeed) * (xDistance / distance));
		npc.SetSpeed(eYSpeed, npc.GetSpeed(eCompositeSpeed) * (yDistance / distance));
	}
}