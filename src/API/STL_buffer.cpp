#include "API/STL_buffer.h"

#include <cstdlib>
#include <cstring>

namespace SDS
{

    STLBuffer::STLBuffer() : Buffer("BufferSTL")
    {

    }

    char *STLBuffer::data() noexcept
    {
        return this->buffer.data();
    }

    const char *STLBuffer::data() const noexcept
    {
        return this->buffer.data();
    }

    void STLBuffer::resize(const size_t size, const std::string hint)
    {
        try
        {
            // 这样做将有效取代 STL GNU 默认的 2 次幂重新分配。
            buffer.reserve(size);
            buffer.resize(size, '\0');
        }
        catch (...)
        {
 
        }
    }

    void STLBuffer::reset(const bool resetAbsolutePosition,
                      const bool zeroInitialize)
    {
        this->position = 0;
        if (resetAbsolutePosition)
        {
            this->absolutedPosition= 0;
        }

        if (zeroInitialize)
        {
            std::fill(this->buffer.begin(), this->buffer.end(), 0);
        }
        else
        {
            // just zero out the first and last 1kb
            const size_t bufsize = this->buffer.size();
            size_t s = (bufsize < 1024 ? bufsize : 1024);
            std::fill_n(this->buffer.begin(), s, 0);

            if (bufsize > 1024)
            {
                size_t pos = bufsize - 1024;
                if (pos < 1024)
                {
                    pos = 1024;
                }
                s = bufsize - pos;
                std::fill_n(std::next(this->buffer.begin(), pos), s, 0);
            }
        }
    }

    size_t STLBuffer::getAvailableSize() const
    {
        return this->buffer.size() - this->position;
    }

    void STLBuffer::deleteBuffer() 
    {
        std::vector<char>().swap(this->buffer); 
    }

    template <class T>
    size_t STLBuffer::align() const noexcept
    {

    }
    
    size_t STLBuffer::debugGetSize() const 
    { 
        return this->buffer.size();
    };

} 

