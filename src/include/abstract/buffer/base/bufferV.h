/* Author: Snivyer
*  Create on: 2024/7/19
*  Description:
   BufferV class, learning from adios2 bufferV.h
*/

#ifndef BUFFERV_H
#define BUFFERV_H_

#include <vector>
#include <string>

namespace SDS
{

    struct iovec {
        const void *iov_base;
        size_t iov_len;
    };

    class BufferV {
    public:
        const std::string m_Type;
        const size_t m_MemAlign;                      
        const size_t m_MemBlockSize;

        uint64_t Size() noexcept;

        BufferV(const std::string type, const bool AlwaysCopy = false,
                const size_t MemAlign = 1, const size_t MemBlockSize = 1);
        virtual ~BufferV();

    virtual std::vector<iovec> DataVec() noexcept = 0;

    virtual void Reset();

    virtual size_t AddToVec(const size_t size, const void *buf, size_t align,
                            bool CopyReqd,
                            MemorySpace MemSpace = MemorySpace::Host) = 0;

    struct BufferPos
    {
        int bufferIdx = -1;     // buffer index
        size_t posInBuffer = 0; // position in buffer[idx]
        size_t globalPos = 0;   // global position in virtual buffer
        BufferPos(int idx, size_t pos, size_t globalPos)
        : bufferIdx(idx), posInBuffer(pos), globalPos(globalPos){};
    };

    /** Allocate size bytes and return BufferPos position.
     * Used by Span functions to allocate memory on behalf of the user
     * Return both the position in the virtual memory buffer as well
     * as all info needed to retrieve a valid pointer any time
     * during execution (even after reallocs)
     */
    virtual BufferPos Allocate(const size_t size, size_t align) = 0;
    virtual void DownsizeLastAlloc(const size_t oldSize,
                                   const size_t newSize) = 0;

    void AlignBuffer(const size_t align);

    virtual void *GetPtr(int bufferIdx, size_t posInBuffer) = 0;

protected:
    std::vector<char> zero;
    const bool m_AlwaysCopy = false;

    struct VecEntry
    {
        bool External;
        const void *Base;
        size_t Offset;
        size_t Size;
    };
    std::vector<VecEntry> DataV;
    size_t CurOffset = 0;
    size_t m_internalPos = 0;
};


}


#endif
