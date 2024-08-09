#include "abstract/buffer/base/buffer.h"


namespace SDS
{

    Buffer::Buffer(const std::string type, const size_t fixedSize)
                :type(type), fixedSize(fixedSize)
    {

    }

    void Buffer::resize(const size_t size)
    {

    }

    void Buffer::reset(const bool resetAbsolutePosition, const bool zeroInitialize)
    {
 
    }

    char *Buffer::data() noexcept 
    {
        return nullptr;
    }

    const char *Buffer::data() const noexcept
    {
        return nullptr;
    }

    size_t Buffer::getAvailableSize() const
    {
        if (fixedSize > 0 && fixedSize >= position)
        {
            return fixedSize - position;
        }   
        return 0;
    }
    
    void Buffer::deleteBuffer()
    {
   
    }

} 
