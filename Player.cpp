#include "Player.h"

Player::Player(CL_GraphicContext* gc, CL_ResourceManager& resources, int posX, int posY, char* objectType, PlayerType playerType)
	: Character(gc, resources, posX, posY, objectType)
{	
	m_attackKeyPressed = false;
	m_jumpKeyPressed = false;
	m_moveLeftKeyPressed = false;
	m_moveRightKeyPressed = false;
	m_playerType = playerType;
}

void Player::SetKeybinds(u32 leftKey, u32 rightKey, u32 jumpKey, u32 attackKey)
{
	m_attackKey = attackKey;
	m_moveLeftKey = leftKey;
	m_moveRightKey = rightKey;
	m_jumpKey = jumpKey;
}

void Player::ConnectSlots(CL_Signal_v0& drawSignal, CL_Signal_v1<int>& updateSignal, CL_DisplayWindow window)
{
	//TODO: pass CL_InputDevice instead of CL_DisplayWindow to prevent unnecessary method invokations
	AnimateObject::ConnectSlots(drawSignal, updateSignal);
	m_keyDownSlot = window.get_ic().get_keyboard().sig_key_down().connect(this, &Player::OnKeyDown);
	m_keyUpSlot = window.get_ic().get_keyboard().sig_key_up().connect(this, &Player::OnKeyUp);
}

void Player::Respawn()
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
	
	m_attackKeyPressed = false;
	m_jumpKeyPressed = false;
	m_moveLeftKeyPressed = false;
	m_moveRightKeyPressed = false;
}

void Player::Destroy()
{
	m_drawSlot.disable();
	m_updateSlot.disable();
	StaticObject::Destroy();
}

void Player::OnKeyUp(const CL_InputEvent &key, const CL_InputState &state)
{
	if (m_flags[eDead]) {
		return;
	}

	//TODO: can this be turned into a switch-case without breaking logic?
	
	if (key.id == m_moveLeftKey) {
		m_moveLeftKeyPressed = false;

		//Handle case where both left and right are pressed before releasing left key
		if (m_moveRightKeyPressed) {
			StartMoving(eRight);			
		} else {
			StopMoving();
		}
	}
	
	if (key.id == m_moveRightKey) {
		m_moveRightKeyPressed = false;

		//Handle case where both left and right are pressed before releasing right key
		if (m_moveLeftKeyPressed) {
			StartMoving(eLeft);
		} else {
			StopMoving();
		}
			
	}

	if (key.id == m_jumpKey) {
		m_jumpKeyPressed = false;
		m_stats[eJumpTime] = m_stats[eMaxJumpTime];
	}

	if (key.id == m_attackKey) {
		m_attackKeyPressed = false;
	}
}

void Player::OnKeyDown(const CL_InputEvent &key, const CL_InputState &state)
{
	if (m_flags[eDead]) {
		//TODO: I was a bit lazy here, maybe introduce respawn key as a keybind?
		if (key.id == CL_KEY_F1) {
			Respawn();
		}
		return;
	}

	//TODO: can this be turned into a switch-case without breaking logic?

	if (key.id == m_moveLeftKey) {
		m_moveLeftKeyPressed = true;
		StartMoving(eLeft);
	} 

	if (key.id == m_moveRightKey) {
		m_moveRightKeyPressed = true;
		StartMoving(eRight);
	}

	if (key.id == m_jumpKey && !m_jumpKeyPressed) {
		m_jumpKeyPressed = true;
		StartJump();
	}

	if (key.id == m_attackKey && !m_attackKeyPressed) {
		m_attackKeyPressed = true;
		StartAttack();
	}
}