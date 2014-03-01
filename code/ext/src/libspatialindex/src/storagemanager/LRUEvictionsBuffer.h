#pragma once

#include "Buffer.h"

#include <list>
#include <unordered_map>

namespace SpatialIndex
{
	namespace StorageManager
	{
		class LRUEvictionsBuffer : public Buffer
		{
		public:
			LRUEvictionsBuffer(IStorageManager&, Tools::PropertySet& ps);
		        // see Buffer.h for available properties.

			virtual ~LRUEvictionsBuffer();

			virtual void addEntry(id_type page, Buffer::Entry* pEntry);
			virtual void removeEntry();
                        virtual void processPageAccess(id_type page);
                private:
                        std::list<id_type> accesses_;
                        std::unordered_map<id_type, std::list<id_type>::iterator > pages_;
		}; // LRUEvictionsBuffer
	}
}
