#pragma once

#include "Player.h"
#include "NPC.h"

class Level
{
public: //methods
	//Default constructor
	//TODO: private?
	Level();

	//Constructor with parameters
	Level(CL_GraphicContext* gc, CL_ResourceManager& resources, CL_Signal_v0& drawSignal, CL_Signal_v1<int>& updateSignal, char* levelId);

	//Default destructor
	~Level();

	//Update level (cleanup any dead objects)
	void Update();

	//Get level's collidable objects
	GameObjects* GetCollidableObjects() { return &m_collidableObjects; }

	//Get level's player objects
	GameObjects* GetPlayers() { return &m_players; }

	//Get level's hostile (to the players) objects
	GameObjects* GetHostiles() { return &m_hostiles; }

	//Get level's destructible objects
	GameObjects* GetDestructibles() { return &m_destructibles; }

	//Get level's spawn point
	CL_Point GetSpawnPoint() { return m_spawnPoint; }

	//Get level's spawn direction
	Direction GetSpawnDirection() { return m_spawnDirection; }

private: //members
	GameObjects m_objects, m_collidableObjects, m_players, m_hostiles, m_destructibles;
	CL_Point m_spawnPoint; //The player's spawn point
	Direction m_spawnDirection; //The player's spawn direction
	CL_Slot m_updateSignal;
};

