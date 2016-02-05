#include "Level.h"

Level::Level()
{

}

Level::Level(CL_GraphicContext* gc, CL_ResourceManager& resources, CL_Signal_v0& drawSignal, CL_Signal_v1<int>& updateSignal, char* levelId)
{
	ObjectDataList staticObjects, animateObjects, movingObjects, characters;
	s8 filePath[256] = "Levels/";
	strcat_s(filePath, levelId);
	strcat_s(filePath, ".xml");
	XmlParser parser(filePath);

	staticObjects = parser.GetObjectInfo("/LevelData/StaticObject");
	animateObjects = parser.GetObjectInfo("/LevelData/AnimateObject");
	movingObjects = parser.GetObjectInfo("/LevelData/MovingObject");
	characters = parser.GetObjectInfo("/LevelData/NPC");

	m_spawnPoint.x = parser.ReadInt("/LevelData/SpawnPointX");
	m_spawnPoint.y = parser.ReadInt("/LevelData/SpawnPointY");
	m_spawnDirection = eRight; //TODO: for now, leave it static. If we have time to implement dynamic spawn direction do it then

	if (animateObjects.size() > 0) {
		ObjectDataList::iterator it = animateObjects.begin();
		for ( ; it != animateObjects.end(); ++it) {
			s8* name = it->name;
			s32 posX = it->posX;
			s32 posY = it->posY;
			AnimateObject* object = new AnimateObject(gc, resources, posX, posY, name);
			object->ConnectSlots(drawSignal, updateSignal);
			m_objects.push_back(object);

			if (object->GetFlag(eCollidable)) {
				m_collidableObjects.push_back(object);
			}

			if (object->GetFlag(eDestructible) && !object->GetFlag(eCanAttack)) {
				m_destructibles.push_back(object);
			}

			if (object->GetFlag(eCanAttack)) {
				object->SetEnemyList(&m_players);
				object->SetDestructiblesList(&m_destructibles);
				m_hostiles.push_back(object);
			}
		}
	}

	if (staticObjects.size() > 0) {
		ObjectDataList::iterator it = staticObjects.begin();
		for ( ; it != staticObjects.end(); ++it) {
			s8* name = it->name;
			s32 posX = it->posX;
			s32 posY = it->posY;
			StaticObject* object = new StaticObject(gc, resources, posX, posY, name);
			object->ConnectSlots(drawSignal);
			m_objects.push_back(object);

			if (object->GetFlag(eCollidable)) {
				m_collidableObjects.push_back(object);
			}

			if (object->GetFlag(eDestructible)) {
				m_destructibles.push_back(object);
			}
		}
	}

	if (movingObjects.size() > 0) {
		ObjectDataList::iterator it = movingObjects.begin();
		for ( ; it != movingObjects.end(); ++it) {
			s8* name = it->name;
			s32 posX = it->posX;
			s32 posY = it->posY;
			MovingObject* object = new MovingObject(gc, resources, posX, posY, name);
			object->ConnectSlots(drawSignal, updateSignal);
			object->SetEnvList(&m_collidableObjects);
			m_objects.push_back(object);

			if (object->GetFlag(eCollidable)) {
				m_collidableObjects.push_back(object);
			}

			if (object->GetFlag(eDestructible) && !object->GetFlag(eCanAttack)) {
				m_destructibles.push_back(object);
			}

			if (object->GetFlag(eCanAttack)) {
				object->SetEnemyList(&m_players);
				object->SetDestructiblesList(&m_destructibles);
				m_hostiles.push_back(object);
			}
		}
	}

	if (characters.size() > 0)
	{
		ObjectDataList::iterator it = characters.begin();
		for (; it != characters.end(); ++it)
		{
			s8* name = it->name;
			s32 posX = it->posX;
			s32 posY = it->posY;
			NPC* object = new NPC(gc, resources, posX, posY, name);
			object->ConnectSlots(drawSignal, updateSignal);
			object->SetEnvList(&m_collidableObjects);
			object->SetEnemyList(&m_players);
			m_objects.push_back(object);

			if (object->GetFlag(eCollidable)) {
				m_collidableObjects.push_back(object);
			}

			if (object->GetFlag(eDestructible) && !object->GetFlag(eCanAttack)) {
				m_destructibles.push_back(object);
			}

			if (object->GetFlag(eCanAttack)) {
				object->SetEnemyList(&m_players);
				object->SetDestructiblesList(&m_destructibles);
				m_hostiles.push_back(object);
			}
		}
	}
}

Level::~Level(void)
{
	if (m_objects.size() > 0){
		GameObjects::iterator it = m_objects.begin();
		for (; it != m_objects.end(); ++it)	{
			delete (*it);
		}
	}

	if (m_players.size() > 0) {
		GameObjects::iterator it = m_players.begin();
		for (; it != m_players.end(); ++it)	{
			delete (*it);
		}
	}
}

void Level::Update()
{
	//Cleanup lists, removing destroyed objects
	GameObjects::iterator it = m_objects.begin();
	for (; it != m_objects.end();) {
		if ((*it)->GetFlag(eDead)) {
			//delete object from other lists
			//TODO: a simple .find would be a better alternative to these checks
			if ((*it)->GetFlag(eCollidable)){
				m_collidableObjects.remove(*it);
			}

			if ((*it)->GetFlag(eDestructible) && !(*it)->GetFlag(eCanAttack)) {
				m_destructibles.remove(*it);
			}

			if ((*it)->GetFlag(eCanAttack)) {
				m_hostiles.remove(*it);
			}

			delete *it;
			it = m_objects.erase(it);
		} else {
			++it;
		}
	}
}