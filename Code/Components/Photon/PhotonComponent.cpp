#include <StdAfx.h>
#include "PhotonComponent.h"

#include "../Player.h"

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

//////////////////////////////////////////////////////////////
// IEntityComponent											//
//////////////////////////////////////////////////////////////
void CPhotonComponent::Initialize()
{
	m_LoadBalancingClient.setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS));

	ExitGames::Common::JString username(gEnv->pSystem->GetUserName());
	m_LoadBalancingClient.connect(ExitGames::LoadBalancing::AuthenticationValues(), username);

	if(!gEnv->pConsole->GetCVar("photon_send_message"))
		REGISTER_CVAR(photon_send_message, 0, VF_CHEAT, "");
}

void CPhotonComponent::ProcessEvent(SEntityEvent & event)
{
	if (event.event == ENTITY_EVENT_UPDATE)
	{
		//if (m_photonUpdateFrameCount >= 10)
		//{
			m_LoadBalancingClient.service();
			m_photonUpdateFrameCount = 0;

			if (gEnv->pConsole->GetCVar("photon_send_message")->GetFVal() == 1)
			{
				nByte eventCode = 1; // use distinct event codes to distinguish between different types of events (for example 'move', 'shoot', etc.)
				ExitGames::Common::Hashtable evData; // organize your payload data in any way you like as long as it is supported by Photons serialization				

				IEntity* pShip = gEnv->pEntitySystem->FindEntityByName("PlayerShip");
				CMessage* a = new CMessage(pShip->GetWorldTM());

				nByte* payload = static_cast<nByte*>(static_cast<void*>(a));
				auto size = sizeof(CMessage);

				bool sendReliable = true; // send something reliable if it has to arrive everywhere
				m_LoadBalancingClient.opRaiseEvent(sendReliable, payload, size * 2, eventCode);
								
				CryLogAlways("CPhotonComponent::ProcessEvent: %f - %f - %f", a->worldTM.GetTranslation().x, a->worldTM.GetTranslation().y, a->worldTM.GetTranslation().z);
			}

		//}
		++m_photonUpdateFrameCount;
	}
}

//////////////////////////////////////////////////////////////
// ExitGames::LoadBalancing::Listener						//
//////////////////////////////////////////////////////////////
void CPhotonComponent::debugReturn(int debugLevel, const ExitGames::Common::JString & string)
{
	CryLogAlways("[PhotonComponent]: CPhotonListener::debugReturn: DebugLevel: %d, String: %s", debugLevel, string.UTF8Representation().cstr());
}

void CPhotonComponent::connectionErrorReturn(int errorCode)
{
	CryWarning(EValidatorModule::VALIDATOR_MODULE_NETWORK, EValidatorSeverity::VALIDATOR_ERROR,
		"[PhotonComponent]: CPhotonListener::connectionErrorReturn: Error Code: %d", errorCode);
}

void CPhotonComponent::clientErrorReturn(int errorCode)
{
	CryWarning(EValidatorModule::VALIDATOR_MODULE_NETWORK, EValidatorSeverity::VALIDATOR_ERROR,
		"[PhotonComponent]: CPhotonListener::connectionErrorReturn: Error Code: %d", errorCode);
}

void CPhotonComponent::warningReturn(int warningCode)
{
	CryWarning(EValidatorModule::VALIDATOR_MODULE_NETWORK, EValidatorSeverity::VALIDATOR_WARNING,
		"[PhotonComponent]: CPhotonListener::warningReturn: Warning Code: %d", warningCode);
}

void CPhotonComponent::serverErrorReturn(int errorCode)
{
	CryWarning(EValidatorModule::VALIDATOR_MODULE_NETWORK, EValidatorSeverity::VALIDATOR_ERROR,
		"[PhotonComponent]: CPhotonListener::serverErrorReturn: Warning Code: %d", errorCode);
}

void CPhotonComponent::joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player & player)
{
	CryLogAlways("[PhotonComponent]: CPhotonListener::joinRoomEventAction: PlayerNr: %d - Players: %d - PlayerName: %s", playerNr, playernrs.getSize(), player.getName().UTF8Representation().cstr());

	// Test purposes
	if (playerNr == 2)
	{
		SEntitySpawnParams params;
		params.sName = "otherPlayer";
		params.vPosition = Vec3(10, 10, 10);
		params.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();

		pOtherTestEntity = gEnv->pEntitySystem->SpawnEntity(params);
		pOtherTestEntity->GetOrCreateComponent<CEnemySmallShip>();

		CryLogAlways("[PhotonComponent]: CPhotonListener::joinRoomEventAction: Connected player entity created.");
	}
}

