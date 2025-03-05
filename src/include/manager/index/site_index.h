/* Author: Snivyer
*  Create on: 2025/3/3
*  Description:
   Site Index is a children class of baseIndex, which implement the site index with a simple map.
*/


#pragma once
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include "manager/index/index.h"
#include "manager/metadata/cnt_meta.h"


namespace SDS
{

    struct SiteNode {
        size_t siteID;
        std::string PSiteID;
        std::string siteName;
        std::string siteValue;

        SiteNode* PSiteNode;
        std::vector<SiteNode*> CSNode;

        std::string getCompleteSiteID(int keyLength = 3) {
            return PSiteID + intToStringWithPadding(siteID, keyLength);
        }
    };


    class SiteIndex : BaseIndex {
    public:
        SiteIndex();
        ~SiteIndex();

        bool search(SearchTerm &term, ResultSet &result) override;     
        bool insert(SearchTerm &term, ResultSet &result) override;      
        bool remove(SearchTerm &term, ResultSet &result) override;       
        bool update(SearchTerm &oldTerm, SearchTerm 
                            &newTerm, ResultSet &result) override;     
        bool persist(std::string fileName) override; 
        bool getResult(ResultSet &result, SiteNode* &node);   
        bool getTerm(SearchTerm &term, std::string &siteName);  

        
        bool search(std::string path, SiteNode* &node);          
        
    private:
        bool search(std::string siteName, SiteNode* &node, std::vector<SiteNode*> &searchList);
        bool insert(std::string siteName, SiteNode* &node, SiteNode* pNode);


                   
    };


}
