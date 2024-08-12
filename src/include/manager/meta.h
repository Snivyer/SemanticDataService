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
        virtual bool putMetaWithJson(std::string path) {};
        virtual bool putMetaWithCSV(std::string path )  {};
        virtual bool putMetaWithParquet(std::string path) {};

        void setMetaStore(MetaStore* &ms) {
            _store = ms;
        }
        
        MetaStore* getMetaStore() {
            return _store;
        }




    private:
        MetaStore* _store;



    };


}
