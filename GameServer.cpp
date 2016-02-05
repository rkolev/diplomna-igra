#include "GameServer.h"

u8 GameServer::ms_networkIdCounter = 0;

void GameServer::InitializeNetwork()
{
	InitializeSockets();	
	m_connection = ServerConnection(ProtocolId, TimeOut);
	m_connection.ConnectSlots(m_updateSignal);
	m_connectionDropSlot = m_connection.GetConnectionDropSignal().connect(this, &GameServer::HandleConnectionDrop);
	m_connection.Start(ServerPort);
}

void GameServer::InitializePlayer(PlayerType playerType, u32 keybinds[])
{
	m_networkId = ms_networkIdCounter++;
	Game::InitializePlayer(playerType, keybinds);
}

void GameServer::InitializePuppet(u8 playerType)
{
	ComposeIntroMessage(ms_networkIdCounter);

	s8 playerClass[32];
	strcpy(playerClass, m_playerTypes[(PlayerType)playerType].c_str());
	PlayerController* puppet = new PlayerController(&m_gc, m_resources, m_spawnPoint.x, m_spawnPoint.y, playerClass, (PlayerType)playerType);
	puppet->ConnectSlots(m_drawSignal, m_updateSignal);
	puppet->SetEnvList(m_level->GetCollidableObjects());
	puppet->SetEnemyList(m_level->GetHostiles());
	puppet->SetDestructiblesList(m_level->GetDestructibles());

	//Add puppet to the list of networkPlayets
	m_networkIds.push_back(ms_networkIdCounter);
	m_networkPlayers.insert(std::pair<u8, PlayerController*>(ms_networkIdCounter, puppet));

	//Add puppet to the list of players
	m_players->push_back(puppet);
	m_level->GetHostiles()->push_back(puppet);

	ComposeNewClientMessage(ms_networkIdCounter, playerType);
	ms_networkIdCounter++;
}

void GameServer::HandleConnectionDrop(u8 clientId)
{	
	u8 puppetId = m_networkIds[clientId];
	m_players->remove(m_networkPlayers[puppetId]);
	delete m_networkPlayers[puppetId];
	m_networkPlayers.erase(m_networkPlayers.find(puppetId));
	m_networkIds.erase(m_networkIds.begin() + clientId);

	ComposeConnectionDropMessage(puppetId);	
}

void GameServer::ProcessMessage(u8* message)
{
	u8 messageType = (u8)message[0];
	switch (messageType)
	{
		//Server processes three types of messages:
		//0. new incoming connection
		//1. update messages from clients
		//2. sync message from clients

	case 0:
	{
		//TODO: the message should contain a player type
		//for now, just create a knight for all new players
		InitializePuppet(message[1]);
		break;
	}
	case 1:
	{
		u8 id = (u8)message[1];
		u8 playerStates = (u8)message[2];
		m_networkPlayers[id]->ReadState(playerStates);
		break;
	}
	case 2:
	{
		u8 id = (u8)message[1];
		CL_Point playerPosition;
		playerPosition.x = (((s16)message[2] << 8) | ((s16)message[3]));
		playerPosition.y = (((s16)message[4] << 8) | ((s16)message[5]));
		s32 currentHealth = (((s16)message[6] << 8) | ((s16)message[7]));
		m_networkPlayers[id]->SetPosition(playerPosition);
		m_networkPlayers[id]->SetStat(eCurrentLife, currentHealth);
	}
	}
}

void GameServer::ComposeConnectionDropMessage(u8 networkId)
{
	u8 message[2];
	message[0] = (u8)4;
	message[1] = (u8)networkId;
	MessageData packet;
	memcpy(packet.data, message, sizeof(message));
	packet.size = sizeof(message);
	m_outgoingMessages.push_back(packet);
}

void GameServer::ComposeIntroMessage(u8 networkId)
{
	u8 message[256];
	message[0] = (u8)0;
	message[1] = (u8)networkId;
	message[2] = (u8)((u8)m_players->size());
	u16 nextIndex = 3;
	GameObjects::iterator it = m_players->begin();
	for (; it != m_players->end(); ++it) {		
		if (it == m_players->begin())	{
			message[nextIndex] = (u8)0;
		} else {
			u8 temp = std::distance(m_players->begin(), it);
			message[nextIndex] = (u8)m_networkIds[temp - 1];
		}

		if (it == m_players->begin())	{
			message[nextIndex + 1] = (u8)dynamic_cast<Player*>((*it))->GetType();
		} else {
			message[nextIndex + 1] = (u8)dynamic_cast<PlayerController*>((*it))->GetType();
		}
		
		CL_Point position = (*it)->GetPosition();
		message[nextIndex + 2] = (u8)((position.x >> 8) & 0xFF);
		message[nextIndex + 3] = (u8)(position.x & 0xFF);
		message[nextIndex + 4] = (u8)((position.y >> 8) & 0xFF);
		message[nextIndex + 5] = (u8)(position.y & 0xFF);
		message[nextIndex + 6] = (u8)(((*it)->GetStat(eCurrentLife) >> 8) & 0xFF);
		message[nextIndex + 7] = (u8)((*it)->GetStat(eCurrentLife) & 0xFF);
		nextIndex += 8;
	}

	MessageData packet;
	memcpy(packet.data, message, nextIndex);
	packet.size = nextIndex;
	
	u8 clientId = m_networkIds.size();
	m_connection.SendToSingleClient(packet.data, packet.size, m_networkIds.size());
}

