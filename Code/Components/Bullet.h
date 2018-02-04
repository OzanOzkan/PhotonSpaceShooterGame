#pragma once

#include "GamePlugin.h"
#include <CryEntitySystem/IEntityComponent.h>
#include <CryEntitySystem/IEntitySystem.h>
#include <DefaultComponents/Geometry/StaticMeshComponent.h>
#include <DefaultComponents/Physics/BoxPrimitiveComponent.h>
#include <DefaultComponents/Physics/RigidBodyComponent.h>
#include <DefaultComponents/Effects/ParticleComponent.h>

class CBullet : public IEntityComponent
{
public:
	CBullet() : m_DestroyTimer(0) {};
	~CBullet() {};

	virtual void Initialize() override;
	virtual uint64 GetEventMask() const override { return BIT64(ENTITY_EVENT_UPDATE) | BIT64(ENTITY_EVENT_COLLISION); }
	virtual void ProcessEvent(SEntityEvent& event) override;

	// Reflect type to set a unique identifier for this component
	static void ReflectType(Schematyc::CTypeDesc<CBullet>& desc)
	{
		desc.SetGUID("{5BF3F2C0-8447-447F-8F38-3496148F6C80}"_cry_guid);
		desc.SetLabel("Bullet");
		desc.SetEditorCategory("MyComponents");
		desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::UserAdded });
	}

private:
	void FrameUpdate();
	void OnCollision(EventPhysCollision *pCollision);

private:
	Cry::DefaultComponents::CStaticMeshComponent* m_pStaticMeshComponent = nullptr;
	Cry::DefaultComponents::CBoxPrimitiveComponent* m_pBoxPrimitiveComponent = nullptr;
	Cry::DefaultComponents::CRigidBodyComponent* m_pRigidBodyComponent = nullptr;
	Cry::DefaultComponents::CParticleComponent* m_pSparkParticleComponent = nullptr;

	float m_DestroyTimer;
	SEntitySpawnParams m_sparkParticleSpawnParams;
};