#pragma once
#include "Player.h"
#include "PlayerController.h"
#include "Level.h"
#include "ServerConnection.h"
#include "ClientConnection.h"

class Game
{
public: //methods
	//Default constructor
	//TODO: private?
	Game() : m_quit(false) { }

	//Constructor with parameters
	Game(s8* levelId) : m_quit(false), m_frameCounter(0) { m_levelId = levelId; }

	//Default destructor
	~Game() { }

	//Start the game!
	virtual void Run();

protected: //methods
	//Initialize game
	void Initialize();

	//Initialize level
	void InitializeLevel();
	
	//Initialize player
	virtual	void InitializePlayer(PlayerType playerType, u32 keybinds[]);

	//Handle key down even
	void OnKeyDown(const CL_InputEvent &key, const CL_InputState &state);

	//Stop game
	void OnQuit();

protected: //members
	CL_DisplayWindow m_window;
	CL_GraphicContext m_gc;
	CL_ResourceManager m_resources;
	CL_Signal_v0 m_drawSignal;
	CL_Signal_v1<s32> m_updateSignal;
	CL_Slot m_keyDownSlot;
	CL_Slot m_quitSlot;
	std::map<PlayerType, string> m_playerTypes;
	Level* m_level;
	GameObjects* m_players;
	CL_Point m_spawnPoint;
	bool m_quit;
	bool m_pause;
	s8* m_levelId;
	s32 m_frameCounter;
}; //class Game