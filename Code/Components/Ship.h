#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CrySchematyc/CoreAPI.h>
#include <DefaultComponents/Physics/RigidBodyComponent.h>
#include <DefaultComponents/Physics/BoxPrimitiveComponent.h>
#include <DefaultComponents/Geometry/StaticMeshComponent.h>

#include <vector>
#include <map>

#include "ShipPropulsion.h"
#include "Weapon.h"

class IShip : public IEntityComponent
{
public:
	IShip() : m_iHealth(0), m_fMaxSpeed(0.f) {};
	virtual ~IShip() {};

	virtual void Initialize() override;
	virtual void InitShip() {};

	void Fire();
	void SetHealth(int health) { m_iHealth = health; }
	int GetHealth() { return m_iHealth; }
	float GetMaxSpeed() { return m_fMaxSpeed; }
	void setVelocity(Vec3 velocity);
	Vec3 getVelocity();

	static void ReflectType(Schematyc::CTypeDesc<IShip>& desc)
	{
		desc.SetGUID("{F6C23919-7690-4932-8BFD-5A340CCABCA9}"_cry_guid);
	};

protected:
	int m_iHealth;
	std::vector<CWeapon*> m_vWeapons;
	float m_fMaxSpeed;

	Cry::DefaultComponents::CStaticMeshComponent* m_pStaticMeshComponent = nullptr;
	Cry::DefaultComponents::CRigidBodyComponent* m_pRigidBodyComponent = nullptr;
	Cry::DefaultComponents::CBoxPrimitiveComponent* m_pBoxPrimitiveComponent = nullptr;
};

class CPlayerShip : public IShip
{
public:
	CPlayerShip() {};
	virtual ~CPlayerShip() {};

	virtual uint64 GetEventMask() const override { return BIT64(ENTITY_EVENT_UPDATE); }
	virtual void ProcessEvent(SEntityEvent& event) override;

	virtual void InitShip() override;

	void setRotationWithMouseInput(const float &mouseRotationX, const float &mouseRotationY);
	void setPositionWithInput();

	static void ReflectType(Schematyc::CTypeDesc<CPlayerShip>& desc)
	{
		desc.SetGUID("{831FD1CD-3B76-4891-9B41-0D079BFCF498}"_cry_guid);
		desc.SetEditorCategory("MyComponents");
		desc.SetLabel("PlayerShip");
		desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::UserAdded });
	};

private:
	void updateShipRotation();
};