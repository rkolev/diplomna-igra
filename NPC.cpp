#include "NPC.h"

NPC::NPC(CL_GraphicContext *gc, CL_ResourceManager& resources, int posX, int posY, char* objectType)
	: Character(gc, resources, posX, posY, objectType)
{
	//ugly hack to fix the movement
	//TODO: fix movement and jump so they don't need this
	m_speedY = m_speeds[eYSpeed];
	m_speedX = m_speeds[eXSpeed]; 

	//TODO: set these dynamically
	if (posX > 600) {
		SetDirection(eLeft);
	}

	char ConfigFilePath[256] = "ObjectData/Characters/";
	strcat_s(ConfigFilePath, objectType);
	strcat_s(ConfigFilePath, ".xml");
	XmlParser parser(ConfigFilePath);

	NPC::Initialize(parser, resources);
}

void NPC::Initialize(XmlParser& parser, CL_ResourceManager& resources)
{
	Character::Initialize(parser, resources);
	m_behaviorValues[eAggression] = parser.ReadInt("/ObjectData/BehaviorValues/AGGRESSION");
	m_behaviorValues[eDesire] = parser.ReadInt("/ObjectData/BehaviorValues/DESIRE");
	m_behaviorValues[eFear] = parser.ReadInt("/ObjectData/BehaviorValues/FEAR");
	m_behaviorValues[eMisc] = parser.ReadInt("/ObjectData/BehaviorValues/MISC");
	m_behaviorValues[eWander] = parser.ReadInt("/ObjectData/BehaviorValues/WANDER");
}

void NPC::Update(int timeElapsedMs)
{
	EvaluateSurroundings();
	TakeAction(DetermineAction(), timeElapsedMs);
	Character::Update(timeElapsedMs);
}

void NPC::ResolveCollisions(bool contacts[3], int nextMoveX, int nextMoveY)
{
	//TODO: is this any different from the method in Character class?
	//if a top contact has occurred, we need to stop jumping and start falling
	if (contacts[0] && m_speeds[eYCurrentSpeed] < 0) {
		m_speeds[eYCurrentSpeed] = 0;
		m_stats[eJumpTime] = m_stats[eMaxJumpTime];
	}

	//if a contact has occurred, use the recalculated movement vector
	//and prevent any further movement in this frame by setting
	//the xSpeed and the ySpeed to 0
	if (contacts[1]) {
		m_position.y += nextMoveY;
		m_speeds[eYCurrentSpeed] = 0;
		m_flags[eFalling] = false;
		m_flags[eJumping] = false;
	}
	if (contacts[2]) {
		m_position.x += nextMoveX;
		m_speeds[eXCurrentSpeed] = 0;
	}
}

void NPC::EvaluateSurroundings()
{
	m_nearbyPlayers = UtilsBehavior::DetectEnemies(*this, *m_enemies, *m_environment);
}


NpcBehavior NPC::ChooseBehavior()
{
	std::pair<NpcBehavior, int> chosenBehavior;
	std::map<NpcBehavior, int>::iterator it = m_behaviorValues.begin();

	//TODO: add method to adjust weights to achieve unpredicted behavior
	for ( ; it != m_behaviorValues.end(); it++) {
		//make behavior unpredictable
		//Don't ever uncomment this
		//iterator->second = rand(); 

		if (it->second > chosenBehavior.second)	{
			chosenBehavior = std::pair<NpcBehavior, int>(it->first, it->second);
		}		
	}
	return chosenBehavior.first;
}

BasicAction NPC::DetermineAction()
{
	BasicAction takeAction;
	if (m_nearbyPlayers.size() > 0) {
		//get closest character in sight
		m_target = UtilsBehavior::GetClosestCharacter(*this, m_nearbyPlayers);
		takeAction = Behavior::BehaviorControls(ChooseBehavior(), *this, *m_target, *m_environment);		
		return takeAction;
	}
	return eStop;
}

