#include "Character.h"

Character::Character(CL_GraphicContext* gc, CL_ResourceManager& resources, int posX, int posY, char* objectType)
{
	m_graphicContext = gc;
	m_resourceManager = &resources;
	m_position.x = posX;
	m_position.y = posY;
	
	char ConfigFilePath[256] = "ObjectData/Characters/";
	strcat_s(ConfigFilePath, objectType);
	strcat_s(ConfigFilePath, ".xml");
	XmlParser parser(ConfigFilePath);

	Initialize(parser, resources);
}

void Character::Initialize(XmlParser& parser, CL_ResourceManager& resources)
{
	MovingObject::Initialize(parser, resources);
	char jumpSpriteBuf[64];
	parser.ReadText("/ObjectData/JumpSprite", jumpSpriteBuf, sizeof(jumpSpriteBuf));
	m_jumpSprite = CL_Sprite(*m_graphicContext, jumpSpriteBuf, &resources);
	
	char fallSpriteBuf[64];
	parser.ReadText("/ObjectData/FallSprite", fallSpriteBuf, sizeof(fallSpriteBuf));
	m_fallSprite = CL_Sprite(*m_graphicContext, fallSpriteBuf, &resources);
	
	m_stats[eJumpTime] = m_stats[eMaxJumpTime] = parser.ReadInt("/ObjectData/JumpTime");	
}

void Character::Update(int timeElapsedMs)
{
	HandleJumpFallState(timeElapsedMs);
	MovingObject::Update(timeElapsedMs);
}

unsigned short Character::GetCurrentState()
{
	u8 status = 0;
	status |= (u8)m_flags[eAttacking]; //attack flag
	status |= (u8)m_flags[eJumping] << 1; //jump flag
	status |= (u8)m_flags[eMoving] << 2; //move flag
	status |= (u8)m_flags[eDead] << 3;
	status |= (u8)m_direction << 4; //if dir is RIGHT, flip fourth bit to 1, otherwise, leave as 0;
	return status;
}

void Character::UpdateSprite()
{
	if (m_flags[eAttacking]) {
		m_activeSprite = m_attackSprite;
		SetSpriteDirection();
		return;
	}

	if (m_flags[eJumping] || m_flags[eFalling]) {
		m_activeSprite = m_flags[eJumping] == true ? m_jumpSprite : m_fallSprite;
		SetSpriteDirection();
		return;
	}

	if (m_flags[eMoving]) {
		m_activeSprite = m_moveSprite;
		SetSpriteDirection();
		return;
	}	

	m_activeSprite = m_idleSprite;
	SetSpriteDirection();
}

void Character::HandleJumpFallState(int timeElapsedMs)
{
	if (!m_flags[eAffectedByGravity]){
		return;
	}

	if (m_stats[eJumpTime] < m_stats[eMaxJumpTime]) {
		m_stats[eJumpTime] += timeElapsedMs;
	} else {
		m_flags[eJumping] = false;
		m_flags[eFalling] = true;
	}
}

void Character::AdjustYSpeed()
{
	//TODO: terrible code, consider using references
	if (m_flags[eAffectedByGravity]) {
		if (m_flags[eJumping]) {
			m_speeds[eYCurrentSpeed] = -m_speeds[eYSpeed];
		}
		if (m_flags[eFalling]) {
			if (m_speeds[eYCurrentSpeed] - GRAVITY < m_speeds[eYSpeed] * JUMP_BOOST) {
				m_speeds[eYCurrentSpeed] += GRAVITY;
			} else {
				m_speeds[eYCurrentSpeed] = m_speeds[eYSpeed] * JUMP_BOOST;
			}
		}
		if (!m_flags[eJumping] && !m_flags[eFalling])	{
			m_speeds[eYCurrentSpeed] = GRAVITY;
		}
	} else if (m_flags[eMoving]) {
		m_speeds[eYCurrentSpeed] = m_speeds[eYSpeed];
	} else {
		m_speeds[eYCurrentSpeed] = 0;
	}
}

void Character::ResolveCollisions(bool contacts[3], int nextMoveX, int nextMoveY)
{
	//if a top contact has occurred, we need to stop jumping and start falling
	if (contacts[0] && m_speeds[eYCurrentSpeed] < 0) {
		m_speeds[eYCurrentSpeed] = 0;
		m_stats[eJumpTime] = m_stats[eMaxJumpTime];
	}

	//if a contact has occurred, use the recalculated movement vector
	//and prevent any further movement in this frame by setting
	//the xSpeed and the ySpeed to 0
	if (contacts[1]) {
		m_position.y += static_cast<int>(nextMoveY);
		m_speeds[eYCurrentSpeed] = 0;
		m_flags[eFalling] = false;
		m_flags[eJumping] = false;
	}

	if (contacts[2]) {
		m_position.x += static_cast<int>(nextMoveX);
		m_speeds[eXCurrentSpeed] = 0;
	}
}

void Character::StartJump()
{
	if (!m_flags[eJumping] && !m_flags[eFalling]) {
		m_flags[eJumping] = true;
		m_stats[eJumpTime] = 0;
	}
}