#include "ShipPropulsion.h"

static void RegisterShipPropulsionComponent(Schematyc::IEnvRegistrar& registrar)
{
	Schematyc::CEnvRegistrationScope scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	{
		Schematyc::CEnvRegistrationScope componentScope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CShipPropulsion));
	}
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&RegisterShipPropulsionComponent);

void CShipPropulsion::Initialize()
{
	m_pEntity->LoadGeometry(GetOrMakeEntitySlotId(), "objects/sphere.cgf");
}

void CShipPropulsion::ProcessEvent(SEntityEvent& event)
{

}

void CShipPropulsion::Push()
{

}
