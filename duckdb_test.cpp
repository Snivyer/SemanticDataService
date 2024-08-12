#include "manager/cnt_meta.h"
#include "abstract/meta/meta_store.h"
#include "abstract/GIS/geo_read.h"
#include "third_party_lib/duckdb/include/duckdb.hpp"
#include <iostream>
#include <vector>
#include <io.h>
using namespace std;
using namespace SDS;
using namespace duckdb;


// 测试一：duckdb如何用于持久化元数据
// ContentMeta: 元数据操作类
// MetaStore: 元数据持久化类，其目前采用duckDB进行元数据持久化
void MetaPeristTest() {

    MetaStore* metaStore = new MetaStore();
    ContentMeta cntMetaManager;
    ContentDesc cntDesc[3];
    std::vector<std::string> geoNames;

    geoNames.push_back(std::string("江西省"));
    cntMetaManager.extractSSDesc(cntDesc[0], geoNames);
    geoNames.push_back(std::string("赣州市"));
    cntMetaManager.extractSSDesc(cntDesc[1], geoNames);
    geoNames.push_back(std::string("兴国县"));
    cntMetaManager.extractSSDesc(cntDesc[2], geoNames);

    // 打印内容元数据
    for(int i = 0; i < 3; i++) {
        cntMetaManager.printSSDesc(cntDesc[i]);
    }

    cntMetaManager.setMetaStore(metaStore);
    cntMetaManager.setSSDescFormat();

    // 元数据持久化
    for(int i = 0; i < 3; i++) {
        cntMetaManager.putSSDesc(cntDesc[i]);
    }

    // 元数据输出成不同文件
    std::string path("./metadata/SSDesc/");

    cntMetaManager.putMetaWithJson(path);
    cntMetaManager.putMetaWithCSV(path);
    cntMetaManager.putMetaWithParquet(path);
}


// 测试二：duckdb如何用于加载元数据
// ContentMeta: 元数据操作类
// MetaStore: 元数据持久化类，其目前采用duckDB进行元数据持久化
void MetaLoadTest() {

    MetaStore* ms = new MetaStore();
    ContentMeta cntMetaManager;
    duckdb::unique_ptr<duckdb::MaterializedQueryResult> result;
    std::vector<ContentDesc> cntDescSet; 


    std::string fileName = "./metadata/SSDesc/2024-08-10-14-49-56.json";

    // 从文件中加载数据
    cntMetaManager.setMetaStore(ms);
    cntMetaManager.setSSDescFormat();
    cntMetaManager.loadSSDescWithFile(fileName);
    
    std::string sql = "SELECT * FROM SSDESC";
    ms->excuteQuery(sql, result);
    cntMetaManager.parseSSDesc(cntDescSet, result.get());

    for(auto cntDesc : cntDescSet) {
        cntMetaManager.printSSDesc(cntDesc);
    }

}



int main()
{
   // MetaPeristTest();
   MetaLoadTest();
    

 







    return 0;
}



