#include "abstract/meta/meta_store.h"

namespace SDS {


MetaStore::MetaStore() {
    _db = new DuckDB(nullptr);
    _con = nullptr;
}

MetaStore::~MetaStore() {
    if(_db != nullptr) {
        delete _db;
    }
}

void MetaStore::connect() {
    if(_con == nullptr) {
        _con = new Connection(*_db);
    } 
}

void MetaStore::disconnect() {
    if(_con != nullptr) {
        delete _con;
        _con = nullptr;
    }
}

bool MetaStore::excuteNonQuery(std::string sql) {

    if(_con == nullptr) {
        connect();
    }

    _con->Query(sql)->Print();
    disconnect();
}

bool MetaStore::excuteQuery(std::string sql, duckdb::unique_ptr<duckdb::MaterializedQueryResult> &result) {


    if(_con == nullptr) {
        connect();
    }

    result = _con->Query(sql);
    return true;

}

Appender* MetaStore::getAppender(std::string tableName) {

    if(_con == nullptr) {
        connect();
    }
    return new Appender(*_con, tableName);
}

}
