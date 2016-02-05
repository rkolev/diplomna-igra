#include "MovingObject.h"
#include "Utility.h"

MovingObject::MovingObject(CL_GraphicContext* gc, CL_ResourceManager& resources, int posX, int posY, char* objectType)
{
	m_graphicContext = gc;
	m_resourceManager = &resources;
	m_position.x = posX;
	m_position.y = posY;
	
	char ConfigFilePath[256] = "ObjectData/MovingObjects/";
	strcat_s(ConfigFilePath, objectType);
	strcat_s(ConfigFilePath, ".xml");
	XmlParser parser(ConfigFilePath);

	Initialize(parser, resources);
}

void MovingObject::Initialize(XmlParser& parser, CL_ResourceManager& resources)
{
	AnimateObject::Initialize(parser, resources);
	
	m_flags[eSpectral] = parser.ReadBool("ObjectData/Spectral");
	m_flags[eAffectedByGravity] = parser.ReadBool("/ObjectData/AffectedByGravity");
	m_flags[eOmnidirectional] = parser.ReadBool("/ObjectData/Omnidirectional");

	char moveSpriteBuf[64];
	parser.ReadText("/ObjectData/MoveSprite", moveSpriteBuf, sizeof(moveSpriteBuf));
	m_moveSprite = CL_Sprite(*m_graphicContext, moveSpriteBuf, &resources);

	m_speeds[eXSpeed] = parser.ReadFloat("/ObjectData/xSpeed");
	m_speeds[eYSpeed] = parser.ReadFloat("/ObjectData/ySpeed");
	m_speeds[eCompositeSpeed] = parser.ReadFloat("/ObjectData/CompositeSpeed");

	m_collisionPoints[0].x = parser.ReadIntAttr("/ObjectData/CollisionPoints/TopOne", "x");
	m_collisionPoints[0].y = parser.ReadIntAttr("/ObjectData/CollisionPoints/TopOne", "y");
	m_collisionPoints[1].x = parser.ReadIntAttr("/ObjectData/CollisionPoints/TopTwo", "x");
	m_collisionPoints[1].y = parser.ReadIntAttr("/ObjectData/CollisionPoints/TopTwo", "y");
	m_collisionPoints[2].x = parser.ReadIntAttr("/ObjectData/CollisionPoints/BottomOne", "x");
	m_collisionPoints[2].y = parser.ReadIntAttr("/ObjectData/CollisionPoints/BottomOne", "y");
	m_collisionPoints[3].x = parser.ReadIntAttr("/ObjectData/CollisionPoints/BottomTwo", "x");
	m_collisionPoints[3].y = parser.ReadIntAttr("/ObjectData/CollisionPoints/BottomTwo", "y");
	m_collisionPoints[4].x = parser.ReadIntAttr("/ObjectData/CollisionPoints/LeftOne", "x");
	m_collisionPoints[4].y = parser.ReadIntAttr("/ObjectData/CollisionPoints/LeftOne", "y");
	m_collisionPoints[5].x = parser.ReadIntAttr("/ObjectData/CollisionPoints/LeftTwo", "x");
	m_collisionPoints[5].y = parser.ReadIntAttr("/ObjectData/CollisionPoints/LeftTwo", "y");
	m_collisionPoints[6].x = parser.ReadIntAttr("/ObjectData/CollisionPoints/RightOne", "x");
	m_collisionPoints[6].y = parser.ReadIntAttr("/ObjectData/CollisionPoints/RightOne", "y");
	m_collisionPoints[7].x = parser.ReadIntAttr("/ObjectData/CollisionPoints/RightTwo", "x");
	m_collisionPoints[7].y = parser.ReadIntAttr("/ObjectData/CollisionPoints/RightTwo", "y");

	m_spawnPoint = m_position;
}

void MovingObject::Update(int timeElapsedMs)
{
	AdjustXSpeed();
	AdjustYSpeed();
	CalculateBounds();
	if (m_flags[eSpectral] == false) {
		HandleCollisions(timeElapsedMs);
	} else {
		m_position.x += Move(m_speeds[eXCurrentSpeed], timeElapsedMs);
		m_position.y += Move(m_speeds[eYCurrentSpeed], timeElapsedMs);
	}
	AnimateObject::Update(timeElapsedMs);
}

