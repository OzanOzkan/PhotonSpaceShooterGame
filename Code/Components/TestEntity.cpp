#include "TestEntity.h"

static void RegisterTestComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CTestEntity));
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterTestComponent);

void CTestEntity::Initialize()
{
	m_pBoxPrimitiveComponent = GetEntity()->GetOrCreateComponent<Cry::DefaultComponents::CBoxPrimitiveComponent>();
	m_pBoxPrimitiveComponent->m_size = Vec3(60.f, 60.f, 60.f);

	m_pStaticMeshComponent = GetEntity()->GetOrCreateComponent<Cry::DefaultComponents::CStaticMeshComponent>();

	m_pRigidBodyComponent = GetEntity()->GetOrCreateComponent<Cry::DefaultComponents::CRigidBodyComponent>();

	m_pStaticMeshComponent->SetFilePath("objects/testobjects/SF_Corvette-F3.cgf");

	Cry::DefaultComponents::SPhysicsParameters &physParams = m_pStaticMeshComponent->GetPhysicsParameters();
	physParams.m_mass = 0;

	m_pStaticMeshComponent->LoadFromDisk();
	m_pStaticMeshComponent->ResetObject();
}


void CTestEntity::ProcessEvent(SEntityEvent & event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_COLLISION:
	{
		CryLogAlways("TEST ENTITY COLLISION");

		EventPhysCollision* physCollision = reinterpret_cast<EventPhysCollision*>(event.nParam[0]);

		if (physCollision->pEntity[1])
		{
			IEntity* pOtherEntity = gEnv->pEntitySystem->GetEntityFromPhysics(physCollision->pEntity[1]);
			if (pOtherEntity->GetComponent<CBullet>())
			{
				CryLogAlways("This is a bullet! - ID: %i", pOtherEntity->GetId());
				gEnv->pEntitySystem->RemoveEntity(pOtherEntity->GetId());
			}
		}
	}
	}
}