void NPC::TakeAction(BasicAction action, int timeElapsedMs)
{
	//TODO: terrible code, consider using references
	switch (action)	{
	case eAttack: {
		StopMoving();
		StartAttack(); //TODO: might need to override
		break;
	}	
	case eMove: { //compiler mistook this for the Move() method..
		// reset speed to normal
		if (m_flags[eAffectedByGravity] && !m_flags[eJumping] && !m_flags[eFalling]) {
			m_speeds[eXSpeed] = m_speedX;
			m_speeds[eYSpeed] = m_speedY;
		}
		StartMoving(GetDirection());
		break;
	}	
	case eFlee: {
		// reset speed to normal
		if (m_flags[eAffectedByGravity] && !m_flags[eJumping] && !m_flags[eFalling]) {
			m_speeds[eXSpeed] = m_speedX;
			m_speeds[eYSpeed] = m_speedY;
		}
		//avoid colliding with the character
		//TODO: WHAT? This makes no sense (even before refactor)
		StartMoving(m_direction == eLeft ? eRight : eLeft);
		break;
	}	
	case eJump: {
		StartJump();
		break;
	}
	case eShoot: {
		Shoot(m_attackType);
		break;
	}
	default: {
		StopMoving();
		if (!m_flags[eAffectedByGravity]) {
			m_speeds[eXSpeed] = 0;
			m_speeds[eYSpeed] = 0;
		}
		break;
	}
	}
}

void NPC::StartJump()
{
	//TODO: terrible code, consider using references
	if (!m_flags[eJumping] && !m_flags[eFalling]) // Don't start another jump if already in the air
	{
		m_flags[eMoving] = true;
		m_flags[eJumping] = true;
		m_stats[eJumpTime] = 0;

		//CL_Point offset = GetDirection() == LEFT ? -stats[ATTACK_RANGE] : stats[ATTACK_RANGE];
		double launchAngle = Utility::CalculateTrajectory(GetPosition(), m_target->GetPosition(), m_speeds[eCompositeSpeed], true);
		m_speeds[eXSpeed] = abs(m_speeds[eCompositeSpeed] * sin(launchAngle));
		m_speeds[eYSpeed] = abs(m_speeds[eCompositeSpeed] * cos(launchAngle));
	}
}

//TODO: fix shoot direciton and, if possible amount of objects created
void NPC::ShootMissile()
{
	if (!m_flags[eAttacking])
	{
		//TODO: this isn't properly cleaned up. Also need to add it to Level's object lists
		//All in all we need a better way to create projectiles
		MovingObject* missile = new MovingObject(m_graphicContext, *m_resourceManager, m_position.x, m_position.y, "Missile");
		missile->ConnectSlots(*m_drawSignal, *m_updateSignal);
		missile->SetEnvList(m_environment);
		missile->SetDirection(m_direction);

		if (missile->GetDirection() == eLeft) {
			missile->SetSpeed(eXSpeed, -missile->GetSpeed(eXSpeed));
		}
		StartAttack();
		missile->StartMoving(m_direction);
		m_timeSinceLastAttack = CL_System::get_time();
	}
}

void NPC::ShootArrow()
{
	if (!m_flags[eAttacking])
	{
		//TODO: this isn't properly cleaned up.
		MovingObject* arrow = new MovingObject(m_graphicContext, *m_resourceManager, m_position.x, m_position.y, "Arrow");
		arrow->ConnectSlots(*m_drawSignal, *m_updateSignal);
		arrow->SetEnvList(m_environment);
		double launchAngle = Utility::CalculateTrajectory(arrow->GetPosition(), m_target->GetPosition(), arrow->GetSpeed(eCompositeSpeed), true);
		arrow->SetSpeed(eXSpeed, arrow->GetSpeed(eCompositeSpeed) * sin(launchAngle));
		arrow->SetSpeed(eYSpeed, arrow->GetSpeed(eCompositeSpeed) * cos(launchAngle));

		StartAttack();
		arrow->StartMoving(m_direction);
	}
}

void NPC::Shoot(AttackType projectile)
{
	switch (projectile)	{
	case eFireball: {
		ShootMissile();
		break;
	}
	case eBlueFlame: {
		ShootArrow();
		break;
	}
	default:
		break;
	}
}