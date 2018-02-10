#pragma once

#include <CryEntitySystem/IEntityComponent.h>

class IPhotonCommon
{
public:
	// Todo: Subject to change according to future network actions.
	enum DataKey
	{
		ePDKEY_ENTITY_ID = 0,
		ePDKEY_WORLDTM,
		ePDKEY_STATE,
		ePDKEY_DATA_SIZE
	};

	struct SPhotonSerializedData
	{
	public:
		SPhotonSerializedData() {};

		Matrix34 worldTM;
		bool isFiring;
	};
};