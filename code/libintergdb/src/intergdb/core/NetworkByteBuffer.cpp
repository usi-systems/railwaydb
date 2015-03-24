#include <intergdb/core/NetworkByteBuffer.h>

using namespace intergdb::core;

NetworkByteBuffer::NetworkByteBuffer()
  : autoDealloc(true), buf(NULL),
    size(DEFAULT_SIZE), icursor(0), ocursor(0)
{
    if(size==0)
        buf = NULL;
    else
        buf = alloc(size);
}

NetworkByteBuffer::NetworkByteBuffer(uint64_t initialSize)
    : autoDealloc(true), buf(NULL),
      size(initialSize), icursor(0), ocursor(0)
{
    if(size==0)
        buf = NULL;
    else
        buf = alloc(size);
}

NetworkByteBuffer::NetworkByteBuffer(unsigned char * mybuf, uint64_t mysize)
    : autoDealloc(false),
// autoDealloc is false because we're just using a buffer allocated elsewhere
// and thus someone else should take care of deallocating it for us
      buf(mybuf), size(mysize), icursor(mysize), ocursor(0)
{}

NetworkByteBuffer::NetworkByteBuffer(NetworkByteBuffer const & r)
    : autoDealloc(true), buf(NULL), size(r.size),
      icursor(r.icursor), ocursor(r.ocursor)
{
    buf = alloc(size);
    memcpy(buf,r.buf,size);
}

NetworkByteBuffer::NetworkByteBuffer(NetworkByteBuffer & r, bool isDestructive)
    : autoDealloc(true), buf(NULL), size(r.size),
    icursor(r.icursor), ocursor(r.ocursor)
{
    // a destructive copy will destroy the original object
    if(isDestructive) {
        buf = r.buf;
        r.autoDealloc = true;
        r.buf = NULL;
        r.size = 0;
        r.icursor = 0;
        r.ocursor = 0;
    }
    // a non-destructive copy will duplicate the original object
    else {
        buf = alloc(size);
        memcpy(buf,r.buf,size);
    }
}

NetworkByteBuffer & NetworkByteBuffer::assign(NetworkByteBuffer const & sb)
{
    if(this!=&sb) {
        autoDealloc = true;
        size = sb.size;
        icursor = sb.icursor;
        ocursor = sb.ocursor;
        dealloc(buf);
        buf = alloc(size);
        memcpy(buf,sb.buf,size);
    }
    return *this;
}

NetworkByteBuffer & NetworkByteBuffer::copyData(NetworkByteBuffer const & sb)
{
    if(this != &sb) {
        if(size < sb.icursor)
            resize(sb.icursor - size);
        memcpy(buf,sb.buf,sb.icursor);
        icursor = sb.icursor;
    }
    ocursor = 0;
    return *this;
}

bool NetworkByteBuffer::operator ==(NetworkByteBuffer const & sb) const
{
    if(getSerializedDataSize() != sb.getSerializedDataSize())
        return false;
    if(memcmp(buf,sb.buf,getSerializedDataSize()) != 0)
        return false;
    return true;
}

NetworkByteBuffer::~NetworkByteBuffer()
{
    if(autoDealloc) {
        dealloc(buf);
        buf = NULL;
    }
}
