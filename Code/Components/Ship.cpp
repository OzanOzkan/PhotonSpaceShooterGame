#include <StdAfx.h>
#include "Ship.h"

static void RegisterShipComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CShip));
		scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPlayerShip));
		scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CEnemyDestroyer));
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterShipComponent);

void CShip::Initialize()
{
	m_pBoxPrimitiveComponent = GetEntity()->GetOrCreateComponent<Cry::DefaultComponents::CBoxPrimitiveComponent>();
	m_pBoxPrimitiveComponent->m_bReactToCollisions = false;

	m_pStaticMeshComponent = GetEntity()->GetOrCreateComponent<Cry::DefaultComponents::CStaticMeshComponent>();

	m_pRigidBodyComponent = GetEntity()->GetOrCreateComponent<Cry::DefaultComponents::CRigidBodyComponent>();
	m_pRigidBodyComponent->m_bSendCollisionSignal = true;

	ShipInit();
}

void CShip::ProcessEvent(SEntityEvent & event)
{
	// Common event implementations for ships.
	switch (event.event)
	{
	case ENTITY_EVENT_UPDATE:
	{
		if (m_Health <= 0.f)
		{
			CryLogAlways("Ship %i destroyed!", GetEntity()->GetId()); // Debug
		}
	}
	break;
	case ENTITY_EVENT_COLLISION:
	{
		EventPhysCollision* physCollision = reinterpret_cast<EventPhysCollision*>(event.nParam[0]);

		if (physCollision->pEntity[1])
		{
			IEntity* pOtherEntity = gEnv->pEntitySystem->GetEntityFromPhysics(physCollision->pEntity[1]);
			if (pOtherEntity->GetComponent<CBullet>())
			{
				CryLogAlways("This is a bullet! - ID: %i - Health: %f", pOtherEntity->GetId(), m_Health); // Debug
				m_Health -= 0.5f;
			}
		}
	}
	}

	// Dispatch the event to ships for further personalized actions.
	ShipEvent(event);
}

void CShip::Fire()
{
	for (CWeapon* currentWeapon : m_vWeapons)
	{
		currentWeapon->Fire();
	}
}

void CShip::setVelocity(Vec3 velocity)
{
	m_pRigidBodyComponent->SetVelocity(velocity); // Todo: speed limits
}

Vec3 CShip::getVelocity()
{
	return m_pRigidBodyComponent->GetVelocity();
}

void CShip::LoadModel(const char* modelPath)
{
	// Load the ship model
	m_pStaticMeshComponent->SetFilePath(modelPath);

	Cry::DefaultComponents::SPhysicsParameters &physParams = m_pStaticMeshComponent->GetPhysicsParameters();
	physParams.m_mass = 0;

	m_pStaticMeshComponent->LoadFromDisk();
	m_pStaticMeshComponent->ResetObject();
	// ~Load the ship model
}

//////////////////////////////////////////////////////////////////////////////////
//	CPlayerShip																	//
//////////////////////////////////////////////////////////////////////////////////
void CPlayerShip::ShipInit()
{
	//"objects/testobjects/SF_Free-Fighter.cgf"
	LoadModel("objects/arc/ARC170.cgf");

	//// Box primitive component
	//m_pBoxPrimitiveComponent->m_size = Vec3(0.6f, 0.8f, 0.3f);
	//Matrix34 boxComponentTM = m_pBoxPrimitiveComponent->GetTransformMatrix();
	//boxComponentTM.SetColumn(3, Vec3(0, 0, -0.3));
	//m_pBoxPrimitiveComponent->SetTransformMatrix(boxComponentTM);
	//// ~Box primitive component

	// Particle emitter for floating starts
	m_pParticleComponent = GetEntity()->CreateComponent<Cry::DefaultComponents::CParticleComponent>();
	m_pParticleComponent->SetEffectName("particles/stars.pfx");
	m_pParticleComponent->Activate(true);
	// ~Particle emitter for floating stars

	// Create weapons.
	CWeapon* pWeapon = m_pEntity->CreateComponent<CWeapon>();
	pWeapon->SetLocalPosition(Vec3(17.50001, 17, -2.2));
	m_vWeapons.push_back(pWeapon);

	CWeapon* pWeapon2 = m_pEntity->CreateComponent<CWeapon>();
	pWeapon2->SetLocalPosition(Vec3(-17.50001, 17, -2.2));
	m_vWeapons.push_back(pWeapon2);
	// ~Create weapons.
}

void CPlayerShip::ShipEvent(SEntityEvent & event)
{
	Vec3 forwardDir = GetEntity()->GetForwardDir();
	GetEntity()->SetPos(GetEntity()->GetPos() + (forwardDir * m_fCurrentSpeed));
}

void CPlayerShip::setRotation(const float &mouseRotationX, const float &mouseRotationY, const float &shipYaw)
{
	/* 
	// This commented implementation is not working. 
	// Something brokes the rendering or camera and suddenly everything dissapears at 90 degrees of rotation and beyond.

	Ang3 angle = Ang3(DEG2RAD(mouseRotationY), DEG2RAD(-mouseRotationX), shipYaw);

	Matrix34 tm = GetEntity()->GetWorldTM();
	Matrix34 angrot = Matrix34::CreateRotationXYZ(angle);
	tm = tm * angrot;

	if(tm.IsValid())
		GetEntity()->SetWorldTM(tm);
	*/

	Ang3 angle = Ang3(DEG2RAD(mouseRotationY * 0.08f), DEG2RAD(-mouseRotationX * 0.08f), shipYaw * 0.008f);

	Matrix34 tm = GetEntity()->GetWorldTM();
	Matrix33 angrot = Matrix33::CreateRotationXYZ(angle);
	tm = tm * angrot;
	angle = CCamera::CreateAnglesYPR(Matrix33(tm));
	tm.SetRotation33(CCamera::CreateOrientationYPR(angle));
	GetEntity()->SetWorldTM(tm);
}

void CPlayerShip::setSpeed(const float &speed)
{
	if ((speed > 0.f && m_fCurrentSpeed < m_fMaxSpeed) || (speed < 0.f && m_fCurrentSpeed > 0.f))
		m_fCurrentSpeed += speed;
}

//////////////////////////////////////////////////////////////////////////////////
//	CEnemyDestroyer																//
//////////////////////////////////////////////////////////////////////////////////
void CEnemyDestroyer::ShipInit()
{
	// Load the ship model
	LoadModel("objects/testobjects/SF_Corvette-F3.cgf");

	// TODO: Box collider size adjustment.
	m_pBoxPrimitiveComponent->m_size = Vec3(75.f, 105.f, 10.f);

	m_pBoxPrimitiveComponent->AddPrimitive();
}

void CEnemyDestroyer::ShipEvent(SEntityEvent & event)
{

}