void CPhotonComponent::leaveRoomEventAction(int playerNr, bool isInactive)
{
	CryLogAlways("[PhotonComponent]: CPhotonListener::leaveRoomEventAction");
}

void CPhotonComponent::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object & eventContent)
{
	CryLogAlways("[PhotonComponent]: CPhotonListener::customEventAction: BROADCAST RECEIVED! playerNr: %d, eventCode: %d, eventContent: %s", playerNr, eventCode, eventContent.toString(true).UTF8Representation().cstr());

	// Test purposes
	if (playerNr == 2)
	{
		nByte* content = ExitGames::Common::ValueObject<nByte*>(eventContent).getDataCopy();
		CMessage* message = static_cast<CMessage*>(static_cast<void*>(content));
		CryLogAlways("Message: X: %f - Y: %f - Z: %f", message->worldTM.GetTranslation().x, message->worldTM.GetTranslation().y, message->worldTM.GetTranslation().z);

		Matrix34 newPos = message->worldTM;
		Vec3 pos = pOtherTestEntity->GetPos();
		Interpolate(pos, newPos.GetTranslation(), 5.f, gEnv->pTimer->GetFrameTime());
		newPos.SetTranslation(pos);
		pOtherTestEntity->SetWorldTM(newPos);

		//CryLogAlways("Other entity: %d - X: %f - Y: %f - Z: %f", pOtherTestEntity->GetId(), pOtherTestEntity->GetPos().x, pOtherTestEntity->GetPos().y, pOtherTestEntity->GetPos().z);
	}
}

void CPhotonComponent::connectReturn(int errorCode, const ExitGames::Common::JString & errorString, const ExitGames::Common::JString & cluster)
{
	CryLogAlways("[PhotonComponent]: CPhotonListener::connectReturn: ErrorCode: %d, ErrorString: %s, Cluster: %s", errorCode, errorString.UTF8Representation().cstr(), cluster.UTF8Representation().cstr());
	m_isPhotonConnected = true;

	m_LoadBalancingClient.opJoinOrCreateRoom(L"myroom");
}

void CPhotonComponent::disconnectReturn(void)
{
	CryLogAlways("[PhotonComponent]: CPhotonListener::disconnectReturn");
}

void CPhotonComponent::createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable & roomProperties, const ExitGames::Common::Hashtable & playerProperties, int errorCode, const ExitGames::Common::JString & errorString)
{
	CryLogAlways("[PhotonComponent]: CPhotonListener::createRoomReturn");
}

void CPhotonComponent::joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable & roomProperties, const ExitGames::Common::Hashtable & playerProperties, int errorCode, const ExitGames::Common::JString & errorString)
{
	CryLogAlways("[PhotonComponent]: CPhotonListener::createRoomReturn");
}

void CPhotonComponent::joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable & roomProperties, const ExitGames::Common::Hashtable & playerProperties, int errorCode, const ExitGames::Common::JString & errorString)
{
	CryLogAlways("[PhotonComponent]: CPhotonListener::joinRoomReturn");
}

void CPhotonComponent::joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable & roomProperties, const ExitGames::Common::Hashtable & playerProperties, int errorCode, const ExitGames::Common::JString & errorString)
{
	CryLogAlways("[PhotonComponent]: CPhotonListener::joinRandomRoomReturn");
}

void CPhotonComponent::leaveRoomReturn(int errorCode, const ExitGames::Common::JString & errorString)
{
	CryLogAlways("[PhotonComponent]: CPhotonListener::leaveRoomReturn");
}

void CPhotonComponent::joinLobbyReturn(void)
{
	CryLogAlways("[PhotonComponent]: CPhotonListener::joinLobbyReturn");
}

void CPhotonComponent::leaveLobbyReturn(void)
{
	CryLogAlways("[PhotonComponent]: CPhotonListener::leaveLobbyReturn");
}