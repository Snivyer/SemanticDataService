/* Author: Snivyer
*  Create on: 2023/11/15
*  Description:
   STL Buffer, learning from adios2 buffer_STL.h
*/

#ifndef STL_BUFFER_H
#define STL_BUFFER_H_

#include "buffer.h"
#include <vector>

namespace SDS
{
    class STLBuffer : public Buffer
    {
    public:
        std::vector<char> buffer;

        STLBuffer();
        ~STLBuffer() = default;

        char *data() noexcept final;
        const char *data() const noexcept final;

        void resize(const size_t size, const std::string hint) final;

        void reset(const bool resetAbsolutePosition,
               const bool zeroInitialize) final;

        size_t getAvailableSize() const final;

        // 这个函数，我不懂什么意思，所以没有实现
        template <class T>
        size_t align() const noexcept;


        void deleteBuffer();

        size_t debugGetSize() const;

    };


}

#endif