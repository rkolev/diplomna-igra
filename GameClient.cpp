#include "GameClient.h"

void GameClient::InitializeNetwork()
{
	InitializeSockets();
	m_connection = ClientConnection(ProtocolId, TimeOut);
	m_connection.ConnectSlots(m_updateSignal);
	m_connection.Start(ClientPort);	
}

void GameClient::InitializePuppet(u16 playerType, u8 networkId, CL_Point spawnPoint)
{
	s8 character[32];
	strcpy(character, m_playerTypes[(PlayerType)playerType].c_str());
	PlayerController* puppet = new PlayerController(&m_gc, m_resources, spawnPoint.x, spawnPoint.y, character, (PlayerType)playerType);
	puppet->ConnectSlots(m_drawSignal, m_updateSignal);
	puppet->SetEnvList(m_level->GetCollidableObjects());
	puppet->SetEnemyList(m_level->GetHostiles());
	puppet->SetDestructiblesList(m_level->GetDestructibles());

	//Add puppet to the list of networkPlayets
	m_networkPlayers.insert(std::pair<int, PlayerController*>(networkId, puppet));

	//Add puppet to the list of players
	m_players->push_back(puppet);
	m_level->GetHostiles()->push_back(puppet);
}

void GameClient::ProcessMessage(u8* message)
{
	//Client processes four types of messages:
	//0. current game state from server (when client first connects)
	//1. update messages from server for other players' states
	//2. player state/position synchronization
	//3. new connection to server
	//3. client disconnected from server

	u8 messageType = (u8)message[0];
	switch (messageType)
	{
	case 0:
	{		
		m_networkId = (u8)message[1];
		u8 playerCount = (u8)message[2];
		u32 nextIndex = 3;
		for (u32 i = 0; i < playerCount; i++) {
			u8 puppetId = (u8)message[nextIndex];
			u8 playerType = (u8)message[nextIndex + 1];
			CL_Point playerPosition;
			playerPosition.x = (((s16)message[nextIndex + 2] << 8) | ((s16)message[nextIndex + 3]));
			playerPosition.y = (((s16)message[nextIndex + 4] << 8) | ((s16)message[nextIndex + 5]));
			s32 currentHealth = (((s16)message[nextIndex + 6] << 8) | ((s16)message[nextIndex + 7]));
			InitializePuppet(playerType, puppetId, playerPosition);
			m_networkPlayers[puppetId]->SetStat(eCurrentLife, currentHealth);
			nextIndex += 8;
		}
		break;
	}
	case 1:
	{
		u8 playerCount = (u8)message[1];
		u32 nextIndex = 2;
		for (u32 i = 0; i < playerCount; i++) {
			u8 id = (u8)message[nextIndex];
			if (id == m_networkId) {
				//ignore update message with data about this client
				nextIndex += 2;
				continue;
			} else {
				u8 playerStates = (u8)message[nextIndex+1];
				m_networkPlayers[id]->ReadState(playerStates);
			}
			nextIndex += 2;
		}
		break;
	}
	case 2:
	{
		bool forceUpdate = (message[1] & 0x01) == 0x01;
		u8 playerCount = (u8)message[2];
		u32 nextIndex = 3;
		for (u32 i = 0; i < playerCount; i++) {
			u8 id = (u8)message[nextIndex];
			CL_Point position;
			position.x = (((s16)message[nextIndex + 1] << 8) | ((s16)message[nextIndex + 2]));
			position.y = (((s16)message[nextIndex + 3] << 8) | ((s16)message[nextIndex + 4]));
			s32 currentHealth = (((s16)message[nextIndex + 5] << 8) | ((s16)message[nextIndex + 6]));
			if (id == m_networkId) {
				if (forceUpdate)	{
					(*m_players->begin())->SetPosition(position);
					(*m_players->begin())->SetStat(eCurrentLife, currentHealth);
				}
			} else {
				m_networkPlayers[id]->SetPosition(position);
				m_networkPlayers[id]->SetStat(eCurrentLife, currentHealth);
			}
			nextIndex += 7;
		}		
		break;
	}
	case 3:
	{
		u8 puppetId = (u8)message[1];
		if (puppetId == m_networkId) {
			break;
		}
		u8 playerType = (u8)message[2];
		InitializePuppet(playerType, puppetId, m_spawnPoint);
		break;
	}
	case 4:
	{
		u8 puppetId = (u8)message[1];
		m_players->remove(m_networkPlayers[puppetId]);
		delete m_networkPlayers[puppetId];
		m_networkPlayers.erase(m_networkPlayers.find(puppetId));
		break;
	}
	}
}

