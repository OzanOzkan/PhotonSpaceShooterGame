#include <StdAfx.h>
#include "PhotonComponent.h"

static void RegisterPhotonComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPhotonComponent));
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPhotonComponent);

CPhotonComponent::CPhotonComponent()
	: m_photonUpdateFrameCount(60)
	, m_isPhotonConnected(false)
	, m_LoadBalancingClient(*this, L"55750f6f-b994-4c06-b2a6-1c5f21f2a89c", L"1.0", ExitGames::Photon::ConnectionProtocol::DEFAULT, true, ExitGames::LoadBalancing::RegionSelectionMode::BEST)
	, m_isRoomCreated(false)
	, m_isJoinedToRoom(false)
	, photon_send_message(0)
{
}

CPhotonComponent::~CPhotonComponent()
{
	m_LoadBalancingClient.disconnect();
}

void CPhotonComponent::Initialize()
{
	m_LoadBalancingClient.setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS));
	m_LoadBalancingClient.connect();

	if(!gEnv->pConsole->GetCVar("photon_send_message"))
		REGISTER_CVAR(photon_send_message, 0, VF_CHEAT, "");
}

void CPhotonComponent::ProcessEvent(SEntityEvent & event)
{
	if (event.event == ENTITY_EVENT_UPDATE)
	{
		if (m_photonUpdateFrameCount >= 60)
		{
			m_LoadBalancingClient.service();
			m_photonUpdateFrameCount = 0;


			if (gEnv->pConsole->GetCVar("photon_send_message")->GetFVal() == 1)
			{
				nByte eventCode = 1; // use distinct event codes to distinguish between different types of events (for example 'move', 'shoot', etc.)
				ExitGames::Common::Hashtable evData; // organize your payload data in any way you like as long as it is supported by Photons serialization				
				evData.put("x", GetEntity()->GetWorldPos().x);
				evData.put("y", GetEntity()->GetWorldPos().y);
				evData.put("z", GetEntity()->GetWorldPos().z);

				CMessage* a = new CMessage(10);
				char* payload = reinterpret_cast<char*>(&a);

				bool sendReliable = false; // send something reliable if it has to arrive everywhere
				m_LoadBalancingClient.opRaiseEvent(sendReliable, payload, eventCode);
			}

		}
		++m_photonUpdateFrameCount;
	}
	/*
	if (event.event == ENTITY_EVENT_UPDATE)
	{
		// If Photon service is not connected, nothing to do.
		if (!m_isPhotonConnected)
		{
			if (m_isPhotonConnected = m_LoadBalancingClient.connect(ExitGames::LoadBalancing::AuthenticationValues().setUserID(L"007"), L"ozan"))
			{
				CryLogAlways("[PhotonComponent]: Connected to Photon service. MS: %s", m_LoadBalancingClient.getMasterserverAddress().UTF8Representation().cstr());
				//if(m_LoadBalancingClient.opJoinLobby(L"mylobby"))
				//	CryLogAlways("[PhotonComponent]: Joined to lobby.");
			}
		}
		else
		{
			if (m_photonUpdateFrameCount == 300)
			{
				m_LoadBalancingClient.service();
				m_photonUpdateFrameCount = 0;
			}
			++m_photonUpdateFrameCount;

			if (!m_isJoinedToRoom)
			{
				if (m_isJoinedToRoom = m_LoadBalancingClient.opJoinOrCreateRoom(L"testroom"))
					CryLogAlways("[PhotonComponent]: Joined / created the room.");
			}
			else
			{
				ExitGames::LoadBalancing::MutableRoom& currentRoom = m_LoadBalancingClient.getCurrentlyJoinedRoom();
				CryLogAlways("Room: %s - Players: %d - Max: %d", currentRoom.getName().UTF8Representation().cstr(), currentRoom.getPlayerCount(), currentRoom.getMaxPlayers());
			}
		}
	}
	*/
	//if (!m_isRoomCreated)	
	//{ 
	//	if (m_isRoomCreated = m_LoadBalancingClient.opCreateRoom(L"testroom", ExitGames::LoadBalancing::RoomOptions().setMaxPlayers(5)))
	//		CryLogAlways("[PhotonComponent]: A new game room created.");
	//}

	//if (!m_isJoinedToRoom)
	//{
	//	if(m_isJoinedToRoom = m_LoadBalancingClient.opJoinOrCreateRoom(L"testroom"))
	//		CryLogAlways("[PhotonComponent]: Joined / created the room.");
	//}
	//else
	//{
	//	ExitGames::LoadBalancing::MutableRoom& currentRoom = m_LoadBalancingClient.getCurrentlyJoinedRoom();
	//	CryLogAlways("Room: %s - Players: %d - Max: %d", currentRoom.getName().UTF8Representation().cstr(), currentRoom.getPlayerCount(), currentRoom.getMaxPlayers());
	//}

	//Matchmaking();

	/*
	if (m_isRoomCreated)
	{
		if (m_photonTestEventSentCount == 300)
		{
			nByte eventCode = 1; // use distinct event codes to distinguish between different types of events (for example 'move', 'shoot', etc.)
			ExitGames::Common::Hashtable evData; // organize your payload data in any way you like as long as it is supported by Photons serialization
			evData.put("testevent", 1);
			bool sendReliable = false; // send something reliable if it has to arrive everywhere
			if (m_LoadBalancingClient.opRaiseEvent(sendReliable, evData, eventCode))
			{
				CryLogAlways("[PhotonComponent]: opRaiseEvent: true.");
			}

			m_photonTestEventSentCount = 0;
		}

		++m_photonTestEventSentCount;
	}
	*/
}

void CPhotonComponent::Matchmaking()
{
	ExitGames::Common::JVector<ExitGames::LoadBalancing::Room*> roomList = m_LoadBalancingClient.getRoomList();

	CryLogAlways("Room count: %d", roomList.getSize());

	// Join to an existing room
	if (roomList.getSize() > 0)
	{
		CryLogAlways("First room: %s", roomList.getFirstElement()->getName().toString(true).cstr());

		/*
		m_isRoomCreated = true;

		if (m_isJoinedToRoom = m_LoadBalancingClient.opJoinRoom(roomList.getFirstElement()->getName()))
		{
			CryLogAlways("[PhotonComponent]: Joined to the game room: %s", roomList.getFirstElement()->getName().toString(true).cstr());
			return;
		}
		*/
	}

	/*
	CryWarning(EValidatorModule::VALIDATOR_MODULE_GAME, EValidatorSeverity::VALIDATOR_WARNING, "[PhotonComponent]: Could not joined to the game room.");
	*/
}
