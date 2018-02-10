#include <StdAfx.h>
#include "PhotonClientComponent.h"

#include "../Player.h"

static void RegisterPhotonClientComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPhotonClientComponent));
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPhotonClientComponent);

CPhotonClientComponent::CPhotonClientComponent()
	: m_localPlayerId(0)
	, m_isPhotonConnected(false)
	, m_LoadBalancingClient(*this, L"55750f6f-b994-4c06-b2a6-1c5f21f2a89c", L"1.0")
	, m_firstInit(true)
	, m_firstRoomJoin(true)
	, m_playerCount(0)
{
}

CPhotonClientComponent::~CPhotonClientComponent()
{
	m_LoadBalancingClient.disconnect();
}

//////////////////////////////////////////////////////////////
// IEntityComponent											//
//////////////////////////////////////////////////////////////
void CPhotonClientComponent::Initialize()
{
	// Debug texts to screen
	m_pDebugDrawComponent = GetEntity()->GetOrCreateComponent<Cry::DefaultComponents::CDebugDrawComponent>();

	// Photon debug
	m_LoadBalancingClient.setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS));

	// Photon connection to the cloud
	ExitGames::Common::JString username(gEnv->pSystem->GetUserName());
	m_LoadBalancingClient.selectRegion("eu");
	m_LoadBalancingClient.connect(ExitGames::LoadBalancing::AuthenticationValues(), username);
}

void CPhotonClientComponent::ProcessEvent(SEntityEvent & event)
{
	if (event.event == ENTITY_EVENT_UPDATE)
	{
		if (m_firstInit)
		{
			CryLogAlways("[PhotonClientComponent]: Starting to registering Photon entities to the system.");

			// Get all entities which have Photon Entity Component
			IEntityIt* entityIt = gEnv->pEntitySystem->GetEntityIterator();
			while (!entityIt->IsEnd())
			{
				IEntity* pEnt = entityIt->Next();
				if (CPhotonEntityComponent* pPhotonEntComponent = pEnt->GetComponent<CPhotonEntityComponent>())
				{
					m_localNetworkedEntities.insert(std::make_pair(pEnt->GetId(), pPhotonEntComponent));
				}
			}

			m_firstInit = false;

			CryLogAlways("[PhotonClientComponent]: %d Photon entities registered to the system.", m_localNetworkedEntities.size());
		}

		m_LoadBalancingClient.service();

		for (auto currentEntity : m_localNetworkedEntities)
		{
			SPhotonSerializedData* serializedData = currentEntity.second->GetSerializedData();

			nByte* payload = reinterpret_cast<nByte*>(serializedData);
			m_LoadBalancingClient.opRaiseEvent(true, payload, sizeof(SPhotonSerializedData), 1);
		}

		UpdateInfoTexts();
	}
}

void CPhotonClientComponent::UpdateInfoTexts()
{
	Schematyc::CSharedString text;

	if (m_isPhotonConnected)
	{
		text.append("Connected. Player: ");
		text.append(gEnv->pSystem->GetUserName());

		text.append("\n");
		text.append("Players: ");
		text.append(std::to_string(m_playerCount).c_str());
	}
	else
		text.append("Not connected.");

	m_pDebugDrawComponent->DrawText(text, 1, 1, 1.2f, Vec3(1, 1, 1), 0.06f);
}

void CPhotonClientComponent::CreateRemotePlayer(int playerId, const char* playerName)
{
	SEntitySpawnParams params;
	params.sName = "otherPlayer";
	params.vPosition = Vec3(10, 10, 10);
	params.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();

	if (IEntity* pEntity = gEnv->pEntitySystem->SpawnEntity(params))
	{
		pEntity->GetOrCreateComponent<CEnemySmallShip>()->DrawPlayerName(playerName);
		m_remoteNetworkedEntities[playerId] = pEntity;

		CryLogAlways("[PhotonClientComponent]: CPhotonClientComponent::joinRoomEventAction: Connected player entity created.");
	}
	else
	{
		CryWarning(EValidatorModule::VALIDATOR_MODULE_NETWORK, EValidatorSeverity::VALIDATOR_ERROR,
			"[PhotonClientComponent]: CPhotonClientComponent::joinRoomEventAction: Connected player entity cannot created.");
	}
}

