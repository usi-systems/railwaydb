#include <time.h>
#include <stdlib.h>
#include <cmath>

#include <spatialindex/SpatialIndex.h>
#include "LRUEvictionsBuffer.h"

using namespace SpatialIndex;
using namespace SpatialIndex::StorageManager;

IBuffer* SpatialIndex::StorageManager::returnLRUEvictionsBuffer(IStorageManager& sm, Tools::PropertySet& ps)
{
	IBuffer* b = new LRUEvictionsBuffer(sm, ps);
	return b;
}

IBuffer* SpatialIndex::StorageManager::createNewLRUEvictionsBuffer(IStorageManager& sm, uint32_t capacity, bool bWriteThrough)
{
	Tools::Variant var;
	Tools::PropertySet ps;

	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = capacity;
	ps.setProperty("Capacity", var);

	var.m_varType = Tools::VT_BOOL;
	var.m_val.blVal = bWriteThrough;
	ps.setProperty("WriteThrough", var);

	return returnLRUEvictionsBuffer(sm, ps);
}

LRUEvictionsBuffer::LRUEvictionsBuffer(IStorageManager& sm, Tools::PropertySet& ps) 
  : Buffer(sm, ps)
{
}

LRUEvictionsBuffer::~LRUEvictionsBuffer()
{
}

void LRUEvictionsBuffer::addEntry(id_type page, Entry* e)
{
	assert(m_buffer.size() <= m_capacity);

	if (m_buffer.size() == m_capacity) 
          removeEntry();
	assert(m_buffer.find(page) == m_buffer.end());
	m_buffer.insert(std::pair<id_type, Entry*>(page, e));

        accesses_.push_front(page);
        pages_[page] = accesses_.begin();
}

void LRUEvictionsBuffer::removeEntry()
{
	if (m_buffer.size() == 0) return;

	id_type page = accesses_.back();
        assert(m_buffer.count(page)>0);

	std::map<id_type, Entry*>::iterator it = m_buffer.find(page);

	if ((*it).second->m_bDirty)
	{
		id_type page = (*it).first;
		m_pStorageManager->storeByteArray(page, ((*it).second)->m_length, (const byte *) ((*it).second)->m_pData);
	}

	delete (*it).second;
	m_buffer.erase(it);

        pages_.erase(page);
        accesses_.pop_back();
}

void LRUEvictionsBuffer::processPageAccess(id_type page)
{
        std::list<id_type>::iterator it = pages_[page];
        if (it==accesses_.begin())
                return;
        accesses_.erase(it);
        accesses_.push_front(page);
        pages_[page] = accesses_.begin();
}
