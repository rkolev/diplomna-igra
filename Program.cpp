#include "Program.h"

int Program::main(const std::vector<CL_String> &args)
{
	try
	{
		CL_SetupCore setupCore;
		CL_SetupDisplay setupDisplay;
		CL_SetupGL setup_gl;
		s8 gameSettings[256] = "settings/GameSettings.xml";
		XmlParser parser(gameSettings);
		GameMode gameMode = (GameMode)parser.ReadInt("/GameData/Type");
		s8 levelId[256];
		parser.ReadText("/GameData/Level", levelId, sizeof(levelId));
		Game* game;

		switch (gameMode)
		{
		case eSinglePlayer:
		{
			game = new Game(levelId);
			break;
		}
			
		case eHotseat:
		{
			game = new HotseatGame(levelId);
			break;
		}

		case eMultiplayerServer:
		{
			game = new GameServer(levelId);
			break;
		}

		case eMultiplayerClient:
		{
			game = new GameClient(levelId);
			break;
		}
		}

		game->Run();
		delete game;
		return 0;
	}
	catch(CL_Exception &exception)
	{
		// Create a console window for text-output if not available
		CL_ConsoleWindow console("Console", 80, 160);
		CL_Console::write_line("Exception caught: " + exception.get_message_and_stack_trace());
		console.display_close_message();

		return -1;
	}
}

// Instantiate CL_ClanApplication, informing it where the Program is located
CL_ClanApplication app(&Program::main);