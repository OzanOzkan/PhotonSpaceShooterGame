#include "StdAfx.h"
#include "Weapon.h"

static void RegisterWeaponComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CWeapon));
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterWeaponComponent);

// Called on entity initialization
void CWeapon::Initialize()
{

}

// Called for every event
void CWeapon::ProcessEvent(SEntityEvent& event)
{
	switch (event.event)
	{
	case ENTITY_EVENT_UPDATE:
	{
		FrameUpdate();
	}
	}
}

void CWeapon::SetLocalPosition(Vec3 localPosition)
{
	// Set this entitycomponent's local position on the parent entity
	Matrix34 localTM = GetTransformMatrix();
	localTM.SetColumn(3, localPosition);
	SetTransformMatrix(localTM);
}

// Fires the weapon
void CWeapon::Fire()
{
	if (m_fLastFireTime >= 50.f)
	{
		SEntitySpawnParams sBulletSpawnParams;
		sBulletSpawnParams.pClass = gEnv->pEntitySystem->GetClassRegistry()->GetDefaultClass();
		sBulletSpawnParams.vPosition = GetWorldTransformMatrix().GetColumn3();
		sBulletSpawnParams.qRotation = IDENTITY;

		IEntity* pSpawnedBullet = gEnv->pEntitySystem->SpawnEntity(sBulletSpawnParams);
		pSpawnedBullet->GetOrCreateComponentClass<CBullet>();

		m_fLastFireTime = 0;
	}
}

// Called on every frame
void CWeapon::FrameUpdate()
{
	if(m_fLastFireTime < 50.f)
		++m_fLastFireTime;


	//Debug
	gEnv->pRenderer->GetIRenderAuxGeom()->DrawSphere(GetWorldTransformMatrix().GetColumn3(), 0.5f, Col_Red);
}