/* Author: Snivyer
*  Create on: 2023/11/15
*  Description:
   Buffer class, learning from adios2 buffer.h
*/

#ifndef BUFFER_H
#define BUFFER_H_

#include <string>

namespace SDS
{
    class Buffer
    {
    public:

        // 缓存类型
        const std::string type;

        // 是否支持拓展，为0可以拓展，否则缓存具有固定大小
        const size_t fixedSize = 0;

        // 有效数据位置
        size_t position; 

        size_t absolutedPosition;

        Buffer(const std::string type, const size_t fixedSize = 0);

        virtual ~Buffer() = default;

        virtual char *data() noexcept;
        virtual const char *data() const noexcept;
        virtual void resize(const size_t size, const std::string hint);

        /**
        * 将缓存的偏移位置置为0
        * @param resetAbsolutePosition true: 重置 absolutePosition = 0
        * @param zeroInitialize 会在缓存内容中填充 '\0'
        */
        virtual void reset(const bool resetAbsolutePosition,
                       const bool zeroInitialize);

        virtual size_t getAvailableSize() const;

        /** Deletes buffer memory manually */
        virtual void deleteBuffer();

    };


}

#endif