void GameServer::ComposeUpdateMessage()
{
	if (m_networkPlayers.empty()) {
		return;
	}

	u8 message[256];
	message[0] = (u8)1;
	message[1] = (u8)((u8)m_players->size());
	u16 nextIndex = 2;
	GameObjects::iterator it = m_players->begin();
	for (; it != m_players->end(); ++it) {
		if (it == m_players->begin()) {
			message[nextIndex] = (u8)0;
		} else {
			u8 temp = std::distance(m_players->begin(), it);
			message[nextIndex] = (u8)m_networkIds[temp - 1];
		}
		message[nextIndex + 1] = (u8)(dynamic_cast<Character*>(*it))->GetCurrentState();
		nextIndex += 2;
	}

	MessageData packet;
	memcpy(packet.data, message, nextIndex);
	packet.size = nextIndex;
	m_outgoingMessages.push_back(packet);
}

void GameServer::ComposeNewClientMessage(u8 networkId, u8 playerType)
{
	u8 message[3];
	message[0] = (u8)3;
	message[1] = (u8)networkId;
	message[2] = (u8)playerType;
	MessageData packet;
	memcpy(packet.data, message, sizeof(message));
	packet.size = sizeof(message);
	m_outgoingMessages.push_back(packet);
}

void GameServer::ComposeSyncMessage()
{
	if (m_networkPlayers.empty()) {
		return;
	}

	for (u8 i = 0; i < m_networkIds.size(); i++)
	{
		u32 rtt = m_connection.GetRoundTripTime(i);
		u8 message[512];
		message[0] = (u8)2;
		message[1] = (u8)0;
		message[2] = (u8)m_players->size();
		u16 nextIndex = 3;
		GameObjects::iterator it = m_players->begin();
		for (; it != m_players->end(); ++it) {		
			if (it == m_players->begin()) {
				message[nextIndex] = (u8)0;
			} else {
				u8 temp = std::distance(m_players->begin(), it);
				message[nextIndex] = (u8)m_networkIds[temp - 1];
			}
			CL_Point predictedPosition;
			predictedPosition.x = (*it)->GetPosition().x + static_cast<s32>((*it)->GetSpeed(eXCurrentSpeed)*rtt / 2);
			predictedPosition.y = (*it)->GetPosition().y + static_cast<s32>((*it)->GetSpeed(eYCurrentSpeed)*rtt / 2);
			message[nextIndex + 1] = (u8)((predictedPosition.x >> 8) & 0xFF);
			message[nextIndex + 2] = (u8)(predictedPosition.x & 0xFF);
			message[nextIndex + 3] = (u8)((predictedPosition.y >> 8) & 0xFF);
			message[nextIndex + 4] = (u8)(predictedPosition.y & 0xFF);
			message[nextIndex + 5] = (u8)(((*it)->GetStat(eCurrentLife) >> 8) & 0xFF);
			message[nextIndex + 6] = (u8)((*it)->GetStat(eCurrentLife) & 0xFF);
			nextIndex += 7;
		}
		m_connection.SendToSingleClient(message, nextIndex, i);
	}
}

void GameServer::Run()
{
	XmlParser parser("settings/Players.xml");
	PlayerType playerType = (PlayerType)parser.ReadInt("/PlayerData/PlayerOneType");
	u32 keybinds[4] = { CL_KEY_LEFT, CL_KEY_RIGHT, CL_KEY_UP, CL_KEY_J };

	Initialize();
	InitializeLevel();
	InitializePlayer(playerType, keybinds);
	InitializeNetwork();

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

		m_frameCounter++;

		m_incomingMessages.clear();
		m_outgoingMessages.clear();

		while (true) {
			MessageData packet;
			u32 bytes_read = m_connection.ReceivePacket(packet.data, sizeof(packet.data));
			if (bytes_read == 0) {
				break;
			}
			m_incomingMessages.push_back(packet);
		}

		std::vector<MessageData>::iterator it = m_incomingMessages.begin();
		for (; it != m_incomingMessages.end(); ++it) {
			ProcessMessage((*it).data);
		}

		//Update the game state
		m_level->Update();
		m_updateSignal.invoke(timeElapsedMs);
		m_drawSignal.invoke();

		// Flip the display, showing on the screen what we drew since last call to flip()
		m_window.flip(0);
		
		ComposeUpdateMessage();

		if ((m_frameCounter % 6) == 0) {
			ComposeSyncMessage();
		}
		
		if (m_outgoingMessages.size() > 0) {
			std::vector<MessageData>::iterator it = m_outgoingMessages.begin();
			for (; it != m_outgoingMessages.end(); ++it)
			{
				m_connection.SendPacket((*it).data, (*it).size);
			}
		}		

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