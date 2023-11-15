/* Author: Snivyer
*  Create on: 2023/11/10
*  Description:
   StorageSpace is a data description set which containing storage semantic of data by using ContentID.
*/

#ifndef SEMANTIC_SPACE_H_
#define SEMANTIC_SPACE_H_

#include <string>
#include <vector>
#include "manager/cnt_meta.h"


namespace SDS
{
   
    class SemanticSpace
    {
    public:    
        std::string SSName;         // 语义空间名称
        int spaceID;                // 语义空间ID
        std::string PSID;           // 父语义空间ID
        int childrenNum;            // 子语义空间数量    
        time_t createT;             // 创建时间

        struct ContentID cntID;     // 内容ID描述符
        ContentMeta cntMeta;        // 内容描述符

        int createSemanticSpace(std::string SSName, std::string GeoName);       // 创建语义空间

    };

}

#endif

