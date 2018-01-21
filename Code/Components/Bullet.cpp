#include "StdAfx.h"
#include "Bullet.h"

#include "Ship.h"

#include <CryRenderer/IRenderAuxGeom.h>

static void RegisterBulletComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CBullet));
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterBulletComponent);


// Called on entity creation time
void CBullet::Initialize()
{
	m_pBoxPrimitiveComponent = GetEntity()->GetOrCreateComponent<Cry::DefaultComponents::CBoxPrimitiveComponent>();
	m_pBoxPrimitiveComponent->m_size = Vec3(0.5f, 4.5f, 0.2f);
	Matrix34 boxComponentTM = m_pBoxPrimitiveComponent->GetTransformMatrix();
	boxComponentTM.SetColumn(3, Vec3(0.5, 4.5, 0));
	m_pBoxPrimitiveComponent->SetTransformMatrix(boxComponentTM);

	// Static mesh component
	m_pStaticMeshComponent = GetEntity()->GetOrCreateComponent<Cry::DefaultComponents::CStaticMeshComponent>();
	m_pStaticMeshComponent->SetFilePath("objects/bullet/bullet.cgf");

	Cry::DefaultComponents::SPhysicsParameters &physParams = m_pStaticMeshComponent->GetPhysicsParameters();
	physParams.m_mass = 0;

	m_pStaticMeshComponent->LoadFromDisk();
	m_pStaticMeshComponent->ResetObject();
	// ~Static mesh component

	IMaterial* pMaterial = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial("objects/bullet/bullet");
	GetEntity()->SetMaterial(pMaterial);

	m_pRigidBodyComponent = GetEntity()->GetOrCreateComponent<Cry::DefaultComponents::CRigidBodyComponent>();

	//GetEntity()->PrePhysicsActivate(true);
}

void CBullet::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_UPDATE:
	{
		FrameUpdate();
	}
	break;
	case ENTITY_EVENT_COLLISION:
	{
		OnCollision(reinterpret_cast<EventPhysCollision*>(event.nParam[0]));
	}
	}
}

void CBullet::FrameUpdate()
{
	if (GetEntity())
	{
		Vec3 forwardDir = GetEntity()->GetForwardDir();
		Matrix34 entityTM = GetEntity()->GetWorldTM();
		entityTM.SetTranslation(entityTM.GetTranslation() + (forwardDir * 8.f));
		GetEntity()->SetWorldTM(entityTM);
	}

	++m_DestroyTimer;

	if (m_DestroyTimer >= 2000.f)
	{
		gEnv->pEntitySystem->RemoveEntity(GetEntity()->GetId());
	}

	//Debug
	//gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(GetWorldTransformMatrix().GetColumn3(), 0.5f, Col_Red);
}

void CBullet::OnCollision(EventPhysCollision *pCollision)
{
	CryLogAlways("Bullet %i collided.", GetEntity()->GetId());
	gEnv->pEntitySystem->RemoveEntity(GetEntity()->GetId());
}