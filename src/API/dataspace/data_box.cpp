#include "API/dataspace/data_box.h"


namespace SDS
{

    DataBox::DataBox() {

    }

    DataBox::~DataBox() {
        
    }


    void DataBox::init(ContentDesc &cntDesc) {

        // todo 首先将元数据描述符存储到元数据存储区
        putContentMeta(cntDesc);

        // todo 开辟一定大小的数据空间
    }

  

    void DataBox::putContentMeta(ContentDesc &cntDesc) {

        auto buffer = meta.buffer;
        auto position = meta.position;


        // 计算所需要的内容元数据空间大小
        size_t descTotalSize = 0;
        SSDesc ssDesc = cntDesc.ssDesc;
        descTotalSize += ssDesc.adCode.size() + ssDesc.geoName.size() 
                        + sizeof(GeoCoordinate) * ssDesc.geoPerimeter.size()
                        + sizeof(GeoCoordinate);
        
        TSDesc tsDesc = cntDesc.tsDesc;
        time_t reportTime = mktime(&tsDesc.reportT);
        time_t startTime = mktime(&(tsDesc.startT));
        time_t endTime = mktime(&(tsDesc.endT));
        descTotalSize += sizeof(time_t) * 4 + sizeof(tsDesc.count);

        VLDesc vlDesc = cntDesc.vlDesc;
        descTotalSize += vlDesc.varListName.size() + sizeof(vlDesc.groupLen) + vlDesc.desc.size();
        size_t varNum = vlDesc.desc.size();
        for(auto varDesc: vlDesc.desc) {
            descTotalSize += varDesc.varType.size() + varDesc.varName.size() 
                            + sizeof(varDesc.varLen) + varDesc.shape.size()
                            + sizeof(varDesc.resRation) + sizeof(varDesc.ncVarID)
                            + sizeof(varDesc.ncGroupID);
        }

        // 数据箱子中数据的总长度为：变量组总长度 + 时间点数量 
        size_t dataTotolSize = vlDesc.groupLen * tsDesc.count;
        descTotalSize += 1;

        meta.resize(position + descTotalSize);
        copyToBuffer<size_t>(buffer, position, &dataTotolSize, sizeof(dataTotolSize));

        // 填充空间描述符
        copyToBuffer<char>(buffer, position, ssDesc.geoName.data(), ssDesc.geoName.size());
        copyToBuffer<char>(buffer, position, ssDesc.adCode.data(), ssDesc.adCode.size());
        copyToBuffer<double>(buffer, position, &(ssDesc.geoCentral.logitude), sizeof(ssDesc.geoCentral.logitude));
        copyToBuffer<double>(buffer, position, &(ssDesc.geoCentral.latitude), sizeof(ssDesc.geoCentral.latitude));
        for(auto geo: ssDesc.geoPerimeter) {
            copyToBuffer<double>(buffer, position, &(geo.logitude), sizeof(geo.logitude));
            copyToBuffer<double>(buffer, position, &(geo.latitude), sizeof(geo.latitude));
        }

        // 填充时间描述符
        copyToBuffer<int>(buffer, position, &(tsDesc.count), sizeof(tsDesc.count));
        copyToBuffer<time_t>(buffer, position, &(reportTime), sizeof(reportTime));
        copyToBuffer<time_t>(buffer, position, &(startTime), sizeof(startTime));
        copyToBuffer<time_t>(buffer, position, &(endTime), sizeof(endTime));

        // 填充变量组描述符
        copyToBuffer<int>(buffer, position, &(vlDesc.groupLen), sizeof(vlDesc.groupLen));
        copyToBuffer<size_t>(buffer, position, &varNum, 1);
        copyToBuffer<char>(buffer, position, vlDesc.varListName.data(), vlDesc.varListName.size());
        for(auto varDesc: vlDesc.desc) {
            copyToBuffer<char>(buffer, position, varDesc.varName.data(), varDesc.varName.size());
            copyToBuffer<char>(buffer, position, varDesc.varType.data(), varDesc.varType.size());
            copyToBuffer<int>(buffer, position, &(varDesc.varLen), sizeof(varDesc.varLen));
            for(auto shapeLen: varDesc.shape) {
                copyToBuffer<size_t>(buffer, position, &(shapeLen), 1);
            }
            copyToBuffer<double>(buffer, position, &(varDesc.resRation), sizeof(varDesc.resRation));
            copyToBuffer<int>(buffer, position, &(varDesc.ncVarID), sizeof(varDesc.ncVarID));
            copyToBuffer<int>(buffer, position, &(varDesc.ncGroupID), sizeof(varDesc.ncGroupID));
        }
    }

    void addSpaceIndex(size_t start, size_t count) {

    }




    // 在元数据区填充数据箱子的尾注，尾注中包含数据箱子的版本和索引的起始地址
    void DataBox::putFooter(FootRegion &footer) {

        auto buffer = meta.buffer;
        auto position = meta.position;

        meta.resize(position + sizeof(FootRegion));
        copyToBuffer<int8_t>(buffer, position, &(footer.version), sizeof(footer.version));
        copyToBuffer<uint16_t>(buffer, position, &(footer.SRIndexStart), sizeof(footer.SRIndexStart));
        copyToBuffer<uint16_t>(buffer, position, &(footer.TRIndexStart), sizeof(footer.TRIndexStart));
        copyToBuffer<uint16_t>(buffer, position, &(footer.VRIndexStart), sizeof(footer.VRIndexStart));
        copyToBuffer<int8_t>(buffer, position, &(footer.isLittleEndian), sizeof(footer.isLittleEndian));
    }




    ResizeResult DataBox::resizeBuffer(const size_t dataIn) {
        const size_t currentSize = this->data.buffer.size();
        const size_t requiredSize = dataIn + this->data.position;
        const size_t maxBufferSize = 500;

        ResizeResult result = ResizeResult::Unchanged;

        if(dataIn > maxBufferSize) {

        }

        if(requiredSize <= currentSize) {

        }
        else if (requiredSize > maxBufferSize) {
            if (currentSize < maxBufferSize)  {
                this->data.resize(maxBufferSize);
            }
             result = ResizeResult::Flush;
        }
        else // buffer must grow
        {
            if (currentSize < maxBufferSize)
            {
                const float growthFactor = 2;
                const size_t nextSize = growthFactor * currentSize;
                this->data.resize(nextSize);
                result = ResizeResult::Success;
            }
        }

        return result;
    }

    void DataBox::resetBuffer(Buffer &buffer, const bool resetAbsolutePosition,
                         const bool zeroInitialize) {
        buffer.reset(resetAbsolutePosition, zeroInitialize);
    }

    void DataBox::deleteBuffers() {
        this->data.deleteBuffer();
        this->meta.deleteBuffer();
    }
}