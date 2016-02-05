#include "PlayerController.h"

PlayerController::PlayerController(CL_GraphicContext* gc, CL_ResourceManager& resources, int posX, int posY, char* objectType, PlayerType playerType)
	: Character(gc, resources, posX, posY, objectType)
{
	m_playerType = playerType;
}

void PlayerController::Update(s32 timeElapsedMs)
{
	TakeAction();
	Character::Update(timeElapsedMs);
}

void PlayerController::ReadState(u8 state)
{
	for (int i = 0; i < PLAYER_ACTION_COUNT; i++) {
		m_nextActions[(BasicAction)i] = state >> i & 1;
	}
	
	bool dead = (bool)(state >> PLAYER_ACTION_COUNT & 1);
	m_direction = (Direction)(state >> (PLAYER_ACTION_COUNT + 1) & 1);
	
	//If nextAction states that the puppet is dead and it isn't actually dead,
	//KILL IT!
	if ((m_flags[eDead] != dead) && m_flags[eDead] == false) {
		Destroy();
		m_flags[eDead] = dead;
	}

	//If nextAction states that the puppet has respawned and it's still dead
	//REVIVE IT!
	if ((m_flags[eDead] != dead) && m_flags[eDead]) {
		Respawn();
		m_flags[eDead] = dead;
	}
}

void PlayerController::TakeAction()
{
	//Attack
	if (m_nextActions[eAttack]) {
		StartAttack();
	}

	//Jump
	if (m_nextActions[eJump]) {
		StartJump();
	} else {
		m_stats[eJumpTime] = m_stats[eMaxJumpTime];
	}

	if (m_nextActions[eMove]) {
		StartMoving(m_direction);
	} else {
		StopMoving();
	}
}

void PlayerController::Destroy()
{
	m_drawSlot.disable();
	m_updateSlot.disable();
	StaticObject::Destroy();
}

void PlayerController::Respawn()
{
	m_flags[eDead] = false;
	m_flags[eAttacking] = false;
	m_flags[eMoving] = false;
	m_flags[eJumping] = false;
	m_stats[eCurrentLife] = m_stats[eMaxLife];
	m_direction = eRight;
	m_position = m_spawnPoint;

	m_drawSlot.enable();
	m_updateSlot.enable();
}