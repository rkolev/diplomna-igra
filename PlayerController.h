#pragma once
#include "Character.h"

//This is a controller class, used to display network players on the local game instance
class PlayerController : public Character
{
public: //methods
	//Default constructor TODO: proper initialization or make it private
	//do we want to be able to create gameobjects this way?
	PlayerController() {}

	//Constructor with values
	PlayerController(CL_GraphicContext* gc, CL_ResourceManager& resources, s32 posX, s32 posY, s8* objectType, PlayerType playerType);
	
	//Default destructor
	~PlayerController() {}

	//Updates the current object's state according to changes to game state since the last frame
	void Update(s32 timeElapsedMs) override;

	//Returns the player's character type
	PlayerType GetType() { return m_playerType; }

	//Reads the provided state (in the form of a series of flags) of the remote network player
	//this controller is assigned to and composes the object's next actions
	//that will be undertaken in the next Update() call
	void ReadState(u8);

private: //methods
	//Adjusts the player state according to the values in nextActions
	void TakeAction();
	void Respawn();
	void Destroy() override;

private: //members
	PlayerType m_playerType;
	std::map <BasicAction, bool> m_nextActions;
};

