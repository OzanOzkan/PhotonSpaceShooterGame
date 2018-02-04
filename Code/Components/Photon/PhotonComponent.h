#pragma once

#include <CrySchematyc/CoreAPI.h>
#include <CryEntitySystem/IEntityComponent.h>

// Necessary undefs for Photon. 
// Definitions overlapping with CRYENGINE.
#undef alloc
#undef malloc
#undef free
#undef realloc
#undef calloc
#include "Photon-cpp/inc/PhotonPeer.h"
#include "Common-cpp/inc/Common.h"
#include "LoadBalancing-cpp/inc/Listener.h"
#include "LoadBalancing-cpp/inc/Client.h"

static const ExitGames::Common::JString appID = L"55750f6f-b994-4c06-b2a6-1c5f21f2a89c";
static const ExitGames::Common::JString appVersion = L"1.0";

class CPhotonListener : public ExitGames::LoadBalancing::Listener
{
public:
	CPhotonListener() {};
	virtual ~CPhotonListener() {};

	// ExitGames::LoadBalancing::Listener
	virtual void debugReturn(int debugLevel, const ExitGames::Common::JString & string) override 
	{ 
		CryLogAlways("[PhotonComponent]: CPhotonListener::debugReturn: DebugLevel: %d, String: %s", debugLevel, string.toString(true).cstr()); 
	}
	virtual void connectionErrorReturn(int errorCode) override { CryLogAlways("[PhotonComponent]: CPhotonListener::connectionErrorReturn"); }
	virtual void clientErrorReturn(int errorCode) override { CryLogAlways("[PhotonComponent]: CPhotonListener::clientErrorReturn"); }
	virtual void warningReturn(int warningCode) override { CryLogAlways("[PhotonComponent]: CPhotonListener::warningReturn"); }
	virtual void serverErrorReturn(int errorCode) override { CryLogAlways("[PhotonComponent]: CPhotonListener::serverErrorReturn"); }
	virtual void joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player & player) override { CryLogAlways("[PhotonComponent]: CPhotonListener::joinRoomEventAction"); }
	virtual void leaveRoomEventAction(int playerNr, bool isInactive) override { CryLogAlways("[PhotonComponent]: CPhotonListener::leaveRoomEventAction"); }
	virtual void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object & eventContent) override 
	{
		CryLogAlways("[PhotonComponent]: CPhotonListener::customEventAction: playerNr: %d, eventCode: %d, eventContent: %s", playerNr, eventCode, eventContent.toString(true).cstr());
	}
	virtual void connectReturn(int errorCode, const ExitGames::Common::JString & errorString, const ExitGames::Common::JString & cluster) override 
	{ 
		CryLogAlways("[PhotonComponent]: CPhotonListener::connectReturn: ErrorCode: %d, ErrorString: %s, Cluster: %s", errorCode, errorString.toString(true).cstr(), cluster.toString(true).cstr()); 
	}
	virtual void disconnectReturn(void) override { CryLogAlways("[PhotonComponent]: CPhotonListener::disconnectReturn"); };
	virtual void createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable & roomProperties, const ExitGames::Common::Hashtable & playerProperties, int errorCode, const ExitGames::Common::JString & errorString) override { CryLogAlways("[PhotonComponent]: CPhotonListener::disconnectReturn"); };
	virtual void joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable & roomProperties, const ExitGames::Common::Hashtable & playerProperties, int errorCode, const ExitGames::Common::JString & errorString) override { CryLogAlways("[PhotonComponent]: CPhotonListener::createRoomReturn"); };
	virtual void joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable & roomProperties, const ExitGames::Common::Hashtable & playerProperties, int errorCode, const ExitGames::Common::JString & errorString) override { CryLogAlways("[PhotonComponent]: CPhotonListener::joinRoomReturn"); };
	virtual void joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable & roomProperties, const ExitGames::Common::Hashtable & playerProperties, int errorCode, const ExitGames::Common::JString & errorString) override { CryLogAlways("[PhotonComponent]: CPhotonListener::joinRandomRoomReturn"); };
	virtual void leaveRoomReturn(int errorCode, const ExitGames::Common::JString & errorString) override { CryLogAlways("[PhotonComponent]: CPhotonListener::leaveRoomReturn"); };
	virtual void joinLobbyReturn(void) override { CryLogAlways("[PhotonComponent]: CPhotonListener::joinLobbyReturn"); };
	virtual void leaveLobbyReturn(void) override { CryLogAlways("[PhotonComponent]: CPhotonListener::leaveLobbyReturn"); };
	// ~ExitGames::LoadBalancing::Listener
};

class CPhotonComponent : public IEntityComponent
{
public:
	CPhotonComponent();
	virtual ~CPhotonComponent();

	// IEntityComponent
	virtual void Initialize() override;
	virtual uint64 GetEventMask() const override { return BIT64(ENTITY_EVENT_UPDATE); }
	virtual void ProcessEvent(SEntityEvent& event) override;
	// ~IEntityComponent

	static void ReflectType(Schematyc::CTypeDesc<CPhotonComponent>& desc)
	{
		desc.SetGUID("{C687992A-AB4D-4F98-B134-5049C31693B5}"_cry_guid);
		desc.SetEditorCategory("MyComponents");
		desc.SetLabel("PhotonComponent");
		desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::UserAdded });
	};

private:
	CPhotonListener m_PhotonListener;
	ExitGames::LoadBalancing::Client m_LoadBalancingClient;

	int m_photonUpdateFrameCount;
	bool m_isRoomCreated;
	int m_photonTestEventSentCount = 300;
};