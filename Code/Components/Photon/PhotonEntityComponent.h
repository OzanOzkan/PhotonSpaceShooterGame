#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CrySchematyc/CoreAPI.h>

#include "IPhotonCommon.h"

// Necessary undefs for Photon. 
// Definitions overlaps with CRYENGINE.
#undef alloc
#undef malloc
#undef free
#undef realloc
#undef calloc
#undef STRINGIFY
#include "Common-cpp/inc/Common.h"

#include <vector>
#include <utility>

class CPhotonEntityComponent final 
	: public IEntityComponent
	, public IPhotonCommon
{
public:
	// IEntityComponent
	virtual void Initialize() override;

	static void ReflectType(Schematyc::CTypeDesc<CPhotonEntityComponent>& desc)
	{
		desc.SetGUID("{{B7F7AD63-3634-493E-AEB4-EF2220F5027D}}"_cry_guid);
		desc.SetEditorCategory("Photon Engine");
		desc.SetLabel("Photon Entity Component");
		desc.SetComponentFlags({ IEntityComponent::EFlags::Transform, IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach, IEntityComponent::EFlags::UserAdded });
	};

	IPhotonCommon::SPhotonSerializedData* GetSerializedData();

public:
	IPhotonCommon::SPhotonSerializedData* m_pPhotonSerializedData = nullptr;
};