//////////////////////////////////////////////////////////////
// ExitGames::LoadBalancing::Listener						//
//////////////////////////////////////////////////////////////
void CPhotonClientComponent::debugReturn(int debugLevel, const ExitGames::Common::JString & string)
{
	CryLogAlways("[PhotonClientComponent]: CPhotonClientComponent::debugReturn: DebugLevel: %d, String: %s", debugLevel, string.UTF8Representation().cstr());
}

void CPhotonClientComponent::connectionErrorReturn(int errorCode)
{
	CryWarning(EValidatorModule::VALIDATOR_MODULE_NETWORK, EValidatorSeverity::VALIDATOR_ERROR,
		"[PhotonClientComponent]: CPhotonClientComponent::connectionErrorReturn: Error Code: %d", errorCode);
}

void CPhotonClientComponent::clientErrorReturn(int errorCode)
{
	CryWarning(EValidatorModule::VALIDATOR_MODULE_NETWORK, EValidatorSeverity::VALIDATOR_ERROR,
		"[PhotonClientComponent]: CPhotonClientComponent::connectionErrorReturn: Error Code: %d", errorCode);
}

void CPhotonClientComponent::warningReturn(int warningCode)
{
	CryWarning(EValidatorModule::VALIDATOR_MODULE_NETWORK, EValidatorSeverity::VALIDATOR_WARNING,
		"[PhotonClientComponent]: CPhotonClientComponent::warningReturn: Warning Code: %d", warningCode);
}

void CPhotonClientComponent::serverErrorReturn(int errorCode)
{
	CryWarning(EValidatorModule::VALIDATOR_MODULE_NETWORK, EValidatorSeverity::VALIDATOR_ERROR,
		"[PhotonClientComponent]: CPhotonClientComponent::serverErrorReturn: Warning Code: %d", errorCode);
}

void CPhotonClientComponent::joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player & player)
{
	CryLogAlways("[PhotonClientComponent]: CPhotonClientComponent::joinRoomEventAction: PlayerNr: %d - Players: %d - PlayerName: %s", playerNr, playernrs.getSize(), player.getName().UTF8Representation().cstr());

	m_playerCount = playernrs.getSize();

	// Create already connected players in local client.
	if (m_firstRoomJoin)
	{
		for (int i = 0; i < playernrs.getSize(); ++i)
		{
			CreateRemotePlayer(playernrs[i], player.getName().UTF8Representation().cstr());
		}

		m_firstRoomJoin = false;
	}

	// Create new connected players in local client.
	if (playerNr != m_localPlayerId)
	{
		CreateRemotePlayer(playerNr, player.getName().UTF8Representation().cstr());
	}
}

void CPhotonClientComponent::leaveRoomEventAction(int playerNr, bool isInactive)
{
	CryLogAlways("[PhotonClientComponent]: CPhotonClientComponent::leaveRoomEventAction: Player: %d - isInactive: %d", playerNr, (int)isInactive);

	gEnv->pEntitySystem->RemoveEntity(m_remoteNetworkedEntities[playerNr]->GetId());

	--m_playerCount;
}

