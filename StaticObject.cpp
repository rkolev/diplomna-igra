#include "StaticObject.h"

StaticObject::StaticObject(CL_GraphicContext* gc, CL_ResourceManager& resources, int posX, int posY, char* objectType)
{	
	m_graphicContext = gc;
	m_resourceManager = &resources;
	m_position.x = posX;
	m_position.y = posY;
	strcpy_s(m_objectType, objectType);

	char configFilePath[256] = "ObjectData/StaticObjects/";
	strcat_s(configFilePath, objectType);
	strcat_s(configFilePath, ".xml");
	XmlParser parser(configFilePath);

	Initialize(parser, resources);
}

void StaticObject::InitializeMaps()
{
	for (int i = 0; i < eSpeedElementCount; i++) {
		m_speeds.insert(std::pair<Speed, float>((Speed)i, 0.0f));
	}

	for (int i = 0; i < eStatElementCount; i++) {
		m_stats.insert(std::pair<Stat, int>((Stat)i, 0));
	}

	for (int i = 0; i < eFlagElementCount; i++) {
		m_flags.insert(std::pair<Flag, bool>((Flag)i, false));
	}
}

void StaticObject::Initialize(XmlParser& parser, CL_ResourceManager& resources)
{
	InitializeMaps();
	m_flags[eCollidable] = parser.ReadBool("/ObjectData/Collidable");
	m_flags[eDestructible] = parser.ReadBool("/ObjectData/Destructible");

	char idleSpriteBuf[64];
	parser.ReadText("/ObjectData/IdleSprite", idleSpriteBuf, sizeof(idleSpriteBuf));
	m_idleSprite = CL_Sprite(*m_graphicContext, idleSpriteBuf, &resources);
	m_activeSprite = m_idleSprite;

	m_boundOffsets[0] = parser.ReadIntAttr("/ObjectData/BoundOffsets", "left");
	m_boundOffsets[1] = parser.ReadIntAttr("/ObjectData/BoundOffsets", "right");
	m_boundOffsets[2] = parser.ReadIntAttr("/ObjectData/BoundOffsets", "top");
	m_boundOffsets[3] = parser.ReadIntAttr("/ObjectData/BoundOffsets", "bottom");

	m_stats[eContactDamage] = parser.ReadInt("/ObjectData/ContactDamage");
	m_stats[eMaxLife] = m_stats[eCurrentLife] = parser.ReadInt("/ObjectData/MaxLife");
	CalculateBounds();

	//TODO: currently all objects are initialized facing right.
	//Do we need to dynamically set the direction at spawn? (probably yes)
	m_direction = eRight; 
}

void StaticObject::Draw()
{
	m_activeSprite.draw(*m_graphicContext, m_position.x, m_position.y);
}

void StaticObject::ConnectSlots(CL_Signal_v0& drawSignal)
{
	m_drawSlot = drawSignal.connect(this, &StaticObject::Draw);
}

void StaticObject::CalculateBounds()
{
	//TODO: can be moved into an util namespace?
	m_bounds.left = m_position.x - m_boundOffsets[0];
	m_bounds.right = m_position.x + m_boundOffsets[1];
	m_bounds.top = m_position.y - m_boundOffsets[2];
	m_bounds.bottom = m_position.y + m_boundOffsets[3];
}

void StaticObject::Hurt(int damage)
{
	//TODO: references
	if (m_flags[eDestructible]) {
		m_stats[eCurrentLife] -= damage;
		if (m_stats[eCurrentLife] <= 0) {
			m_stats[eCurrentLife] = 0;
			Destroy();
		}
	}
}

void StaticObject::Destroy()
{
	m_flags[eDead] = true;
}