#pragma once
#include "Game.h"

class GameClient : public Game
{
//TODO: document
public: //methods
	GameClient(s8* levelId): Game(levelId) { }
	~GameClient() { }

	void InitializeNetwork();
	void InitializePuppet(u16 playerType, u8 networkId, CL_Point spawnPoint);
	void ProcessMessage(u8* message);
	void ComposeIntroMessage(); //message id = 0
	void ComposeUpdateMessage(); //message id = 1
	void ComposeSyncMessage(); //message id = 2
	void Run() override;

private: //members
	u8 m_networkId;
	std::map<u8, PlayerController*> m_networkPlayers;
	std::vector<MessageData> m_incomingMessages, m_outgoingMessages;
	ClientConnection m_connection;
	u32 m_frameCounter;
}; //class GameClient

