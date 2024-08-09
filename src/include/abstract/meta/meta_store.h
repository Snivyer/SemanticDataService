/* Author: Snivyer
*  Create on: 2024/3/26
*  Description:
    using DuckDB to persist metadata.
*/

#pragma once

#include "third_party_lib/duckdb/include/duckdb.hpp"
#include <vector>

using namespace duckdb;

namespace SDS {

    class MetaStore {
    public:
        MetaStore();
        ~MetaStore();

        void connect();
        void disconnect();
        bool excuteNonQuery(std::string sql);
        bool excuteQuery(std::string sql, duckdb::unique_ptr<duckdb::MaterializedQueryResult> &result);
        Appender* getAppender(std::string tableName);

    private: 
        DuckDB* _db;
        Connection* _con;

    };
}

