#pragma once
#include "UtilsBehavior.h"

#define TIMEOUT 250

namespace Behavior
{
	//TODO: document (when you figure the logic out)
	BasicAction BehaviorControls(NpcBehavior npcBehavior, GameObject& npc, GameObject& target, GameObjects& environment);
	BasicAction AggressiveBehavior(GameObject& npc, GameObject& target, GameObjects& environment);
	BasicAction CowardlyBehavior(GameObject& npc, GameObject& target, GameObjects& environment);
	BasicAction GreedyBehavior(GameObject& npc, GameObject& target, GameObjects& environment);
	BasicAction WanderingBehavior(GameObject& npc, GameObject& target, GameObjects& environment);
	BasicAction MiscellaneousBehavior(GameObject& npc, GameObject& target, GameObjects& environment);
}; //namespace Behavior

