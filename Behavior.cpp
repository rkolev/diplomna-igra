#include "Behavior.h"

namespace Behavior
{

	BasicAction BehaviorControls(NpcBehavior behavior, GameObject& npc, GameObject& target, GameObjects& environment)
	{
		switch (behavior)
		{
		case eDesire:
		{
			return GreedyBehavior(npc, target, environment);
		}
		break;
		case eFear:
		{
			return CowardlyBehavior(npc, target, environment);
		}
		break;
		case eAggression:
		{
			return AggressiveBehavior(npc, target, environment);
		}
		break;
		case eWander:
		{
			return WanderingBehavior(npc, target, environment);
		}
		break;
		case eMisc:
		{
			return MiscellaneousBehavior(npc, target, environment);
		}
		break;
		default:
		{
			return eStop;
		}
		break;
		}
	}

	BasicAction AggressiveBehavior(GameObject& npc, GameObject& target, GameObjects& environment)
	{
		//TODO: there isn't really any point of else if you return in the if above
		if (UtilsBehavior::CanHit(npc, target))	{
			return eAttack;
		} else if (npc.GetFlag(eAttacking) && !UtilsBehavior::CanHit(npc, target)) {
			return eStop;
		} else if (!npc.GetFlag(eAffectedByGravity) || UtilsBehavior::CanMove(npc, environment)) {
			CL_Point destination = target.GetPosition();
			if (!UtilsBehavior::IsFacing(npc, target)) {
				UtilsBehavior::FaceTarget(npc, target, false);
			}				
				
			if (npc.GetDirection() == eLeft) {
				destination.x += npc.GetStat(eAttackRange);
			} else {
				destination.x -= npc.GetStat(eAttackRange);
			}				
			UtilsBehavior::DetermineSpeeds(npc, destination);				
			return eMove;
		}		
		return eStop;
	}

	BasicAction CowardlyBehavior(GameObject& npc, GameObject& target, GameObjects& environment)
	{
		if (!UtilsBehavior::IsFacing(npc, target)) {
			UtilsBehavior::FaceTarget(npc, target, false);
		}

		//TODO: there isn't really any point of else if you return in the if above
		if (npc.GetFlag(eCanShoot)
			//&& Utils::CalculateTrajectory(npc.GetPosition(), target.GetPosition(), npc.GetSpeed(COMPOSITE_SPEED), true) != 0.0f
			&& (npc.GetStat(eTimeSinceLastAttack) > npc.GetStat(eAttackCooldown))) {
			return eShoot;
		} else if (Utility::GetDistance(npc.GetPosition(), target.GetPosition()) < 2 * npc.GetStat(eAttackRange)) {
			return eFlee;
		}
		return eStop;
	}

	//TODO: do we really need two methods that return the same thing?
	//Or are they not implemented yet?
	BasicAction GreedyBehavior(GameObject& npc, GameObject& target, GameObjects& environment)
	{
		return eMove;
	}

	BasicAction WanderingBehavior(GameObject& npc, GameObject& target, GameObjects& environment)
	{
		return eMove;
	}

	BasicAction MiscellaneousBehavior(GameObject& npc, GameObject& target, GameObjects& environment)
	{		
		if (!UtilsBehavior::IsFacing(npc, target)) {
			UtilsBehavior::FaceTarget(npc, target, false);
		}

		//TODO: save stuff in local vars to avoid redundant method invocations
		if (Utility::GetDistance(npc.GetPosition(), target.GetPosition()) < npc.GetStat(eAttackRange)) {
			return eFlee;
		}

		//TODO: there isn't really any point of else if you return in the if above
		if (UtilsBehavior::CanHit(npc, target))	{
			return eAttack;
		} else if (abs(npc.GetPosition().y - target.GetPosition().y) < 30) {
			if (UtilsBehavior::CanMove(npc, environment)) {
				return eMove;
			} else if (Utility::CalculateTrajectory(npc.GetPosition(), target.GetPosition(), npc.GetSpeed(eCompositeSpeed), true) != 0.0f) {
				return eJump;
			}
			return eStop;
		} else if (Utility::CalculateTrajectory(npc.GetPosition(), target.GetPosition(), npc.GetSpeed(eCompositeSpeed), true) != 0.0f) {
			return eJump;
		} else {
			return eStop;
		}
	}
}