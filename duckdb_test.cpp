#include "manager/cnt_meta.h"
#include "abstract/meta/meta_store.h"
#include "abstract/GIS/geo_read.h"
#include "third_party_lib/duckdb/include/duckdb.hpp"
#include <iostream>
#include <vector>
using namespace std;
using namespace SDS;
using namespace duckdb;


// 测试一：duckdb如何用于持久化元数据
// ContentMeta: 元数据操作类
// MetaStore: 元数据持久化类，其目前采用duckDB进行元数据持久化
void MetaPeristTest() {

    MetaStore* metaStore = new MetaStore();
    ConnectConfig config;
    config.rootPath = "./data/ldasin/fcst/";
    Adaptor* adaptor = new LocalAdaptor(config);
    ContentMeta cntMetaManager;
    ContentDesc cntDesc[3];

    cntMetaManager.setAdaptor(adaptor);

    // 填充空间描述信息
    std::vector<std::string> geoNames;
    geoNames.push_back(std::string("江西省"));
    cntMetaManager.extractSSDesc(cntDesc[0], geoNames);
    geoNames.push_back(std::string("赣州市"));
    cntMetaManager.extractSSDesc(cntDesc[1], geoNames);
    geoNames.push_back(std::string("兴国县"));
    cntMetaManager.extractSSDesc(cntDesc[2], geoNames);

    // 打印空间元数据
    for(int i = 0; i < 3; i++) {
        cntMetaManager.printSSDesc(cntDesc[i]);
    }

    // 填充时间段描述信息
    cntMetaManager.extractTSDesc(cntDesc[0], "./data/ldasin/fcst/33-20220620T000000/");
    cntMetaManager.extractTSDesc(cntDesc[1], "./data/ldasin/fcst/33-20220803T000000/");
    cntMetaManager.extractTSDesc(cntDesc[2], "./data/ldasin/fcst/44-20221208T000000/");

    // 打印时间元数据
    for(int i = 0; i < 3; i++) {
        cntMetaManager.printTSDesc(cntDesc[i]);
    }

    // 填充变量列表描述信息
    cntMetaManager.extractVLDesc(cntDesc[0], "./data/ldasin/fcst/33-20220620T000000/");
    cntMetaManager.extractVLDesc(cntDesc[1], "./data/ldasin/fcst/33-20220803T000000/");
    cntMetaManager.extractVLDesc(cntDesc[2], "./data/ldasin/fcst/44-20221208T000000/");

     // 打印变量元数据
    for(int i = 0; i < 3; i++) {
        cntMetaManager.printVLDesc(cntDesc[i]);
    }


    //  配置元数据存储schema
    cntMetaManager.setMetaStore(metaStore);
    cntMetaManager.setSSDescFormat();
    cntMetaManager.setTSDescFormat();
    cntMetaManager.setVLDescFormat();


    // 元数据持久化
    for(int i = 0; i < 3; i++) {
        cntMetaManager.putSSDesc(cntDesc[i]);
        cntMetaManager.putTSDesc(cntDesc[i]);
        cntMetaManager.putVLDesc(cntDesc[i], std::to_string(i));
    }

    // 元数据输出成不同文件
    std::string path("./metadata/");

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
    MetaPeristTest();
   // MetaLoadTest();
    



    return 0;
}



