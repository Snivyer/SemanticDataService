#include "API/data_box.h"


namespace SDS
{

    FootRegion::FootRegion(const int8_t version): version(version)
    {

    }

    SerialElementIndex::SerialElementIndex(const uint32_t memberID, 
                                            const size_t buffSize = 200):memberID(memberID)
    {
        this->buffer.reserve(buffSize);
    }

    void DataBox::init()
    {

    }

    ResizeResult DataBox::resizeBuffer(const size_t dataIn,const std::string hint)
    {
        const size_t currentSize = this->data.buffer.size();
        const size_t requiredSize = dataIn + this->data.position;
        const size_t maxBufferSize = 500;

        ResizeResult result = ResizeResult::Unchanged;

        if(dataIn > maxBufferSize)
        {

        }

        if(requiredSize <= currentSize)
        {

        }
        else if (requiredSize > maxBufferSize)
        {
            if (currentSize < maxBufferSize)
            {
                this->data.resize(maxBufferSize, " when resizing buffer to " +
                                             std::to_string(maxBufferSize) +
                                             "bytes, " + hint + "\n");
            }
             result = ResizeResult::Flush;
        }
        else // buffer must grow
        {
            if (currentSize < maxBufferSize)
            {
                const float growthFactor = 2;
                const size_t nextSize = growthFactor * currentSize;
                this->data.resize(nextSize, " when resizing buffer to " +
                                        std::to_string(nextSize) + "bytes, " +
                                        hint);
                result = ResizeResult::Success;
            }
        }

        return result;
    }

    void DataBox::resetBuffer(Buffer &buffer, const bool resetAbsolutePosition,
                         const bool zeroInitialize)
    {
        buffer.reset(resetAbsolutePosition, zeroInitialize);
    }

    void DataBox::deleteBuffers()
    {
   
        this->data.deleteBuffer();
        this->meta.deleteBuffer();
    }





}