void CPhotonClientComponent::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object & eventContent)
{
	CryLogAlways("[PhotonClientComponent]: CPhotonClientComponent::customEventAction: BROADCAST RECEIVED! playerNr: %d, eventCode: %d, eventContent: %s", playerNr, eventCode, eventContent.toString(true).UTF8Representation().cstr());

	if (playerNr != m_localPlayerId)
	{
		nByte* content = ExitGames::Common::ValueObject<nByte*>(eventContent).getDataCopy();
		SPhotonSerializedData* data = reinterpret_cast<SPhotonSerializedData*>(content);

		if (IEntity* pCurrentEntity = m_remoteNetworkedEntities[playerNr])
		{
			// Player Position
			Matrix34 newPos = data->worldTM;
			Vec3 pos = pCurrentEntity->GetPos();
			Interpolate(pos, newPos.GetTranslation(), 5.f, gEnv->pTimer->GetFrameTime());
			newPos.SetTranslation(pos);
			pCurrentEntity->SetWorldTM(newPos);

			// Firing
			if (data->isFiring)
			{
				pCurrentEntity->GetComponent<CEnemySmallShip>()->Fire();
			}
		}
		else
		{
			CryWarning(EValidatorModule::VALIDATOR_MODULE_NETWORK, EValidatorSeverity::VALIDATOR_ERROR,
				"[PhotonClientComponent]: CPhotonClientComponent::customEventAction: Remote player entity update unsuccessfull. PlayerNr: %d - TotalPlayers: %d", playerNr, m_remoteNetworkedEntities.size());
		}
	}
}

void CPhotonClientComponent::connectReturn(int errorCode, const ExitGames::Common::JString & errorString, const ExitGames::Common::JString & cluster)
{
	CryLogAlways("[PhotonClientComponent]: CPhotonClientComponent::connectReturn: ErrorCode: %d, ErrorString: %s, Cluster: %s", errorCode, errorString.UTF8Representation().cstr(), cluster.UTF8Representation().cstr());
	m_isPhotonConnected = true;

	m_LoadBalancingClient.opJoinOrCreateRoom(L"myroom");
}

void CPhotonClientComponent::disconnectReturn(void)
{
	CryLogAlways("[PhotonClientComponent]: CPhotonClientComponent::disconnectReturn");
}

void CPhotonClientComponent::createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable & roomProperties, const ExitGames::Common::Hashtable & playerProperties, int errorCode, const ExitGames::Common::JString & errorString)
{
	CryLogAlways("[PhotonClientComponent]: CPhotonClientComponent::createRoomReturn: PlayerNr: %d, ErrorCode: %d", localPlayerNr, errorCode);
	m_localPlayerId = localPlayerNr;
}

void CPhotonClientComponent::joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable & roomProperties, const ExitGames::Common::Hashtable & playerProperties, int errorCode, const ExitGames::Common::JString & errorString)
{
	CryLogAlways("[PhotonClientComponent]: CPhotonClientComponent::joinOrCreateRoomReturn: PlayerNr: %d, ErrorCode: %d", localPlayerNr, errorCode);
	m_localPlayerId = localPlayerNr;
}

void CPhotonClientComponent::joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable & roomProperties, const ExitGames::Common::Hashtable & playerProperties, int errorCode, const ExitGames::Common::JString & errorString)
{
	CryLogAlways("[PhotonClientComponent]: CPhotonClientComponent::joinRoomReturn: PlayerNr: %d, ErrorCode: %d", localPlayerNr, errorCode);
	m_localPlayerId = localPlayerNr;
}

void CPhotonClientComponent::joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable & roomProperties, const ExitGames::Common::Hashtable & playerProperties, int errorCode, const ExitGames::Common::JString & errorString)
{
	CryLogAlways("[PhotonClientComponent]: CPhotonClientComponent::joinRandomRoomReturn");
}

void CPhotonClientComponent::leaveRoomReturn(int errorCode, const ExitGames::Common::JString & errorString)
{
	CryLogAlways("[PhotonClientComponent]: CPhotonClientComponent::leaveRoomReturn");
}

void CPhotonClientComponent::joinLobbyReturn(void)
{
	CryLogAlways("[PhotonClientComponent]: CPhotonClientComponent::joinLobbyReturn");
}

void CPhotonClientComponent::leaveLobbyReturn(void)
{
	CryLogAlways("[PhotonClientComponent]: CPhotonClientComponent::leaveLobbyReturn");
}