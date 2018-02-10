#pragma once

#include <CrySchematyc/CoreAPI.h>
#include <CryEntitySystem/IEntityComponent.h>
#include <CryGame/GameUtils.h>
#include <DefaultComponents/Debug/DebugDrawComponent.h>

#include "IPhotonCommon.h"
#include "PhotonEntityComponent.h"

#include "../Ship.h"


// Necessary undefs for Photon. 
// Definitions overlaps with CRYENGINE.
#undef alloc
#undef malloc
#undef free
#undef realloc
#undef calloc
#undef STRINGIFY
#include "Photon-cpp/inc/PhotonPeer.h"
#include "Common-cpp/inc/Common.h"
#include "LoadBalancing-cpp/inc/Listener.h"
#include "LoadBalancing-cpp/inc/Client.h"

class CPhotonClientComponent 
	: public IEntityComponent
	, public ExitGames::LoadBalancing::Listener
	, public IPhotonCommon
{
public:
	CPhotonClientComponent();
	virtual ~CPhotonClientComponent();

	//////////////////////////////////////////////////////////////
	// IEntityComponent											//
	//////////////////////////////////////////////////////////////
	virtual void Initialize() override;
	virtual uint64 GetEventMask() const override { return BIT64(ENTITY_EVENT_UPDATE); }
	virtual void ProcessEvent(SEntityEvent& event) override;


	static void ReflectType(Schematyc::CTypeDesc<CPhotonClientComponent>& desc)
	{
		desc.SetGUID("{C687992A-AB4D-4F98-B134-5049C31693B5}"_cry_guid);
		desc.SetEditorCategory("Photon Engine");
		desc.SetLabel("Photon Client Component");
		desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::UserAdded });
	};

	//////////////////////////////////////////////////////////////
	// ExitGames::LoadBalancing::Listener						//
	//////////////////////////////////////////////////////////////
	virtual void debugReturn(int debugLevel, const ExitGames::Common::JString & string) override;
	virtual void connectionErrorReturn(int errorCode) override;
	virtual void clientErrorReturn(int errorCode) override;
	virtual void warningReturn(int warningCode) override;
	virtual void serverErrorReturn(int errorCode) override;
	virtual void joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player & player) override;
	virtual void leaveRoomEventAction(int playerNr, bool isInactive) override;
	virtual void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object & eventContent) override;
	virtual void connectReturn(int errorCode, const ExitGames::Common::JString & errorString, const ExitGames::Common::JString & cluster) override;
	virtual void disconnectReturn(void) override;
	virtual void createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable & roomProperties, const ExitGames::Common::Hashtable & playerProperties, int errorCode, const ExitGames::Common::JString & errorString) override;
	virtual void joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable & roomProperties, const ExitGames::Common::Hashtable & playerProperties, int errorCode, const ExitGames::Common::JString & errorString) override;
	virtual void joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable & roomProperties, const ExitGames::Common::Hashtable & playerProperties, int errorCode, const ExitGames::Common::JString & errorString) override;
	virtual void joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable & roomProperties, const ExitGames::Common::Hashtable & playerProperties, int errorCode, const ExitGames::Common::JString & errorString) override;
	virtual void leaveRoomReturn(int errorCode, const ExitGames::Common::JString & errorString) override;
	virtual void joinLobbyReturn(void) override;
	virtual void leaveLobbyReturn(void) override;

	void CreateRemotePlayer(int playerId, const char* playerName);
	void UpdateInfoTexts();

private:
	int m_localPlayerId;
	bool m_isPhotonConnected;
	ExitGames::LoadBalancing::Client m_LoadBalancingClient;
	std::map<int, CPhotonEntityComponent*> m_localNetworkedEntities;
	std::map<int, IEntity*> m_remoteNetworkedEntities;
	bool m_firstInit = true;
	bool m_firstRoomJoin = true;
	int m_playerCount;

	Cry::DefaultComponents::CDebugDrawComponent* m_pDebugDrawComponent = nullptr;
};