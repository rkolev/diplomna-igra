#include "Game.h"

void Game::Initialize()
{
	//Setup game window
	CL_DisplayWindowDescription desc;
	desc.set_title("SUPER MEGA AWESOME GAME                 by R&D Intercative");
	desc.set_size(CL_Size(RESOLUTION_X, RESOLUTION_Y), true);
	desc.set_allow_resize(false);
	m_window = CL_DisplayWindow(desc);

	//Setup input device and graphic context
	m_gc = m_window.get_gc();

	//Initialize resource manager and quit slot
	m_resources = CL_ResourceManager("resources.xml");
	m_quitSlot = m_window.sig_window_close().connect(this, &Game::OnQuit);
	m_keyDownSlot = m_window.get_ic().get_keyboard().sig_key_down().connect(this, &Game::OnKeyDown);

	XmlParser parser("settings/PlayerClasses.xml");
	s32 playerTypeCount = parser.ReadIntAttr("/PlayerClasses", "count");

	for (s32 i = 0; i < playerTypeCount; i++) {
		s8 node[64] = "/PlayerClasses/Class";
		s8 temp[5];
		itoa(i + 1, temp, 10);
		strcat(node, temp);
		s8 playerClass[32];
		parser.ReadText(node, playerClass, sizeof(playerClass));
		m_playerTypes.insert(std::pair<PlayerType, string>((PlayerType)i, string(playerClass)));
	}
}

void Game::InitializeLevel()
{
	m_level = new Level(&m_gc, m_resources, m_drawSignal, m_updateSignal, m_levelId);
	m_spawnPoint = m_level->GetSpawnPoint();

	//Store the level's player list so we can easily add new players to it
	m_players = m_level->GetPlayers();
}

void Game::InitializePlayer(PlayerType playerType, u32 keybinds[])
{
	//Initialize local player
	s8 playerClass[32];
	strcpy(playerClass, m_playerTypes[playerType].c_str());
	Player* player = new Player(&m_gc, m_resources, m_spawnPoint.x, m_spawnPoint.y, playerClass, playerType);
	player->ConnectSlots(m_drawSignal, m_updateSignal, m_window);
	player->SetKeybinds(keybinds[0], keybinds[1], keybinds[2], keybinds[3]);
	player->SetEnvList(m_level->GetCollidableObjects());
	player->SetEnemyList(m_level->GetHostiles());
	player->SetDestructiblesList(m_level->GetDestructibles());

	//Add local player to the list of players
	m_players->push_back(player);
	m_level->GetHostiles()->push_back(player);
}

void Game::Run()
{
	XmlParser parser("settings/Players.xml");
	PlayerType playerType = (PlayerType)parser.ReadInt("/PlayerData/PlayerOneType");
	u32 keybinds[4] = { CL_KEY_LEFT, CL_KEY_RIGHT, CL_KEY_UP, CL_KEY_J };

	Initialize();
	InitializeLevel();	
	InitializePlayer(playerType, keybinds);

	s32 timeElapsedMs, lastTime, currentTime, waitTime;
	currentTime = lastTime = CL_System::get_time();

	// Main game loop
	while(!m_quit) {
		//Calculate the elapsed time since the beginning of the last frame
		currentTime = CL_System::get_time();
		timeElapsedMs = currentTime - lastTime;
		if (timeElapsedMs > 50)	{
			//Guard to ensure that the frame time isn't too long
			timeElapsedMs = 50;
		}
		lastTime = currentTime;
		
		//Update the game state
		m_level->Update();
		m_updateSignal.invoke(timeElapsedMs);	
		m_drawSignal.invoke();
		
		// Flip the display, showing on the screen what we drew since last call to flip()
		m_window.flip(0);

		//Measure the time for this frame's update
		waitTime = 16 - (CL_System::get_time() - currentTime);
		if (waitTime < 0) {
			waitTime = 0;
		}

		// This call processes user input and other events
		CL_KeepAlive::process(0);

		// Sleep for a little while to avoid using too much of the CPU.
		CL_System::sleep(waitTime);		
	}
}

void Game::OnQuit()
{
	m_quit = true;
}

void Game::OnKeyDown(const CL_InputEvent &key, const CL_InputState &state)
{
	if (key.id == CL_KEY_ESCAPE) {
		m_quit = true;
	}
}