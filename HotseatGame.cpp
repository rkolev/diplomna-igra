#include "HotseatGame.h"

void HotseatGame::Run()
{
	XmlParser parser("settings/Players.xml");
	PlayerType playerOneType = (PlayerType)parser.ReadInt("/PlayerData/PlayerOneType");
	PlayerType playerTwoType = (PlayerType)parser.ReadInt("/PlayerData/PlayerTwoType");
	u32 playerOneKeybinds[4] = { CL_KEY_LEFT, CL_KEY_RIGHT, CL_KEY_UP, CL_KEY_J };
	u32 playerTwoKeybinds[4] = { CL_KEY_A, CL_KEY_D, CL_KEY_SPACE, CL_KEY_E };
	
	Initialize();
	InitializeLevel();
	InitializePlayer(playerOneType, playerOneKeybinds);
	InitializePlayer(playerTwoType, playerTwoKeybinds);

	s32 timeElapsedMs, lastTime, currentTime, waitTime;
	currentTime = lastTime = CL_System::get_time();

	// Main game loop
	while (!m_quit) {
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
		if (waitTime < 0){
			waitTime = 0;
		}

		// This call processes user input and other events
		CL_KeepAlive::process(0);

		// Sleep for a little while to avoid using too much of the CPU.
		CL_System::sleep(waitTime);
	}
}