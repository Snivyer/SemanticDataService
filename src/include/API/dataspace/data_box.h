/* Author: Snivyer
*  Create on: 2023/11/15
*  Description:
   data box is a basic structure in memory to manage data in term of space, time and variables. 
*/

#pragma once

#include "abstract/buffer/malloc/STL_buffer.h"
#include "abstract/buffer/memory/memory.h"
#include "manager/cnt_meta.h"
#include "manager/sto_meta.h"
#include <unordered_set>
#include <unordered_map>
#include <cstddef>

namespace SDS
{

    // 元数据区
    struct FootRegion {
        uint16_t SRIndexStart = 0;      // 空间索引区偏移地址
        uint16_t TRIndexStart = 0;      // 时间索引区偏移地址
        uint16_t VRIndexStart = 0;      // 变量索引区偏移地址
        int8_t  version = -1;           // 版本
        int8_t isLittleEndian = 1;     // 是否采用小端方式

        FootRegion(const int8_t version): version(version) {}
    };

    // 元数据索引表，用于构建空间索引、时间索引以及变量索引
    struct SerialElementIndex
    {
        std::vector<char> buffer;      // 索引区缓存大小，默认200bytes
        uint16_t count;                // 元素个数
        const uint32_t  memberID;      // 唯一ID
        size_t lastUpadatePosition;    // 上次更新元素的地址
        bool valid;                    // 当前元素是否合法

        SerialElementIndex(const uint32_t memberID, const size_t buffSize = 200):memberID(memberID) {
            this->buffer.reserve(buffSize);
        }

    };

    // 元数据集合
    struct MetadataSet
    {

        // 数据箱子索引表
        SerialElementIndex DataBoxIndex = SerialElementIndex(0);

 
        /** 空间索引表 @brief key: Space ID, value: Space index  */
        std::unordered_map<std::string, SerialElementIndex> SpaceIndices;

        /** 时间索引表 @brief key: TimeSlot ID, value: TimeSlot index  */
        std::unordered_map<std::string, SerialElementIndex> TimeSlotIndices;

        /** 时间索引表 @brief key: Time ID, value: TimePoint index  */
        std::unordered_map<std::string, SerialElementIndex> TimeIndices;


        /** 变量列表索引表 @brief key: variable List ID, value: attribute list index  */
        std::unordered_map<std::string, SerialElementIndex> varListIndices;

        /** 变量索引表 @brief key: variable name, value: variable index  */
        std::unordered_map<std::string, SerialElementIndex> varsIndices;

        /** 属性索引表 @brief key: attribute name, value: attribute index  */
        std::unordered_map<std::string, SerialElementIndex> attrsIndices;

        // 当前数据箱子数量 
        uint64_t dataBoxCount = 0;

        // 当前数据箱子的起始/相对位置
        size_t dataBoxLengthPosition = 0;

        // 当前数据箱子中的空间数量
        uint32_t dataBoxSpaceCount = 0;
        
        // 当前数据箱子中空间的起始/相对位置
        size_t dataBoxSpaceLengthPosition = 0;

        // 当前数据箱子中的时间段数量
        uint32_t dataBoxTimeSlotCount = 0;

         // 当前数据箱子中时间段的起始/相对位置
        size_t dataBoxTimeSlotLengthPosition = 0;

        // 当前数据箱子中的变量列表数量
        uint32_t dataBoxVarListCount = 0;

        // 当前数据箱子中变量列表的起始/相对位置
        size_t dataBoxVarListLengthPosition = 0;

        // 当前数据箱子中的变量数量
        uint32_t dataBoxVarCount = 0;

        // 当前数据箱子中变量的起始/相对位置
        size_t dataBoxVarLengthPosition = 0;

        // 当前的数据箱子是否在写
        bool dataBoxIsOpen = false;

        // 数据箱子索引偏移地址
        size_t  dataBoxIndexStart;

        // 空间索引偏移地址
        size_t spaceIndexStart;

        // 时间段索引偏移地址
        size_t timeSlotIndexStart;

        // 时间点索引偏移地址
        size_t timePointIndexStart;

        // 变量列表索引偏移地址
        size_t varListIndexStart;

        // 变量索引偏移地址
        size_t varIndexStart;

        // 属性索引偏移地址
        size_t attrIndexStart;
    };


    enum ResizeResult
    {
        Failure,   //!< FAILURE, caught a std::bad_alloc
        Unchanged, //!< UNCHANGED, no need to resize (sufficient capacity)
        Success,   //!< SUCCESS, resize was successful
        Flush      //!< FLUSH, need to flush to transports for current variable
    };


    // 数据箱子类
    class DataBox
    {
    public:        
        
        // 数据缓存
        STLBuffer data;

        // 元数据缓存
        STLBuffer meta;

        // 元数据索引
        struct MetadataSet metaSet;

        // 是否是行存
        bool isRowMajor = true;

        DataBox();
        ~DataBox();

        void Init(ContentDesc &cntDesc);


        ResizeResult resizeBuffer(const size_t dataIn);

        void resetBuffer(Buffer &buffer, const bool resetAbsolutePosition = false,
                     const bool zeroInitialize = true);
        
        void deleteBuffers();

        size_t debugGetDataBufferSize() const;

    private:
        void PutFooter(FootRegion &footer);
        void PutContentMeta(ContentDesc &cntDesc);

    };


};
   
