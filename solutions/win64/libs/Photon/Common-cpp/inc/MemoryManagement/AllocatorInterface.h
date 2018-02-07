/* Exit Games Common - C++ Client Lib
 * Copyright (C) 2004-2017 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/defines.h"

namespace ExitGames
{
	namespace Common
	{
		namespace MemoryManagement
		{
			/**
			   Custom Allocators to be used with Photons Memory Management need to inherit and implement this interface.
			   The allocator that is used used by Photon can be set through setAllocator(). */
			class AllocatorInterface
			{
			public:
				/**
				   Destructor. */
				virtual ~AllocatorInterface(void){}

				/**
				   This function gets called by MemoryManagement::setMaxAllocSize() and an implementation is required to behave as explained in the documentation of that function. */
				virtual void setMaxAllocSize(size_t maxAllocSize) = 0;
				
				/**
				   This function gets called by #EG_MALLOC and an implementation is required to behave as explained in the documentation of that macro. */
				virtual void* alloc(size_t size) = 0;
				
				/**
				   This function gets called by #EG_FREE and an implementation is required to behave as explained in the documentation of that macro. */
				virtual void dealloc(void* p) = 0;
				
				/**
				   This function gets called by #EG_REALLOC and an implementation is required to behave as explained in the documentation of that macro. */
				virtual void* resize(void* p, size_t size) = 0;
			};
		}
	}
}