void GameClient::ComposeIntroMessage()
{
	u8 message[2];
	message[0] = (u8)0;
	message[1] = (u8)dynamic_cast<Player*>((*m_players->begin()))->GetType(); //Player Type

	MessageData packet;
	memcpy(packet.data, message, sizeof(message));
	packet.size = sizeof(message);

	m_connection.SendPacket(packet.data, packet.size);
}

void GameClient::ComposeUpdateMessage()
{
	u8 message[3];
	message[0] = (u8)1;
	message[1] = (u8)m_networkId;
	message[2] = (u8)(dynamic_cast<Player*>(*m_players->begin())->GetCurrentState());

	MessageData packet;
	memcpy(packet.data, message, sizeof(message));
	packet.size = sizeof(message);

	m_outgoingMessages.push_back(packet);
}

void GameClient::ComposeSyncMessage()
{	
	u32 rtt = m_connection.GetRoundTripTime();
	u8 message[512];
	message[0] = (u8)2;
	message[1] = m_networkId;
	CL_Point predictedPosition;
	predictedPosition.x = (*m_players->begin())->GetPosition().x + static_cast<int>((*m_players->begin())->GetSpeed(eXCurrentSpeed)*rtt / 2);
	predictedPosition.y = (*m_players->begin())->GetPosition().y + static_cast<int>((*m_players->begin())->GetSpeed(eYCurrentSpeed)*rtt / 2);
	message[2] = (u8)((predictedPosition.x >> 8) & 0xFF);
	message[3] = (u8)(predictedPosition.x & 0xFF);
	message[4] = (u8)((predictedPosition.y >> 8) & 0xFF);
	message[5] = (u8)(predictedPosition.y & 0xFF);
	message[6] = (u8)(((*m_players->begin())->GetStat(eCurrentLife) >> 8) & 0xFF);
	message[7] = (u8)((*m_players->begin())->GetStat(eCurrentLife) & 0xFF);
		
	MessageData packet;
	memcpy(packet.data, message, sizeof(message));
	packet.size = sizeof(message);

	m_outgoingMessages.push_back(packet);
}

void GameClient::Run()
{
	XmlParser parser("settings/Players.xml");
	XmlParser addressParser("settings/NetworkSettings.xml");
	PlayerType playerType = (PlayerType)parser.ReadInt("/PlayerData/PlayerOneType");
	u32 keybinds[4] = { CL_KEY_LEFT, CL_KEY_RIGHT, CL_KEY_UP, CL_KEY_J };
	u32 a, b, c, d, port;
	a = addressParser.ReadIntAttr("/NetworkSettings/ServerAddress", "a");
	b = addressParser.ReadIntAttr("/NetworkSettings/ServerAddress", "b");
	c = addressParser.ReadIntAttr("/NetworkSettings/ServerAddress", "c");
	d = addressParser.ReadIntAttr("/NetworkSettings/ServerAddress", "d");
	port = addressParser.ReadIntAttr("/NetworkSettings/ServerAddress", "port");

	Initialize();
	InitializeLevel();
	InitializePlayer(playerType, keybinds);
	InitializeNetwork();
	Address server(a, b, c, d, port);
	m_connection.Connect(server);
	ComposeIntroMessage();
	while (m_connection.GetState() == eConnecting) {
		while (true) {
			MessageData packet;
			int bytes_read = m_connection.ReceivePacket(packet.data, sizeof(packet.data));
			if (bytes_read == 0) {
				break;
			}
			if ((u8)packet.data[0] == 0) {
				ProcessMessage(packet.data);
			}
		}
		m_updateSignal.invoke(16);
		CL_System::sleep(16);
	}

	s32 timeElapsedMs, lastTime, currentTime, waitTime;
	currentTime = lastTime = CL_System::get_time();

	// Main game loop
	while (!m_quit)
	{

		if (m_connection.GetState() == eDisconnected) {
			m_quit = true;
		}
		//Calculate the elapsed time since the beginning of the last frame
		currentTime = CL_System::get_time();
		timeElapsedMs = currentTime - lastTime;
		if (timeElapsedMs > 50) {
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
			std::vector<MessageData>::iterator it = m_incomingMessages.begin();
			for (; it != m_outgoingMessages.end(); ++it) {
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