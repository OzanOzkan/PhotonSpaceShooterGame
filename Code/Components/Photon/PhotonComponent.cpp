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
	: m_photonUpdateFrameCount(100)
	, m_LoadBalancingClient(m_PhotonListener, appID, appVersion)
	, m_isRoomCreated(false)
{
}

CPhotonComponent::~CPhotonComponent()
{
	m_LoadBalancingClient.disconnect();
}

void CPhotonComponent::Initialize()
{
	if (!m_LoadBalancingClient.connect())
		CryWarning(EValidatorModule::VALIDATOR_MODULE_GAME, EValidatorSeverity::VALIDATOR_WARNING, "[PhotonComponent]: Could not connect to Photon service.");
	else
		CryLogAlways("[PhotonComponent]: Connected to Photon service.");
}

void CPhotonComponent::ProcessEvent(SEntityEvent & event)
{
	if (m_photonUpdateFrameCount == 100)
	{
		m_LoadBalancingClient.service();
		
		m_photonUpdateFrameCount = 0;
	}

	++m_photonUpdateFrameCount;

	if (!m_isRoomCreated)
	{
		if(m_isRoomCreated = m_LoadBalancingClient.opCreateRoom(L"testroom", ExitGames::LoadBalancing::RoomOptions().setMaxPlayers(5)))
			CryLogAlways("[PhotonComponent]: Game room created.");
		else
			CryWarning(EValidatorModule::VALIDATOR_MODULE_GAME, EValidatorSeverity::VALIDATOR_WARNING, "[PhotonComponent]: Could not create the game room.");
	}

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
}
