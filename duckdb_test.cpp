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




int main()
{
    MetaStore* meta = new MetaStore();
    //工作一： 调整代码，论证duckdb如何用于持久化元数据
    std::string sql = "CREATE TABLE SSDESC (geoName VARCHAR, adCode VARCHAR, geoCentral DOUBLE[2], geoPerimeter DOUBLE[2][4])";
    meta->excuteNonQuery(sql);

    ContentMeta cnt[3];

    cnt[0].extractSSDesc(std::string("江西省"));
    cnt[1].extractSSDesc(std::string("江西省"), std::string("赣州市"));
    cnt[2].extractSSDesc(std::string("江西省"), std::string("赣州市"), std::string("兴国县"));

    cnt[0].printSSDesc();
    cnt[1].printSSDesc();
    cnt[2].printSSDesc();

    cnt[0].putSSDesc(meta);
    cnt[1].putSSDesc(meta);
    cnt[2].putSSDesc(meta);

    std::string path("./metadata/SSDesc/");
    cnt[0].putMetaWithJson(path, meta);
    cnt[0].putMetaWithCSV(path, meta);
    cnt[0].putMetaWithParquet(path, meta);

    meta->excuteNonQuery("SELECT * FROM './metadata/SSDesc/ssdesc.parquet'");


    // 工作二： 论证如何通过duckdb从GIS Json中提取空间元数据，并存储到空间描述符中

    // loadTable(meta);
    
    // duckdb::unique_ptr<duckdb::MaterializedQueryResult> result;
    // std::vector<ContentMeta> desc_list;


    // std::string sql = "SELECT * FROM SSDESC";
    // meta->excuteQuery(sql, result);

    // duckdb::idx_t row_count = result->RowCount();

    // for (idx_t row = 0; row < row_count; row++) {
    //     ContentMeta d;
    //     d.desc->ssDesc.geoName = result->GetValue(0, row).GetValue<std::string>();
    //     d.desc->ssDesc.adCode =  result->GetValue(1, row).GetValue<std::string>();
    //     auto list1 =  result->GetValue(2, row).GetValue<std::string>();
    //     auto list2 =  result->GetValue(3, row);

    //     cout << list1 << std::endl;
    //     cout << list2 << std::endl;
    //     d.printSSDesc();
    // }


    return 0;
}



