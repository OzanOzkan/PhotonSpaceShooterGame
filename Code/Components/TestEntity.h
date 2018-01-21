#pragma once

#include "GamePlugin.h"
#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntitySystem.h>
#include <DefaultComponents/Geometry/StaticMeshComponent.h>
#include <DefaultComponents/Physics/BoxPrimitiveComponent.h>
#include <DefaultComponents/Physics/RigidBodyComponent.h>

class CTestEntity : public IEntityComponent
{
public:
	CTestEntity() {};
	~CTestEntity() {};

	virtual void Initialize() override;
	virtual uint64 GetEventMask() const override { return BIT64(ENTITY_EVENT_UPDATE) | BIT64(ENTITY_EVENT_COLLISION); }
	virtual void ProcessEvent(SEntityEvent& event) override;

	// Reflect type to set a unique identifier for this component
	static void ReflectType(Schematyc::CTypeDesc<CTestEntity>& desc)
	{
		desc.SetGUID("{1305FA0D-D205-486D-B84C-A639F9988373}"_cry_guid);
		desc.SetEditorCategory("MyComponents");
		desc.SetLabel("TestEntity");
		desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::UserAdded });
	}

private:
	Cry::DefaultComponents::CStaticMeshComponent* m_pStaticMeshComponent = nullptr;
	Cry::DefaultComponents::CBoxPrimitiveComponent* m_pBoxPrimitiveComponent = nullptr;
	Cry::DefaultComponents::CRigidBodyComponent* m_pRigidBodyComponent = nullptr;
};