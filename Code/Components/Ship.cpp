#include <StdAfx.h>
#include "Ship.h"

static void RegisterShipComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(IShip));
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterShipComponent);

static void RegisterPlayerShipComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPlayerShip));
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPlayerShipComponent);

static void RegisterEnemyDestroyerComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CEnemyDestroyer));
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterEnemyDestroyerComponent);

void IShip::Initialize()
{
	m_pStaticMeshComponent = GetEntity()->GetOrCreateComponent<Cry::DefaultComponents::CStaticMeshComponent>();
	m_pRigidBodyComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CRigidBodyComponent>();
	m_pBoxPrimitiveComponent = m_pEntity->GetOrCreateComponent<Cry::DefaultComponents::CBoxPrimitiveComponent>();

	ShipInit();
}

void IShip::ProcessEvent(SEntityEvent & event)
{
	ShipEvent(event);
}

void IShip::Fire()
{
	for (CWeapon* currentWeapon : m_vWeapons)
	{
		currentWeapon->Fire();
	}
}

void IShip::setVelocity(Vec3 velocity)
{
	m_pRigidBodyComponent->SetVelocity(velocity); // Todo: speed limits
}

Vec3 IShip::getVelocity()
{
	return m_pRigidBodyComponent->GetVelocity();
}

void CPlayerShip::ShipInit()
{
	// Load the ship model
	//m_pStaticMeshComponent->SetFilePath("objects/testobjects/SF_Free-Fighter.cgf");
	m_pStaticMeshComponent->SetFilePath("objects/arc/ARC170.cgf");

	Cry::DefaultComponents::SPhysicsParameters &physParams = m_pStaticMeshComponent->GetPhysicsParameters();
	physParams.m_mass = 0;

	m_pStaticMeshComponent->LoadFromDisk();
	m_pStaticMeshComponent->ResetObject();
	// ~Load the ship model

	// Box primitive component
	m_pBoxPrimitiveComponent->m_size = Vec3(0.6f, 0.8f, 0.3f);
	Matrix34 boxComponentTM = m_pBoxPrimitiveComponent->GetTransformMatrix();
	boxComponentTM.SetColumn(3, Vec3(0, 0, -0.3));
	m_pBoxPrimitiveComponent->SetTransformMatrix(boxComponentTM);
	// ~Box primitive component

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

	m_fMaxSpeed = 0.50f;
	m_iHealth = 100;
	m_cameraOffset = Vec3(0, -35.f, 15.f);
}

void CPlayerShip::ShipEvent(SEntityEvent & event)
{
	Vec3 forwardDir = GetEntity()->GetForwardDir();
	GetEntity()->SetPos(GetEntity()->GetPos() + (forwardDir * m_fCurrentSpeed));
}

void CPlayerShip::updateShipRotation()
{
	// CAUTION: Max rotation -/+ 0.50f

	Vec3 currentVelocity = getVelocity();

	Matrix34 transformation = GetEntity()->GetLocalTM();

	Ang3 ypr = CCamera::CreateAnglesYPR(Matrix33(transformation));
	ypr.x = 0;

	if (currentVelocity.z > -0.50f && currentVelocity.z < 0.50f)
		ypr.y = currentVelocity.z * 0.05f;

	if (currentVelocity.x > -0.50f && currentVelocity.x < 0.50f)
		ypr.z = currentVelocity.x * 0.05f;

	transformation.SetRotation33(CCamera::CreateOrientationYPR(ypr));

	GetEntity()->SetLocalTM(transformation);
}

void CPlayerShip::setRotation(const float &mouseRotationX, const float &mouseRotationY, const float &shipYaw)
{
	Ang3 angle = Ang3(DEG2RAD(mouseRotationY), DEG2RAD(-mouseRotationX), shipYaw);

	Matrix34 tm = GetEntity()->GetWorldTM();
	Matrix34 angrot = Matrix34::CreateRotationXYZ(angle);
	tm = tm * angrot;

	if(tm.IsValid())
		GetEntity()->SetWorldTM(tm);
}

void CPlayerShip::setSpeed(const float &speed)
{
	if ((speed > 0.f && m_fCurrentSpeed < m_fMaxSpeed) || (speed < 0.f && m_fCurrentSpeed > 0.f))
		m_fCurrentSpeed += speed;
}

void CEnemyDestroyer::ShipInit()
{
	// Load the ship model
	//m_pStaticMeshComponent->SetFilePath("objects/testobjects/SF_Free-Fighter.cgf");
	m_pStaticMeshComponent->SetFilePath("objects/testobjects/SF_Corvette-F3.cgf");

	Cry::DefaultComponents::SPhysicsParameters &physParams = m_pStaticMeshComponent->GetPhysicsParameters();
	physParams.m_mass = 0;

	m_pStaticMeshComponent->LoadFromDisk();
	m_pStaticMeshComponent->ResetObject();
	// ~Load the ship model

	m_iHealth = 100;

	// TODO: Box collider size adjustment.
}

void CEnemyDestroyer::ShipEvent(SEntityEvent & event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_COLLISION:
	{
		CryLogAlways("CEnemyDestroyer::ShipEvent() - COLLISION");

		EventPhysCollision* physCollision = reinterpret_cast<EventPhysCollision*>(event.nParam[0]);
		if (CBullet* pBullet = gEnv->pEntitySystem->GetEntityFromPhysics(physCollision->pEntity[0])->GetComponent<CBullet>())
		{
			CryLogAlways("ISABET! - ENTITY 0");
		}

		if (CBullet* pBullet = gEnv->pEntitySystem->GetEntityFromPhysics(physCollision->pEntity[1])->GetComponent<CBullet>())
		{
			CryLogAlways("ISABET! - ENTITY 1");
		}
	}
	}
}
