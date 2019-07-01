#pragma once

#include <string>

#include "leveldb/db.h"

namespace dt {
    namespace db {
        leveldb::DB* open_database();
        leveldb::Status put(leveldb::DB* db, std::string key, std::string value);

        template <typename Func>
            void map(leveldb::DB* db, Func&& function) {
            auto it = db->NewIterator(leveldb::ReadOptions());
            for (it->SeekToFirst(); it->Valid(); it->Next()) {
                function(it->key().ToString(), it->value().ToString());
            }
            assert(it->status().ok());  // Check for any errors found during the scan
            delete it;
        }
    }
}
