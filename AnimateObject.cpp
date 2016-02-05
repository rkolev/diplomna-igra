#include "AnimateObject.h"

AnimateObject::AnimateObject(CL_GraphicContext* gc, CL_ResourceManager& resources, s32 posX, s32 posY, s8* objectType)
{
	m_graphicContext = gc;
	m_resourceManager = &resources;
	m_position.x = posX;
	m_position.y = posY;
	
	s8 ConfigFilePath[256] = "ObjectData/AnimateObjects/";
	//TODO: what's this x for?
	s32 x = sizeof(ConfigFilePath);
	strcat_s(ConfigFilePath, objectType);
	strcat_s(ConfigFilePath, ".xml");
	XmlParser parser(ConfigFilePath);

	Initialize(parser, resources);
}

void AnimateObject::Initialize(XmlParser& parser, CL_ResourceManager& resources)
{
	StaticObject::Initialize(parser, resources);
	m_flags[eCanAttack] = parser.ReadBool("/ObjectData/CanAttack");
	m_flags[eCanShoot] = parser.ReadBool("/ObjectData/CanShoot");
	if (m_flags[eCanAttack])	{
		m_attackType = (AttackType)parser.ReadInt("ObjectData/AttackType");
		m_stats[eSightRadius] = parser.ReadInt("ObjectData/SightRadius");
		m_stats[eAttackRange] = parser.ReadInt("/ObjectData/AttackRange");
		m_stats[eAttackPower] = parser.ReadInt("ObjectData/AttackPower");
		m_stats[eAttackCooldown] = parser.ReadInt("/ObjectData/AttackCooldown");
		m_stats[eAttackFrame] = parser.ReadInt("ObjectData/AttackFrame");
		m_stats[eTimeSinceLastAttack] = m_stats[eAttackCooldown];
		char attackSpriteBuf[64];
		parser.ReadText("/ObjectData/AttackSprite", attackSpriteBuf, sizeof(attackSpriteBuf));
		//TODO: a good idea is to somehow initialize attackSprite if canattack = false
		m_attackSprite = CL_Sprite(*m_graphicContext, attackSpriteBuf, &resources);
		m_attackBoundsOffsets[0] = parser.ReadIntAttr("/ObjectData/AttackBounds", "left");
		m_attackBoundsOffsets[1] = parser.ReadIntAttr("/ObjectData/AttackBounds", "right");
		m_attackBoundsOffsets[2] = parser.ReadIntAttr("/ObjectData/AttackBounds", "top");
		m_attackBoundsOffsets[3] = parser.ReadIntAttr("/ObjectData/AttackBounds", "bottom");
	}
}

void AnimateObject::ConnectSlots(CL_Signal_v0& drawSignal, CL_Signal_v1<int>& updateSignal)
{
	StaticObject::ConnectSlots(drawSignal);
	m_updateSlot = updateSignal.connect(this, &AnimateObject::Update);
	m_drawSignal = &drawSignal;
	m_updateSignal = &updateSignal;
}

void AnimateObject::CalculateBounds()
{
	StaticObject::CalculateBounds();
	//TODO: terrible code (consider using a reference
	int xOffset = m_direction == eRight ? m_stats[eAttackRange] : -m_stats[eAttackRange];
	m_attackBounds.left = m_position.x - m_attackBoundsOffsets[0] + xOffset;
	m_attackBounds.right = m_position.x + m_attackBoundsOffsets[1] + xOffset;
	m_attackBounds.top = m_position.y - m_attackBoundsOffsets[2];
	m_attackBounds.bottom = m_position.y + m_attackBoundsOffsets[3];
}

void AnimateObject::Update(int timeElapsedMs)
{
	HandleAttackState(timeElapsedMs);
	UpdateSprite();
	m_activeSprite.update();
}

void AnimateObject::StartAttack()
{
	//Invoked by a controller object, sets off an attack animation (melee)
	if (m_flags[eCanAttack] && (m_stats[eTimeSinceLastAttack] > m_stats[eAttackCooldown]))	{
		m_flags[eAttacking] = true;
	}
}

void AnimateObject::StartAttack(CL_Point target)
{
	//Ranged attack
	//TODO: implementation
}

void AnimateObject::UpdateSprite()
{
	if (m_flags[eAttacking]) {
		m_activeSprite = m_attackSprite;
		SetSpriteDirection();
		return;
	}
	
	m_activeSprite = m_idleSprite;
	SetSpriteDirection();
}

void AnimateObject::SetSpriteDirection()
{
	//Set the direction in which the sprite is drawn, depending on the object's current dir
	float angle = m_direction == eLeft ? 180.0f : 0.0f;
	m_activeSprite.set_angle_yaw(CL_Angle(angle, cl_degrees));
}

void AnimateObject::HandleAttackState(int timeElapsedMs)
{
	if (m_flags[eAttacking]) {
		if (m_activeSprite.is_finished()) {
			m_flags[eAttacking] = false;
			m_stats[eTimeSinceLastAttack] = 0;
			m_attackSprite.restart();
		} else {
			//Only check for hits on a specific frame
			//This way we avoid multiple hits in a single animation
			//Open for better suggestions
			if (m_activeSprite.get_current_frame() == m_stats[eAttackFrame])	{
				PerformHitCheck();
			}
		}
	} else {
		m_stats[eTimeSinceLastAttack] += timeElapsedMs;
	}
}

void AnimateObject::PerformHitCheck()
{
	for (GameObjects::iterator it = m_enemies->begin(); it != m_enemies->end(); it++) {
		if (this == *it) continue; //just in case
		if (Utility::BoundsIntersect((*it)->GetBounds(), m_attackBounds)) {
			(*it)->Hurt(m_stats[eAttackPower]);
		}
	}

	for (GameObjects::iterator it = m_destructibles->begin(); it != m_destructibles->end(); it++) {
		if (this == *it) continue; //just in case
		if (Utility::BoundsIntersect((*it)->GetBounds(), m_attackBounds)) {
			(*it)->Hurt(m_stats[eAttackPower]);
		}
	}
}