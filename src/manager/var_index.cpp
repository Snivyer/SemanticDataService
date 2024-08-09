#include "manager/var_index.h"

namespace SDS
{

    // 初始化空间索引
    VarIndex::VarIndex(struct IndexEntrance &entrance):BaseIndex(entrance)
    {

    }

    // 析构函数
    VarIndex::~VarIndex()
    {

    }

    // 解析检索词
    bool  VarIndex::getTerm(SearchTerm &term, std::string &varName)
    {
       
    
    }



    bool VarIndex::search(SearchTerm &term, ResultSet &result)
    {
        
     

       

    }

    // 这里还有大问题，节点没有传参数进来，哈哈哈哈哈
    bool VarIndex::insert(SearchTerm &term, ResultSet &result)
    {

       
        


    }

    bool VarIndex::remove(SearchTerm &term, ResultSet &result)
    {

    }
    
    bool VarIndex::update(SearchTerm &oldTerm, SearchTerm &newTerm, ResultSet &result)
    {

    }     
    
    bool VarIndex::persist(std::string fileName)
    {

    }
    


}