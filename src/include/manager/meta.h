/* Author: Snivyer
*  Create on: 2023/11/8
*  Description:
   Metadata is abstract class for metadata mamagement, 
   which storing metadata with LevelDB
*/

#ifndef METADATA_H_
#define METADATA_H_


namespace SDS
{
    using Key = std::vector<int>;          // 查询key
    
    class Metadata
    {
    private:
        virtual std::string getMeta(Key key);
        virtual void putMeta(Key key, std::string value);


    };


}

#endif