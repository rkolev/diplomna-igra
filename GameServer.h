#pragma once
#include "Game.h"

class GameServer : public Game
{
//TODO: document
public: //methods
	GameServer(s8* levelId) : Game(levelId) { }
	~GameServer() { }
	void Run() override;

private: //methods
	void InitializeNetwork();
	void InitializePlayer(PlayerType playerType, u32 keybinds[]) override;
	void InitializePuppet(u8 playerType);
	void HandleConnectionDrop(u8 clientId);
	void ProcessMessage(u8* message);
	void ComposeConnectionDropMessage(u8 networkId); //message id = 4
	void ComposeIntroMessage(u8 networkId); //message id = 0
	void ComposeUpdateMessage(); //message id = 1
	void ComposeSyncMessage(); //message id = 2
	void ComposeNewClientMessage(u8 networkId, u8 playerType); //message id = 3

private: //members
	static u8 ms_networkIdCounter;
	u8 m_networkId;
	CL_Slot m_connectionDropSlot;
	std::map<u8, PlayerController*> m_networkPlayers;
	std::vector<u8> m_networkIds;
	//std::map<int, int> networkConnections;
	//std::vector<PlayerController*> networkPlayers;
	std::vector<MessageData> m_incomingMessages, m_outgoingMessages;
	ServerConnection m_connection;

};

