/* Author: Snivyer
*  Create on: 2023/11/15
*  Description:
   STL Buffer, learning from adios2 buffer_STL.h
*/

#pragma once

#include "abstract/buffer/base/buffer.h"
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

        void resize(const size_t size) final;

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

