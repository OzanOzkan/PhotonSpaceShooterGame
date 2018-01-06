#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CrySchematyc/CoreAPI.h>

class CShipPropulsion : public IEntityComponent
{
public:
	CShipPropulsion() {};
	virtual ~CShipPropulsion() {};

	virtual void Initialize() override;
	virtual uint64 GetEventMask() const override { return BIT64(ENTITY_EVENT_UPDATE); }
	virtual void ProcessEvent(SEntityEvent& event) override;

	void Push();

	static void ReflectType(Schematyc::CTypeDesc<CShipPropulsion>& desc)
	{
		desc.SetGUID("{6B8C33C8-FF01-4BF1-B50D-106CB23570DE}"_cry_guid);
		desc.SetEditorCategory("MyComponents");
		desc.SetLabel("ShipPropulsion");
		desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::UserAdded });
	};
};