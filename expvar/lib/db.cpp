#include "db.h"

leveldb::DB* dt::db::open_database() {
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    printf("Opening database\n");
    leveldb::Status status = leveldb::DB::Open(options, "./test.db", &db);
    assert(status.ok());
    return db;
}

leveldb::Status dt::db::put(leveldb::DB* db, std::string key, std::string value) {
    return db->Put(leveldb::WriteOptions(), key, value);
}
