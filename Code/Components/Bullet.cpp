#include "StdAfx.h"
#include "Bullet.h"

#include "Ship.h"

#include <CryRenderer/IRenderAuxGeom.h>

// Called on entity creation time
void CBullet::Initialize()
{
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

	//SEntityPhysicalizeParams sPhysicsParams;
	//sPhysicsParams.type = PE_RIGID;
	//sPhysicsParams.mass = 0.f;
	//
	//GetEntity()->Physicalize(sPhysicsParams);

	GetEntity()->PrePhysicsActivate(true);
}

void CBullet::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_UPDATE:
	{
		FrameUpdate();
	}
	case ENTITY_EVENT_COLLISION:
	{
		OnCollision((EventPhysCollision*)event.nParam[0]);
	}
	}
}

void CBullet::FrameUpdate()
{
	if (IEntity* pEntity = GetEntity())
	{
		Vec3 forwardDir = pEntity->GetForwardDir();
		pEntity->SetPos(pEntity->GetPos() + (forwardDir * 2.f));
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
		
}