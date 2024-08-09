/* Author: Snivyer
*  Create on: 2023/11/8
*  Description:
   Metadata is abstract class for metadata mamagement, 
   which storing metadata with LevelDB
*/

#pragma once
#include <string>
#include <vector>
#include "abstract/meta/meta_store.h"


namespace SDS
{
    using Key = std::vector<int>;          // 查询key
    
    class Metadata
    {

    public:
        virtual std::string getMeta(Key key) {};
        virtual void putMeta(Key key, std::string value) {};
        virtual void putMetaWithJson(std::string path, MetaStore* &ms) {};
        virtual void putMetaWithCSV(std::string path, MetaStore* &ms)  {};
        virtual void putMetaWithParquet(std::string path, MetaStore* &ms) {};
    };


}
