#include <StdAfx.h>
#include "PhotonEntityComponent.h"

static void RegisterPhotonEntityComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPhotonEntityComponent));
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterPhotonEntityComponent);


void CPhotonEntityComponent::Initialize()
{
	m_pPhotonSerializedData = new SPhotonSerializedData();
}

IPhotonCommon::SPhotonSerializedData* CPhotonEntityComponent::GetSerializedData()
{
	m_pPhotonSerializedData->worldTM = GetEntity()->GetWorldTM();

	return m_pPhotonSerializedData;
}