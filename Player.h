#pragma once

#include "Character.h"

//This class is used to draw and control the local player character
class Player : public Character
{
public: //methods
	//Default constructor TODO: proper initialization or make it private
	//do we want to be able to create gameobjects this way?
	Player() {}

	//Constructor with values
	Player(CL_GraphicContext* gc, CL_ResourceManager& resources, s32 posX, s32 posY, s8* objectType, PlayerType playerType);

	//Default destructor
	~Player() {}

	//Connects @drawSignal and @updateSignal to the object's drawSlot and updateSlot
	//Connects the keyboard slot from @window to the keyDownSlot and keyUpSlot
	void ConnectSlots(CL_Signal_v0& drawSignal, CL_Signal_v1<s32>& updateSignal, CL_DisplayWindow window);

	//Sets the player's control keybinds
	void SetKeybinds (u32 leftKey, u32 rightKey, u32 jumpKey, u32 attackKey);

	//Returns the player's character type
	PlayerType GetType() { return m_playerType; }

private: //methods
	//In addition to setting the Dead flag to true, this method
	//Disables the draw and update slots for the player
	//This prevents the player from being drawn or responding to keyboard commands
	//Until the respawn button is pressed
	//This is done because player objects are destroyed up when they die (so they can respawn)
	void Destroy() override;

	//Re-enables the draw and update slots of the object
	void Respawn();

	//Keyboard input handling (when key is released)
	void OnKeyUp(const CL_InputEvent &key, const CL_InputState &state);

	//Keyboard input handling (when key is pressed)
	void OnKeyDown(const CL_InputEvent &key, const CL_InputState &state);

protected: //members
	//Flags for keypresses
	bool m_jumpKeyPressed;
	bool m_attackKeyPressed;
	bool m_moveLeftKeyPressed;
	bool m_moveRightKeyPressed;

	//Keybinds
	u32 m_attackKey;
	u32 m_moveRightKey;
	u32 m_moveLeftKey;
	u32 m_jumpKey;

	PlayerType m_playerType;
	CL_Slot m_keyDownSlot;
	CL_Slot m_keyUpSlot;
};