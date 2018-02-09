#include "StdAfx.h"
#include "Player.h"

#include <CryRenderer/IRenderAuxGeom.h>

#include <CryGame/GameUtils.h>

static void RegisterPlayerComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPlayerComponent));
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPlayerComponent);

void CPlayerComponent::Initialize()
{
	//m_pShip->SetTransformMatrix(Matrix34::Create(Vec3(1.f), IDENTITY, Vec3(0, 30, -12.f)));

	// Create the camera component, will automatically update the viewport every frame
	m_pCameraComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CCameraComponent>();
	
	// Get the input component, wraps access to action mapping so we can easily get callbacks when inputs are triggered
	m_pInputComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CInputComponent>();

	// Register an action, and the callback that will be sent when it's triggered
	m_pInputComponent->RegisterAction("player", "moveleft", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::MoveLeft, activationMode);  });
	// Bind the 'A' key the "moveleft" action
	m_pInputComponent->BindAction("player", "moveleft", eAID_KeyboardMouse, EKeyId::eKI_A);

	m_pInputComponent->RegisterAction("player", "moveright", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::MoveRight, activationMode);  });
	m_pInputComponent->BindAction("player", "moveright", eAID_KeyboardMouse, EKeyId::eKI_D);

	m_pInputComponent->RegisterAction("player", "moveforward", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::MoveForward, activationMode);  });
	m_pInputComponent->BindAction("player", "moveforward", eAID_KeyboardMouse, EKeyId::eKI_W);

	m_pInputComponent->RegisterAction("player", "moveback", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::MoveBack, activationMode);  });
	m_pInputComponent->BindAction("player", "moveback", eAID_KeyboardMouse, EKeyId::eKI_S);

	m_pInputComponent->RegisterAction("player", "fire", [this](int activationMode, float value) { HandleInputFlagChange((TInputFlags)EInputFlag::Fire, activationMode);  });
	m_pInputComponent->BindAction("player", "fire", eAID_KeyboardMouse, EKeyId::eKI_Mouse1);

	m_pInputComponent->RegisterAction("player", "mouse_rotateyaw", [this](int activationMode, float value) { m_mouseDeltaRotation.x -= value; });
	m_pInputComponent->BindAction("player", "mouse_rotateyaw", eAID_KeyboardMouse, EKeyId::eKI_MouseX);

	m_pInputComponent->RegisterAction("player", "mouse_rotatepitch", [this](int activationMode, float value) { m_mouseDeltaRotation.y -= value; });
	m_pInputComponent->BindAction("player", "mouse_rotatepitch", eAID_KeyboardMouse, EKeyId::eKI_MouseY);

	m_pPhotonComponent = GetEntity()->GetOrCreateComponent<CPhotonComponent>();

	Revive();

	//cameraTM.SetTranslation(Vec3(0, -3.f, 1.f));
}

uint64 CPlayerComponent::GetEventMask() const
{
	return BIT64(ENTITY_EVENT_START_GAME) | BIT64(ENTITY_EVENT_UPDATE);
}

void CPlayerComponent::SpawnShip()
{
	// Spawn the ship
	SEntitySpawnParams shipSpawnParams;
	shipSpawnParams.sName = "PlayerShip";
	shipSpawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
	shipSpawnParams.vPosition = Vec3(GetEntity()->GetPos().x, GetEntity()->GetPos().y, GetEntity()->GetPos().z);
	shipSpawnParams.qRotation = IDENTITY;
	IEntity* pSpawnedShip = gEnv->pEntitySystem->SpawnEntity(shipSpawnParams);
	m_pShip = pSpawnedShip->GetOrCreateComponent<CPlayerShip>();
	
	cameraTM.SetTranslation(m_pShip->getCameraOffset());
}

void CPlayerComponent::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_START_GAME:
	{
		// Revive the entity when gameplay starts
		Revive();
		SpawnShip();
	}
	break;
	case ENTITY_EVENT_UPDATE:
	{
		SEntityUpdateContext* pCtx = (SEntityUpdateContext*)event.nParam[0];

		float shipYaw = 0.f;

		// Check input to calculate local space velocity
		if (m_inputFlags & (TInputFlags)EInputFlag::MoveLeft)
		{
			shipYaw = 0.01f;
		}
		if (m_inputFlags & (TInputFlags)EInputFlag::MoveRight)
		{
			shipYaw = -0.01f;
		}
		if (m_inputFlags & (TInputFlags)EInputFlag::MoveForward)
		{
			m_pShip->setSpeed(0.01f);
		}
		if (m_inputFlags & (TInputFlags)EInputFlag::MoveBack)
		{
			m_pShip->setSpeed(-0.001f);
		}
		if (m_inputFlags & (TInputFlags)EInputFlag::Fire)
		{
			m_pShip->Fire();
		}

		m_pShip->setRotation(m_mouseDeltaRotation.x, m_mouseDeltaRotation.y, shipYaw);

		m_mouseDeltaRotation = ZERO;

		Matrix34 cameraMatrix = m_pShip->GetEntity()->GetWorldTM() * cameraTM;
		Vec3 cameraPos = GetEntity()->GetPos();
		Interpolate(cameraPos, cameraMatrix.GetTranslation(), 5.f, pCtx->fFrameTime);
		cameraMatrix.SetTranslation(cameraPos);
		
		if(cameraMatrix.IsValid())
			GetEntity()->SetWorldTM(cameraMatrix);
	}
	break;
	}
}

void CPlayerComponent::Revive()
{
	// Set player transformation, but skip this in the Editor
	if (!gEnv->IsEditor())
	{
	// 
		Vec3 playerScale = Vec3(1.f);
		Quat playerRotation = IDENTITY;

		// Position the player in the center of the map
		const float heightOffset = 20.f;
		float terrainCenter = gEnv->p3DEngine->GetTerrainSize() / 2.f;
		float height = gEnv->p3DEngine->GetTerrainZ(static_cast<int>(terrainCenter), static_cast<int>(terrainCenter));
		Vec3 playerPosition = Vec3(terrainCenter, terrainCenter, height + heightOffset);

		m_pEntity->SetWorldTM(Matrix34::Create(playerScale, playerRotation, playerPosition));
	}

	// Unhide the entity in case hidden by the Editor
	GetEntity()->Hide(false);

	// Reset input now that the player respawned
	m_inputFlags = 0;
	m_mouseDeltaRotation = ZERO;
}

void CPlayerComponent::HandleInputFlagChange(TInputFlags flags, int activationMode, EInputFlagType type)
{
	switch (type)
	{
	case EInputFlagType::Hold:
	{
		if (activationMode == eIS_Released)
		{
			m_inputFlags &= ~flags;
		}
		else
		{
			m_inputFlags |= flags;
		}
	}
	break;
	case EInputFlagType::Toggle:
	{
		if (activationMode == eIS_Released)
		{
			// Toggle the bit(s)
			m_inputFlags ^= flags;
		}
	}
	break;
	}
}