void MovingObject::UpdateSprite()
{
	if (m_flags[eAttacking])	{
		//Avoid resetting the sprite animation
		//Avoid rotating the sprite while attacking
		//TODO: I smell a bug
		if (m_activeSprite != m_attackSprite) {
			m_activeSprite = m_attackSprite;
			SetSpriteDirection();
		}
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

void MovingObject::SetSpriteDirection()
{
	if (m_flags[eOmnidirectional])
	{
		//Calculate movement vector and rotate sprite accordingly
		float currentSpeed = sqrt(pow(m_speeds[eXCurrentSpeed], 2) + pow(m_speeds[eYCurrentSpeed], 2));
		float angle = 0.0f;
		if (currentSpeed != 0) {
			float sine = m_speeds[eYCurrentSpeed] / currentSpeed;
			angle = asin(sine) * (180.0f / CL_M_PI);
		}
		m_activeSprite.set_angle(CL_Angle(angle, cl_degrees));
	}
	AnimateObject::SetSpriteDirection();
}

void MovingObject::AdjustXSpeed()
{
	if (m_flags[eMoving]) {
		m_speeds[eXCurrentSpeed] = m_speeds[eXSpeed];
		if (m_flags[eAffectedByGravity] && m_direction == eLeft) {
			//TODO: terrible code (consider using a reference)
			m_speeds[eXCurrentSpeed] = -m_speeds[eXCurrentSpeed];
		}
	} else {
		m_speeds[eXCurrentSpeed] = 0;
	}
}

void MovingObject::AdjustYSpeed()
{
	//TODO: terrible code (consider using a reference)
	if (m_flags[eAffectedByGravity])
	{
		if (m_speeds[eYCurrentSpeed] - GRAVITY < m_speeds[eYSpeed] * JUMP_BOOST) {
			m_speeds[eYCurrentSpeed] += GRAVITY;
		} else {
			m_speeds[eYCurrentSpeed] = m_speeds[eYSpeed] * JUMP_BOOST;
		}
	} else if (m_flags[eMoving])	{
		m_speeds[eYCurrentSpeed] = m_speeds[eYSpeed];
	} else {
		m_speeds[eYCurrentSpeed] = 0;
	}
}

void MovingObject::HandleCollisions(int timeElapsedMs)
{
	bool contacts[3];
	int iterations = 5;
	int nextMoveX = Move(m_speeds[eXCurrentSpeed], timeElapsedMs);
	int nextMoveY = Move(m_speeds[eYCurrentSpeed], timeElapsedMs);
	GameObjects collidingObjects;
	CL_Rect movementBounds;

	contacts[0] = contacts[1] = contacts[2] = true; // 0 = top contact; 1 = bottom contact; 2 = x contact

	movementBounds.left = min(m_bounds.left, m_bounds.left + nextMoveX);
	movementBounds.top = min(m_bounds.top, m_bounds.top + nextMoveY);
	movementBounds.right = max(m_bounds.right, m_bounds.right + nextMoveX);
	movementBounds.bottom = max(m_bounds.bottom, m_bounds.bottom + nextMoveY);

	for (int iteration = 0; iteration < iterations && (contacts[0] || contacts[1] || contacts[2]); iteration++)	{
		collidingObjects = Utility::GetCollidingObjects(movementBounds, m_environment);
		contacts[0] = contacts[1] = contacts[2] = false;

		int originalMoveX, originalMoveY;
		originalMoveX = nextMoveX;
		originalMoveY = nextMoveY;

		//Detect collisions
		GameObjects::iterator it = collidingObjects.begin();
		for ( ; it != collidingObjects.end() && contacts[0] == false && contacts[1] == false && contacts[2] == false; it++)	{
			if (this == *it) {
				//don't collide with urself
				continue;
			}
			Utility::SpecContacts((*it)->GetBounds(), movementBounds, m_bounds, nextMoveX, nextMoveY, m_collisionPoints);
			Utility::PenetrationResolution((*it)->GetBounds(), m_bounds, nextMoveX, nextMoveY, m_collisionPoints);
			Utility::DetectContact(contacts, originalMoveX, originalMoveY, nextMoveX, nextMoveY);

			ResolveCollisions(contacts, nextMoveX, nextMoveY);
		}
	}

	//Update the player's position
	m_position.x += static_cast<int>(Move(m_speeds[eXCurrentSpeed], timeElapsedMs));
	m_position.y += static_cast<int>(Move(m_speeds[eYCurrentSpeed], timeElapsedMs));
}

void MovingObject::ResolveCollisions(bool contacts[3], int nextMoveX, int nextMoveY)
{
	//TODO: needs implementation
}

void MovingObject::StartMoving(Direction dir)
{
	m_flags[eMoving] = true;
	m_direction = dir;
}

void MovingObject::StopMoving()
{
	m_flags[eMoving] = false;
}

int MovingObject::Move(float speed, int timeElapsedMs)
{
	//TODO: this can (really) go into a util namespace,
	//since it doesn't mutate any class members
	int offset = static_cast<int>(std::ceil(abs(speed*timeElapsedMs)));
	if (speed < 0)
		offset = -offset;
	return offset;
}
